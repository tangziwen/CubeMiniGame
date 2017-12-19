#pragma once

#include "../../Math/vec3.h"
#include "../../Math/vec4.h"
#include <vector>
namespace tzw
{
	/**
	* @brief The surface class convertes a tile::accessor to an iso-surface. Call this class by one thread at a time.
	* Uses marching cubes and transvoxel for surface-generation.
	* http://www.terathon.com/voxels/
	* To understand this class and algos please read Eric Lengyel¡¯s Dissertation: http://www.terathon.com/lengyel/Lengyel-VoxelTerrain.pdf .
	* If you got a custom voxel, that effects the resulting surface derive this class and reimplement vertexGotCreated() and vertexGotCreatedLod().
	* Private methods are undocumentated because they simply implement the code described in Eric Lengyel¡¯s Dissertation.
	*/
	class surface
	{
	public:
		/**
		* @brief ~surface desctructor
		*/
		~surface()
		{
		}

		/**
		* @brief calculateSurface calculates the iso surface.
		* @param voxel Contains the voxel needed for the surface calculation.
		* @param voxelSize voxel-scale.
		* @param calculateNormalCorrection check chapter 3.3 in Eric Lengyel¡¯s Dissertation.
		* @param lod Lod index starting with 0.
		*/
		void calculateSurface(const vec4 * srcData, const float &voxelSize = 1., const bool& calculateNormalCorrection = true, const int &lod = 0)
		{
			static_cast<t_thiz>(this)->clear();

			m_voxel = voxel;
			m_lod = lod;

			m_vertices.reserve(1000);
			m_indices.reserve(2000);

			const vector3int32 voxelStart(-1);
			const vector3int32 voxelEnd(t_voxelAccessor::voxelLength + 2);

			const int32 vertexIndicesReuseSize(((voxelEnd.x - voxelStart.x) * 3)*((voxelEnd.y - voxelStart.y) * 3)*((voxelEnd.z - voxelStart.z) * 3));
			// the indexer for the vertices. *3 because gets saved with edge-id
			int32 *vertexIndicesReuse = new int32[vertexIndicesReuseSize];
			for (int32 index = 0; index < vertexIndicesReuseSize; ++index)
			{
				vertexIndicesReuse[index] = -1;
			}

			// isLevel describes at which interpolation-level a surface is generated around the voxel
			const int8 isoLevel(0);
			for (int32 x = voxelStart.x; x < voxelEnd.x - 1; ++x)
			{
				for (int32 y = voxelStart.y; y < voxelEnd.y - 1; ++y)
				{
					for (int32 z = voxelStart.z; z < voxelEnd.z - 1; ++z)
					{
						// depending on the voxel-neighbour- the count and look, of the triangles gets calculated.
						uint8 tableIndex(0);
						const vector3int32 posVoxel(x, y, z);
						t_calcVoxel voxelCalc;
						const vector3int32 toCheck[] = {
							vector3int32(0, 0, 0),
							vector3int32(1, 0, 0),
							vector3int32(0, 0, 1),
							vector3int32(1, 0, 1),
							vector3int32(0, 1, 0),
							vector3int32(1, 1, 0),
							vector3int32(0, 1, 1),
							vector3int32(1, 1, 1)
						};
						uint16 toAddToTableIndex(1);
						for (uint16 indCheck = 0; indCheck < 8; ++indCheck)
						{
							voxelCalc[indCheck] = getVoxel(posVoxel + toCheck[indCheck]); // <-- acc to valgrind getVoxel is the most expensivec call;
							if (voxelCalc[indCheck].getInterpolation() < isoLevel) // OPTIMISE reuse "voxelCalc", instead of calling getVoxel 2 times later!
							{
								tableIndex |= toAddToTableIndex;
							}
							toAddToTableIndex *= 2;
						}

						// Now create a triangulation of the isosurface in this
						// cell.
						if (tableIndex == 0 || tableIndex == 255)
						{
							continue;
						}
						bool calculateFaces(true);
						if (calculateNormalCorrection)
						{
							calculateFaces = (posVoxel >= vector3int32(0) && posVoxel < voxelEnd - vector3int32(2));
						}
						// OPTIMISE: too many vertices get calculated because of normalcorrection; optimise!
						const RegularCellData *data = &regularCellData[regularCellClass[tableIndex]];
						int32 ids[12];
						for (int32 ind = 0; ind < data->GetVertexCount(); ++ind)
						{
							int32 data2 = regularVertexData[tableIndex][ind];
							int32 corner0 = data2 & 0x0F;
							int32 corner1 = (data2 & 0xF0) >> 4;
							int32 id = calculateEdgeId(posVoxel, data2 >> 8); // for reuse
							BASSERT(id >= 0);
							BASSERT(id < vertexIndicesReuseSize);

							if (vertexIndicesReuse[id] == -1)
							{
								vector3 point = calculateIntersectionPosition(posVoxel, corner0, corner1); // OPTIMISE so dirty - use voxelCalc inside the method!
																										   // we calucluate here everything in positive values; but normal correction starts @ -1
								point *= voxelSize;
								t_voxel voxel0 = getVoxel(posVoxel + calculateCorner(corner0)); // OPTIMISE so dirty - use voxelCalc!
								t_voxel voxel1 = getVoxel(posVoxel + calculateCorner(corner1));
								const t_vertex vertex(static_cast<t_thiz>(this)->createVertex(posVoxel, voxel0, voxel1, point, vector3()));
								m_vertices.push_back(vertex);
								ids[ind] = vertexIndicesReuse[id] = m_vertices.size() - 1;
							}
							else
							{
								ids[ind] = vertexIndicesReuse[id];
							}
						}
						for (int32 ind = 0; ind < data->GetTriangleCount() * 3; ind += 3)
						{
							// calc triangle and normal
							const int32 vertexIndex0(ids[data->vertexIndex[ind + 0]]);
							const int32 vertexIndex1(ids[data->vertexIndex[ind + 1]]);
							const int32 vertexIndex2(ids[data->vertexIndex[ind + 2]]);
							const vector3 vertex0(m_vertices.at(vertexIndex0).position);
							const vector3 vertex1(m_vertices.at(vertexIndex1).position);
							const vector3 vertex2(m_vertices.at(vertexIndex2).position);
							if (vertex0 == vertex1 || vertex0 == vertex2 || vertex1 == vertex2)
							{
								continue; // triangle with zero space
							}
							const vector3 addNormal = (vertex1 - vertex0).crossProduct(vertex2 - vertex0);//.normalisedCopy();
																										  // if (calculateFaces) // for normal-correction-test
							{
								m_vertices.at(vertexIndex0).normal += addNormal;
								m_vertices.at(vertexIndex1).normal += addNormal;
								m_vertices.at(vertexIndex2).normal += addNormal;
							}
							if (calculateFaces)
							{
								// insert new triangle
								m_indices.push_back(vertexIndex0);
								m_indices.push_back(vertexIndex1);
								m_indices.push_back(vertexIndex2);
							}
						}
					}
				}
			}

			// transvoxel
			if (m_lod > 0)// && false)
			{
				typedef vector3int32 v3i;
				const vector3int32 voxelLookups[][9] = {
					{ v3i(0, 0, 0),v3i(0, 1, 0),v3i(0, 2, 0),v3i(0, 2, 1),v3i(0, 2, 2),v3i(0, 1, 2),v3i(0, 0, 2),v3i(0, 0, 1),v3i(0, 1, 1) },
					{ v3i(0, 0, 0),v3i(1, 0, 0),v3i(2, 0, 0),v3i(2, 0, 1),v3i(2, 0, 2),v3i(1, 0, 2),v3i(0, 0, 2),v3i(0, 0, 1),v3i(1, 0, 1) },
					{ v3i(0, 0, 0),v3i(1, 0, 0),v3i(2, 0, 0),v3i(2, 1, 0),v3i(2, 2, 0),v3i(1, 2, 0),v3i(0, 2, 0),v3i(0, 1, 0),v3i(1, 1, 0) },
				};
				const int32 voxelLengthLodStart(t_voxelAccessor::voxelLengthLod - 2);
				const int32 voxelLengthLodEnd(t_voxelAccessor::voxelLengthLod - 1);
				const vector3int32 toIterate[][2] = {
					{ v3i(0, 0, 0),                     v3i(1, voxelLengthLodStart, voxelLengthLodStart) },
					{ v3i(voxelLengthLodStart, 0, 0),   v3i(voxelLengthLodEnd, voxelLengthLodStart, voxelLengthLodStart) },
					{ v3i(0, 0, 0),                     v3i(voxelLengthLodStart, 1, voxelLengthLodStart) },
					{ v3i(0, voxelLengthLodStart, 0),   v3i(voxelLengthLodStart, voxelLengthLodEnd, voxelLengthLodStart) },
					{ v3i(0, 0, 0),                     v3i(voxelLengthLodStart, voxelLengthLodStart, 1) },
					{ v3i(0, 0, voxelLengthLodStart),   v3i(voxelLengthLodStart, voxelLengthLodStart, voxelLengthLodEnd) }
				};
				const vector3int32 reuseCorrection[] = {
					v3i(1, 0, 0),
					v3i(1, 0, 0),
					v3i(0, 1, 0),
					v3i(0, 1, 0),
					v3i(0, 0, 1),
					v3i(0, 0, 1)
				};

				const bool toInvertTriangles[] = {
					false, true,
					true, false, // data from Eric Lengyel seems to have different axis-desc
					false, true
				};

				for (int32 lod = 0; lod < 6; ++lod)
				{
					const int32 coord(lod / 2);
					const vector3int32& start(toIterate[lod][0]);
					const vector3int32& end(toIterate[lod][1]);
					const bool invertTriangles(toInvertTriangles[lod]);

					// the indexer for the vertices. *3 because gets saved with edge-id
					const int32 vertexIndicesReuseLodSize(((t_voxelAccessor::voxelLength + 1) * 4)*
						((t_voxelAccessor::voxelLength + 1) * 4));
					vector<int32> vertexIndicesReuseLod(vertexIndicesReuseLodSize, -1);

					for (uint32 x = start.x; x < static_cast<unsigned>(end.x); x += 2)
					{
						for (uint32 y = start.y; y < static_cast<unsigned>(end.y); y += 2)
						{
							for (uint32 z = start.z; z < static_cast<unsigned>(end.z); z += 2)
							{
								const vector3int32 voxelPos(x, y, z);
								{
									uint32 tableIndex(0);
									uint32 add(1);
									t_calcVoxelLod voxelCalc;
									for (uint16 ind = 0; ind < 9; ++ind)
									{
										const vector3int32 lookUp((voxelPos - start) + voxelLookups[coord][ind]);
										voxelCalc[ind] = getVoxelLod(lookUp, lod);
										if (voxelCalc[ind].getInterpolation() < isoLevel)
										{
											tableIndex |= add;
										}
										add *= 2;
									}
									if (tableIndex == 0 || tableIndex == 511) // no triangles
									{
										continue;
									}

									uint32 classIndex = transitionCellClass[tableIndex];
									const TransitionCellData *data = &transitionCellData[classIndex & 0x7F]; // only the last 7 bit count
									uint32 ids[12];

									vector3 normalsForTransvoxel[4];

									for (uint16 ind = 0; ind < data->GetVertexCount(); ++ind)
									{
										const uint16 data2 = transitionVertexData[tableIndex][ind];
										const uint16 edge = data2 >> 8;
										const uint16 edgeId = edge & 0x0F;
										const uint16 edgeBetween(data2 & 0xFF);

										if (edgeId == 0x9 || edgeId == 0x8)
										{
											BASSERT(edge == 0x88 || edge == 0x28 || edge == 0x89 || edge == 0x19);

											const uint16 owner((edge & 0xF0) >> 4);
											BASSERT(owner == 1 || owner == 2 || owner == 8);

											uint16 newEdgeId(0);
											uint16 newOwner(0);

											if (coord == 0)
											{
												if (edgeId == 0x8)
												{
													newEdgeId = 0x3;
												}
												else
												{
													newEdgeId = 0x1;
												}
												if (owner == 0x1)
												{
													newOwner = 0x4;
												}
												if (owner == 0x2)
												{
													newOwner = 0x2;
												}
											}
											if (coord == 1)
											{
												if (edgeId == 0x8)
												{
													newEdgeId = 0x2;
												}
												else
												{
													newEdgeId = 0x1;
												}
												newOwner = owner;
											}
											if (coord == 2)
											{
												newEdgeId = edgeId - 6;
												if (owner == 0x1)
												{
													newOwner = 0x1;
												}
												if (owner == 0x2)
												{
													newOwner = 0x4;
												}
											}


											uint16 newEdge((newOwner << 4) | newEdgeId);
											const int32 id = calculateEdgeId((voxelPos / 2) - reuseCorrection[lod], newEdge);

											BASSERT(vertexIndicesReuse[id] != -1);

											ids[ind] = vertexIndicesReuse[id];

											const vector3& normal(m_vertices.at(ids[ind]).normal);
											switch (edgeBetween)
											{
											case 0x9A:
												normalsForTransvoxel[0] = normal;
												break;
											case 0xAC:
												normalsForTransvoxel[1] = normal;
												break;
											case 0xBC:
												normalsForTransvoxel[2] = normal;
												break;
											case 0x9B:
												normalsForTransvoxel[3] = normal;
												break;
											default:
												BASSERT(false);
											}
										}
									}
									for (uint16 ind = 0; ind < data->GetVertexCount(); ++ind)
									{
										const uint16 data2 = transitionVertexData[tableIndex][ind];
										const uint16 edge = data2 >> 8;
										const uint16 edgeId = edge & 0x0F;
										const uint16 edgeBetween(data2 & 0xFF);

										if (edgeId != 0x9 && edgeId != 0x8)
										{
											const uint16 corner0 = data2 & 0x0F;
											const uint16 corner1 = (data2 & 0xF0) >> 4;


											const int32 id = calculateEdgeIdTransvoxel(voxelPos / 2, edge, coord);

											//blub::BOUT("edge:" + blub::string::number(edge, 16) + " id:" + blub::string::number(id));

											bool calculateVertexPosition(id == -1);
											if (!calculateVertexPosition)
											{
												calculateVertexPosition = vertexIndicesReuseLod[id] == -1;
											}

											if (calculateVertexPosition)
											{
												vector3 point = calculateIntersectionPositionTransvoxel(voxelPos - start, corner0, corner1, voxelLookups[coord], lod);
												point += vector3(start) / 2.;

												point *= voxelSize;

												vector3 normal;

												switch (edgeBetween)
												{
												case 0x01:
												case 0x12:
													normal = normalsForTransvoxel[0];
													break;
												case 0x03:
												case 0x36:
													normal = normalsForTransvoxel[3];
													break;
												case 0x25:
												case 0x58:
													normal = normalsForTransvoxel[1];
													break;
												case 0x67:
												case 0x78:
													normal = normalsForTransvoxel[2];
													break;
												case 0x34:
												case 0x14:
												case 0x45:
												case 0x47:
													normal = normalsForTransvoxel[0] + normalsForTransvoxel[1] + normalsForTransvoxel[2] + normalsForTransvoxel[3];
													break;
												default:
													BASSERT(false);
												}

												t_voxel voxel0 = getVoxelLod(voxelPos - start + calculateCornerTransvoxel(corner0, voxelLookups[coord]), lod); // OPTIMISE so dirty - use voxelCalc!
												t_voxel voxel1 = getVoxelLod(voxelPos - start + calculateCornerTransvoxel(corner1, voxelLookups[coord]), lod);
												const t_vertex vertex(static_cast<t_thiz>(this)->createVertexLod(voxelPos, voxel0, voxel1, point, normal));
												m_vertices.push_back(vertex);

												if (id == -1)
												{
													ids[ind] = m_vertices.size() - 1;
												}
												else
												{
													ids[ind] = vertexIndicesReuseLod[id] = m_vertices.size() - 1;
												}
											}
											else
											{
												ids[ind] = vertexIndicesReuseLod[id];
											}
										}
									}
									for (uint16 ind = 0; ind < data->GetTriangleCount() * 3; ind += 3)
									{
										// calc triangle
										const int32 vertexIndex0(ids[data->vertexIndex[ind + 0]]);
										const int32 vertexIndex1(ids[data->vertexIndex[ind + 1]]);
										const int32 vertexIndex2(ids[data->vertexIndex[ind + 2]]);
										const vector3 vertex0(m_vertices.at(vertexIndex0).position);
										const vector3 vertex1(m_vertices.at(vertexIndex1).position);
										const vector3 vertex2(m_vertices.at(vertexIndex2).position);
										if (vertex0 == vertex1 || vertex1 == vertex2 || vertex0 == vertex2)
										{
											continue; // triangle with zero space
										}
										// insert new triangle
										uint16 invert(1);
										if (invertTriangles)
										{
											invert = 0;
										}
										m_indicesLod[lod].push_back(vertexIndex0);
										if ((classIndex >> 7) % 2 == invert)
										{
											m_indicesLod[lod].push_back(vertexIndex1);
											m_indicesLod[lod].push_back(vertexIndex2);
										}
										else
										{
											m_indicesLod[lod].push_back(vertexIndex2);
											m_indicesLod[lod].push_back(vertexIndex1);
										}
									}
								}
							}
						}
					}
				}
			}

			delete[] vertexIndicesReuse;

			// normalise normals
			for (t_vertex& workVertex : m_vertices)
			{
				workVertex.normal.normalise();
			}

#ifdef BLUB_LOG_VOXEL_SURFACE
			blub::BOUT("surface::calculateSurface(..) end");
#endif
		}

		/**
		* @brief clear erases all buffer/results.
		*/
		void clear()
		{
			m_vertices.clear();
			m_indices.clear();
			for (int32 lod = 0; lod < 6; ++lod)
			{
				m_indicesLod[lod].clear();
			}
		}

		/**
		* @brief does the transvoxel algo get applied.
		* @return
		*/
		bool getCaluculateTransvoxel() const
		{
			return m_lod > 0;
		}

		/**
		* @brief same as getCaluculateTransvoxel()
		* @see getCaluculateTransvoxel()
		*/
		bool getCaluculateLod() const
		{
			return getCaluculateTransvoxel();
		}

		/**
		* @brief getPositions returns resulting position-list.
		* @return
		*/
		const t_vertices& getVertices() const
		{
			return m_vertices;
		}
		/**
		* @brief getIndices returns resulting index-list.
		* @return
		*/
		const t_indices& getIndices() const
		{
			return m_indices;
		}
		/**
		* @brief getPositions returns resulting transvoxel-list. Vertices for these indices are in getPositions() and getNormals().
		* @return
		*/
		const t_indices& getIndicesLod(const uint16& lod) const
		{
			BASSERT(lod < 6);
			return m_indicesLod[lod];
		}

	protected:
		/**
		* @brief surface constructor
		*/
		surface()
		{
		}

		/**
		* @brief Creates a vertex
		*/
		t_vertex createVertex(const vector3int32& /*voxelPos*/, const t_voxel &/*voxel0*/, const t_voxel &/*voxel1*/, const vector3 &position, const vector3 &normal)
		{
			t_vertex result;
			result.position = position;
			result.normal = normal;
			return result;
		}

		/**
		* @brief Creates a vertex for lod
		*/
		t_vertex createVertexLod(const vector3int32& /*voxelPos*/, const t_voxel &/*voxel0*/, const t_voxel &/*voxel1*/, const vector3 &position, const vector3 &normal)
		{
			t_vertex result;
			result.position = position;
			result.normal = normal;
			return result;
		}

	private:
		const t_voxel &getVoxel(const vector3int32& pos) const
		{
			return m_voxel->getVoxel(pos);
		}
		const int8 &getVoxelInterpolation(const vector3int32& pos) const
		{
			return getVoxel(pos).getInterpolation();
		}
		const t_voxel &getVoxelLod(const vector3int32& pos, const uint16 &lod) const
		{
			return m_voxel->getVoxelLod(pos, lod);
		}
		const int8 &getVoxelInterpolationLod(const vector3int32& pos, const uint16 &lod) const
		{
			return getVoxelLod(pos, lod).getInterpolation();
		}
		int32 calculateEdgeId(const vector3int32& pos, const int32& edgeInformation) const
		{
			const int32 edge(edgeInformation & 0x0F);
			const int32 owner((edgeInformation & 0xF0) >> 4);
			const int32 diffX(owner % 2);
			const int32 diffY((owner >> 2) % 2); // !!! order
			const int32 diffZ((owner >> 1) % 2);

			BASSERT((diffX == 0) || (diffX == 1));
			BASSERT((diffY == 0) || (diffY == 1));
			BASSERT((diffZ == 0) || (diffZ == 1));

			return calculateVertexId(pos - vector3int32(diffX, diffY, diffZ)) + (edge - 1);
		}
		int32 calculateEdgeIdTransvoxel(const vector3int32& pos, const int32& edgeInformation, const int32& coord) const
		{
			const int32 edge(edgeInformation & 0x0F);
			const int32 owner((edgeInformation & 0xF0) >> 4);
			const int32 diffFst(owner % 2);
			const int32 diffSnd((owner >> 1) % 2);

			BASSERT(edge >= 3);
			BASSERT(edge <= 6);
			BASSERT(owner == 1 || owner == 2 || owner == 4 || owner == 8);
			BASSERT((diffFst == 0) || (diffFst == 1));
			BASSERT((diffSnd == 0) || (diffSnd == 1));

			if (owner == 4) // no reuse
			{
				return -1;
			}
			switch (coord) {
			case 0:
				return calculateVertexIdTransvoxel(vector2int32(pos.y - diffFst, pos.z - diffSnd)) + (edge - 3);
			case 1:
				return calculateVertexIdTransvoxel(vector2int32(pos.x - diffFst, pos.z - diffSnd)) + (edge - 3);
			case 2:
				return calculateVertexIdTransvoxel(vector2int32(pos.x - diffFst, pos.y - diffSnd)) + (edge - 3);
			default:
				break;
			}

			BASSERT(false);
			return -1;
		}
		int32 calculateVertexId(const vector3int32& pos) const
		{
			return (
				((pos.x + 2) * 3) * ((t_voxelAccessor::voxelLengthWithNormalCorrection) * 3)*((t_voxelAccessor::voxelLengthWithNormalCorrection) * 3) +
				((pos.y + 2) * 3) * ((t_voxelAccessor::voxelLengthWithNormalCorrection) * 3) +
				((pos.z + 2) * 3)
				);
		}
		int32 calculateVertexIdTransvoxel(const vector2int32& pos) const
		{
			return (pos.x + 1) * 4 * (t_voxelAccessor::voxelLength + 1) +
				(pos.y + 1) * 4;
		}
		vector3 calculateIntersectionPosition(const vector3int32& pos, const int32& corner0, const int32& corner1)
		{
			const vector3int32 corn0(calculateCorner(corner0));
			const vector3int32 corn1(calculateCorner(corner1));

			const int8 interpolation0 = getVoxelInterpolation(corn0 + pos);
			const int8 interpolation1 = getVoxelInterpolation(corn1 + pos);

			const vector3 result = getInterpolatedPosition(vector3(corn0), vector3(corn1), interpolation0, interpolation1);

			return vector3(pos) + result;
		}
		vector3 calculateIntersectionPositionTransvoxel(const vector3int32& pos, const int32& corner0, const int32& corner1, const vector3int32 voxel[], const uint16 &lod)
		{
			const vector3int32 corn0(calculateCornerTransvoxel(corner0, voxel));
			const vector3int32 corn1(calculateCornerTransvoxel(corner1, voxel));

			const int8 interpolation0 = getVoxelInterpolationLod(corn0 + pos, lod);
			const int8 interpolation1 = getVoxelInterpolationLod(corn1 + pos, lod);

			const vector3 result = getInterpolatedPosition(vector3(corn0), vector3(corn1), interpolation0, interpolation1);

			return (vector3(pos) + result) / 2.;
		}
		static vector3int32 calculateCorner(const int32& corner)
		{
			switch (corner)
			{
			case 0:
				return vector3int32(0, 0, 0);
			case 1:
				return vector3int32(1, 0, 0);
			case 2:
				return vector3int32(0, 0, 1);
			case 3:
				return vector3int32(1, 0, 1);
			case 4:
				return vector3int32(0, 1, 0);
			case 5:
				return vector3int32(1, 1, 0);
			case 6:
				return vector3int32(0, 1, 1);
			case 7:
				return vector3int32(1, 1, 1);
			default:
				// fatal
				break;
			}
			// never reach!
			BASSERT(false);

			return vector3int32(0, 0, 0); // to remove the compile warning
		}
		static vector3int32 calculateCornerTransvoxel(const int32& corner, const vector3int32 voxel[])
		{
			switch (corner)
			{
			case 0x0:
				return vector3int32(0, 0, 0);
			case 0x1:
				return voxel[1];
			case 0x2:
				return voxel[2];
			case 0x3:
				return voxel[7];
			case 0x4:
				return voxel[8];
			case 0x5:
				return voxel[3];
			case 0x6:
				return voxel[6];
			case 0x7:
				return voxel[5];
			case 0x8:
				return voxel[4];
			case 0x9:
			case 0xA:
			case 0xB:
			case 0xC:
			default:
				break;
			}
			BASSERT(false);
			return vector3(0.);
		}
		static vector3 getInterpolatedPosition(const vector3& positionFrom, const vector3& positionTo, const int8& interpolationFrom, const int8& interpolationTo)
		{
			BASSERT(interpolationTo != interpolationFrom);

			const real mu((0.0 - ((real)interpolationFrom)) / ((real)(interpolationTo - interpolationFrom)));
			const vector3 result = positionFrom + mu*(positionTo - positionFrom);

			return result;
		}

	protected:
		t_voxelAccessorPtr m_voxel;
		int32 m_lod;

		t_vertices m_vertices;
		t_indices m_indices;
		t_indices m_indicesLod[6];
	};




}