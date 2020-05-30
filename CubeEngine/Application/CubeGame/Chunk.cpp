#include "Chunk.h"
#include "../EngineSrc/3D/Terrain/MarchingCubes.h"
#include "../EngineSrc/Texture/TextureMgr.h"
#include "EngineSrc/Engine/WorkerThreadSystem.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "GameConfig.h"
#include "GameMap.h"
#include "GameWorld.h"
#include "Utility/math/TbaseMath.h"
#include "Utility/misc/Tmisc.h"
#include "algorithm"
#include "FastNoise/FastNoise.h"
#include "time.h"
#include "3D/Terrain/TransVoxel.h"
/// <summary>	Size of the chunk. </summary>
static int g_chunkSize = BLOCK_SIZE * MAX_BLOCK;
#include "../EngineSrc/3D/Terrain/MCTable.h"
#include "../EngineSrc/Collision/CollisionUtility.h"

#include "EngineSrc/Collision/PhysicsMgr.h"
#include <random>
#include "3D/Vegetation/Tree.h"

namespace tzw
{
	std::mutex loading_mutex;
	/// <summary>	The flat noise. </summary>
	module::Perlin flatNoise;
	/// <summary>	The grass noise. </summary>
	module::Perlin grassNoise;
	FastNoise treeNoise;
	/// <summary>	The LOD list[]. </summary>
	static int lodList[] = {1, 2, 4, 8};

	Chunk::Chunk(int the_x, int the_y, int the_z)
		: x(the_x)
		, y(the_y)
		, z(the_z)
		, m_currenState(State::INVALID)
		, m_rigidBody(nullptr),
		m_isTreeloaded(false)
	{
		m_lod = 0;
		m_localAABB.update(vec3(0, 0, 0));

		m_localAABB.update(vec3(MAX_BLOCK * BLOCK_SIZE,
								MAX_BLOCK * BLOCK_SIZE,
								MAX_BLOCK * BLOCK_SIZE));

		m_basePoint =
			vec3(x * g_chunkSize, y * g_chunkSize, z * g_chunkSize) + vec3(LOD_SHIFT * BLOCK_SIZE, LOD_SHIFT * BLOCK_SIZE, LOD_SHIFT * BLOCK_SIZE);

		setPos(m_basePoint);

		m_mesh = nullptr;
		m_meshLOD1 = nullptr;
		m_needToUpdate = true;

		m_isNeedSubmitMesh = false;

		reCacheAABB();

		m_tmpNeighborChunk.clear();

		m_grass = new Grass("Texture/grass.tga");

		m_grass2 = new Grass("Texture/grass.tga");

		m_tree = new TreeGroup(GameMap::shared()->getTreeId());

		m_isHitable = true;

		grassNoise.SetSeed(time(nullptr));
		m_chunkInfo = GameMap::shared()->getChunkInfo(x, y, z);
	}

	vec3
	Chunk::getGridPos(int the_x, int the_y, int the_z)
	{
		return vec3(
			x * MAX_BLOCK + the_x, y * MAX_BLOCK + the_y, -1 * (z * MAX_BLOCK + the_z));
	}

	bool
	Chunk::intersectByAABB(const AABB& other, vec3& overLap)
	{
		if (m_currenState != State::LOADED)
			return false;
		auto size = m_mesh->getIndicesSize();
		for (size_t i = 0; i < size; i += 3)
		{
			for (int k = 0; k < 3; k++)
			{
				if (other.isInside(m_mesh->m_vertices[i + k].m_pos))
				{
					return true;
				}
			}
		}
		return false;
	}

	Drawable3D*
	Chunk::intersectByRay(const Ray& ray, vec3& hitPoint)
	{
		if (this->hitFirst(ray, hitPoint))
		{
			return this;
		}
		return nullptr;
	}

	bool
	Chunk::intersectBySphere(const t_Sphere& sphere, std::vector<vec3>& hitPoint)
	{
		if (m_currenState != State::LOADED)
			return false;
		auto size = m_mesh->getIndicesSize();
		std::vector<vec3> resultList;
		float t = 0;
		for (size_t i = 0; i < size; i += 3)
		{
			vec3 tmpHitPoint;
			if (sphere.intersectWithTriangle(m_mesh->m_vertices[i + 2].m_pos,
											m_mesh->m_vertices[i + 1].m_pos,
											m_mesh->m_vertices[i].m_pos,
											tmpHitPoint))
			{
				resultList.push_back(tmpHitPoint);
			}
		}
		if (!resultList.empty())
		{
			std::sort(resultList.begin(),
					resultList.end(),
					[sphere](const vec3& v1, const vec3& v2)
					{
						float dist1 = sphere.centre().distance(v1);
						float dist2 = sphere.centre().distance(v2);
						return dist1 < dist2;
					});
			hitPoint = resultList;
			return true;
		}
		return false;
	}

	void
	Chunk::logicUpdate(float delta)
	{
		if (m_isNeedSubmitMesh)
		{
			m_isNeedSubmitMesh = false;

			m_mesh->finish();
			m_meshLOD1->finish();
			if (m_rigidBody)
			{
				PhysicsMgr::shared()->removeRigidBody(m_rigidBody);
				delete m_rigidBody;
				m_rigidBody = nullptr;
			}
			m_rigidBody = PhysicsMgr::shared()->createRigidBodyMesh(m_mesh, nullptr);

			m_rigidBody->setFriction(10.0);
			m_grass->finish();
			m_grass2->finish();
			PhysicsMgr::shared()->addRigidBody(m_rigidBody);
			loading_mutex.lock();
			m_currenState = State::LOADED;
			loading_mutex.unlock();
		}
	}

	bool
	Chunk::getIsAccpectOcTtree() const
	{
		return m_currenState == State::LOADED;
	}

	/// <summary>
	/// Submits the draw command.
	/// </summary>
	void Chunk::submitDrawCmd(RenderCommand::RenderType passType)
	{
		/// just for test
		if (m_currenState != State::LOADED)
			return;
		if (m_mesh->getIndicesSize() == 0)
			return;
		if (m_isNeedSubmitMesh)
			return;
		if (m_mesh->getIndexBuf()->bufferId() == 0)
			return;
		if (passType != RenderCommand::RenderType::Common)
		{
			return;
		}
		auto player = GameWorld::shared()->getPlayer();
		auto pos = player->getPos();
		auto centre = getAABB().centre();
		if(vec3(pos.x, 0, pos.z).distance(vec3(centre.x, 0, centre.z)) < 50.0f)
		{
			RenderCommand command(m_mesh, m_material, passType);
			setUpTransFormation(command.m_transInfo);
			command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
			Renderer::shared()->addRenderCommand(command);
		}else
		{

			RenderCommand command(m_meshLOD1, m_material, passType);
			setUpTransFormation(command.m_transInfo);
			command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
			Renderer::shared()->addRenderCommand(command);
		}

		if (true)
		{
			// m_grass->pushCommand();
			// m_grass2->pushCommand();
			Tree::shared()->addTreeGroup(m_tree);
			// m_tree->pushCommand();
		}
	}

	void
	Chunk::load(int lodLevel)
	{
		if (m_currenState != State::INVALID)
			return;
		m_currentLOD = lodLevel;
		reCache();
		if (!m_mesh)
		{
			m_mesh = new Mesh();
			m_meshLOD1 = new Mesh();
			m_material = MaterialPool::shared()->getMatFromTemplate("VoxelTerrain");
		}
		loading_mutex.lock();
		m_currenState = State::LOADING;
		loading_mutex.unlock();
		setCamera(g_GetCurrScene()->defaultCamera());
		if (!m_chunkInfo->isLoaded)
		{
			WorkerThreadSystem::shared()->pushOrder(WorkerJob([&]()
			{
				initData();
				genMesh(lodLevel);
			}));
		}
		else
		{
			WorkerThreadSystem::shared()->pushOrder(WorkerJob([&]() { genMesh(lodLevel); }));
		}
	}

	void
	Chunk::unload()
	{
		if (m_currenState != State::LOADED)
		{
			return;
		}
		m_currenState = State::INVALID;
		m_isNeedSubmitMesh = false;
		// m_mesh->clear();
		delete m_mesh;
		m_mesh = nullptr;
	}

	void
	Chunk::deformSphere(vec3 pos, float value, float range)
	{
		m_tmpNeighborChunk.clear();
		vec3 relativePost = pos - m_basePoint;
		relativePost = relativePost / BLOCK_SIZE;
		relativePost.z *= -1;
		int posX = relativePost.x;
		int posY = relativePost.y;
		int posZ = relativePost.z;
		int searchSize = int(range / BLOCK_SIZE) + 1;
		for (int i = -searchSize; i <= searchSize; i++)
		{
			for (int j = -searchSize; j <= searchSize; j++)
			{
				for (int k = -searchSize; k <= searchSize; k++)
				{
					int X = posX + i;
					int Y = posY + j;
					int Z = posZ + k;
					float theDist =
						(m_basePoint + vec3(X * BLOCK_SIZE, Y * BLOCK_SIZE, -Z * BLOCK_SIZE))
						.distance(pos);
					if (theDist <= range)
					{
						float actualValue = value * ((range - theDist) / range);
						//X, Y, Z is in 0~15 range, we need add Padding
						deform(X + MIN_PADDING, Y + MIN_PADDING, Z + MIN_PADDING, std::clamp(actualValue, -1.f, 1.f));
					}
				}
			}
		}
		m_chunkInfo->isLoaded = false;
		initData();
		genMesh(m_currentLOD);


		if (!m_tmpNeighborChunk.empty())
		{
			for (auto chunk : m_tmpNeighborChunk)
			{
				chunk->m_chunkInfo->isLoaded = false;
				chunk->initData();
				chunk->genMesh(chunk->m_currentLOD);
			}
		}
		m_tmpNeighborChunk.clear();
	}

	void
	Chunk::deformCube(vec3 pos, float value, float range /*= 1.0f*/)
	{
		m_tmpNeighborChunk.clear();
		vec3 relativePost = pos - m_basePoint;
		relativePost = relativePost / BLOCK_SIZE;
		relativePost.z *= -1;
		int posX = relativePost.x;
		int posY = relativePost.y;
		int posZ = relativePost.z;
		int searchSize = int(range / BLOCK_SIZE);
		for (int i = -searchSize; i <= searchSize; i++)
		{
			for (int j = -searchSize; j <= searchSize; j++)
			{
				for (int k = -searchSize; k <= searchSize; k++)
				{
					int X = posX + i;
					int Y = posY + j;
					int Z = posZ + k;
					deform(X, Y, Z, value);
				}
			}
		}
		genMesh(m_currentLOD);
		if (!m_tmpNeighborChunk.empty())
		{
			for (auto chunk : m_tmpNeighborChunk)
			{
				chunk->genMesh(chunk->m_currentLOD);
			}
		}
		m_tmpNeighborChunk.clear();
	}

	void Chunk::paintSphere(vec3 pos, int matIndex, float range)
	{
		m_tmpNeighborChunk.clear();
		vec3 relativePost = pos - m_basePoint;
		relativePost = relativePost / BLOCK_SIZE;
		relativePost.z *= -1;
		int posX = relativePost.x;
		int posY = relativePost.y;
		int posZ = relativePost.z;
		int searchSize = int(range / BLOCK_SIZE) + 1;
		for (int i = -searchSize; i <= searchSize; i++)
		{
			for (int j = -searchSize; j <= searchSize; j++)
			{
				for (int k = -searchSize; k <= searchSize; k++)
				{
					int X = posX + i;
					int Y = posY + j;
					int Z = posZ + k;
					float theDist =
						(m_basePoint + vec3(X * BLOCK_SIZE, Y * BLOCK_SIZE, -Z * BLOCK_SIZE))
						.distance(pos);
					if (theDist <= range)
					{
						//X, Y, Z is in 0~15 range, we need add Padding
						paint(X + MIN_PADDING, Y + MIN_PADDING, Z + MIN_PADDING, matIndex);
					}
				}
			}
		}
		genMesh(m_currentLOD);

		if (!m_tmpNeighborChunk.empty())
		{
			for (auto chunk : m_tmpNeighborChunk)
			{
				chunk->genMesh(m_currentLOD);
			}
		}
		m_tmpNeighborChunk.clear();
	}
/*
边界情况如下，B是Padding，左1 右2 A是实际内容,
相邻区域的A互相不重叠，但相邻区域间的A和B会相互重叠
BAAAABB
    BAAAABB
*/				
	void
	Chunk::deformWithNeighbor(int X, int Y, int Z, std::function<void(Chunk *, int , int ,int)>neighborTrigger)
	{
		std::vector<int> xList;
		std::vector<int> yList;
		std::vector<int> zList;

		// check is need to calculate neighbors chunk
		if (X <= MIN_PADDING + 1 || X >= MAX_BLOCK)
		{
			if (X <= MIN_PADDING + 1)
				xList.push_back(-1);
			else
				xList.push_back(1);
		}
		if (Y <= MIN_PADDING + 1 || Y >= MAX_BLOCK)
		{
			if (Y <= MIN_PADDING + 1)
				yList.push_back(-1);
			else
			{
				yList.push_back(1);
			}
		}

		if (Z <= MIN_PADDING + 1 || Z >= MAX_BLOCK)
		{
			if (Z <= MIN_PADDING + 1)
				zList.push_back(-1);
			else
				zList.push_back(1);
		}
		xList.push_back(0);
		yList.push_back(0);
		zList.push_back(0);
		for (int offsetX : xList)
		{
			for (int offsetY : yList)
			{
				for (int offsetZ : zList)
				{
					auto neighborChunk = GameWorld::shared()->getChunk(
						this->x + offsetX, this->y + offsetY, this->z + offsetZ);
					if (neighborChunk)
					{
						int nx = X;
						int ny = Y;
						int nz = Z;
						if (offsetX == -1)
							nx += (MAX_BLOCK);
						else if (offsetX == 1)
							nx -= (MAX_BLOCK);
						if (offsetY == -1)
						{
							ny += (MAX_BLOCK);
						}
						else if (offsetY == 1)
						{
							ny -= (MAX_BLOCK);
						}
						if (offsetZ == -1)
							nz += (MAX_BLOCK);
						else if (offsetZ == 1)
							nz -= (MAX_BLOCK);
						neighborTrigger(neighborChunk,nx, ny, nz);
						//neighborChunk->setVoxelScalar(nx, ny, nz, value);
						auto result = std::find(m_tmpNeighborChunk.begin(),
												m_tmpNeighborChunk.end(),
												neighborChunk);
						if (result == m_tmpNeighborChunk.end() && neighborChunk != this &&
							neighborChunk != nullptr)
						{
							m_tmpNeighborChunk.push_back(neighborChunk);
						}
					}
				}
			}
		}
	}

	void
	Chunk::setVoxelScalar(int i, int j, int k, float scalar, bool isAdd)
	{
		// if (!isInOutterRange(x, y, z))
		// 	return;

		//扩展到Short，为了支持+-255的操作
		short scalarInShort = scalar * 128;
		int offset = MIN_PADDING;
		if (isAdd)
		{

			
			auto w = GameMap::shared()->getVoxel(x * MAX_BLOCK + (i - offset) + LOD_SHIFT, y * MAX_BLOCK + (j - offset) + LOD_SHIFT, z * MAX_BLOCK + (k - offset) + LOD_SHIFT);
			//short w = m_chunkInfo->mcPoints[ind].w;
			//m_chunkInfo->mcPoints[ind].w = std::clamp(w + scalarInShort, 0, 255);

			GameMap::shared()->setVoxel(x * MAX_BLOCK + (i - offset) + LOD_SHIFT, y * MAX_BLOCK + (j - offset) + LOD_SHIFT, z * MAX_BLOCK + (k - offset) + LOD_SHIFT, std::clamp(w + scalarInShort, 0, 255));
		}
		else
		{
			//m_chunkInfo->mcPoints[ind].w = std::clamp(scalarInShort, short(0), short(255));

			GameMap::shared()->setVoxel(x * MAX_BLOCK + (i - offset) + LOD_SHIFT, y * MAX_BLOCK + (j - offset) + LOD_SHIFT, z * MAX_BLOCK + (k - offset) + LOD_SHIFT, std::clamp(scalarInShort, short(0), short(255)));
		}
		m_chunkInfo->isEdit = true;
	}

	void
	Chunk::addVoexlScalar(int x, int y, int z, float scalar)
	{
		//	std::cout<<"chunk ("<<this->x<<this->y << this->z<<")"<< " set!!!!!("<<x
		//<<" , "<< y <<" , "<< z <<") as"<< scalar << std::endl;
		int YtimeZ = (MAX_BLOCK + 1) * (MAX_BLOCK + 1);
		int ind = x * YtimeZ + y * (MAX_BLOCK + 1) + z;
		m_chunkInfo->mcPoints[ind].w += scalar;
		if (m_chunkInfo->mcPoints[ind].w > 1.0)
			m_chunkInfo->mcPoints[ind].w = 1.0;
	}

	void Chunk::setVoxelMat(int x, int y, int z, int matIndex, bool isAdd)
	{
		//if (!isInOutterRange(x, y, z))
			//return;
		int YtimeZ = (MAX_BLOCK + MIN_PADDING + MAX_PADDING) * (MAX_BLOCK + MIN_PADDING + MAX_PADDING);
		int ind = x * YtimeZ + y * (MAX_BLOCK + MIN_PADDING + MAX_PADDING) + z;
		m_chunkInfo->mcPoints[ind].setMat(matIndex, 0, 0, vec3(1, 0, 0));
		m_chunkInfo->isEdit = true;
	}

	voxelInfo Chunk::getVoxel(int x, int y, int z)
	{
		int YtimeZ = (MAX_BLOCK + 1) * (MAX_BLOCK + 1);
		int ind = x * YtimeZ + y * (MAX_BLOCK + 1) + z;
		return m_chunkInfo->mcPoints[ind];
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Linear interp. </summary>
	///
	/// <remarks>	Tzwn, 2017/12/21. </remarks>
	///
	/// <param name="p1">   	[in,out] The first vec4. </param>
	/// <param name="p2">   	[in,out] The second vec4. </param>
	/// <param name="value">	The value. </param>
	///
	/// <returns>	A vec3. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	static vec3
	LinearInterp(vec4& p1, vec4& p2, float value)
	{
		vec3 p;
		vec3 tp1 = vec3(p1.x, p1.y, p1.z);
		vec3 tp2 = vec3(p2.x, p2.y, p2.z);
		if (p1.w != p2.w)
			p = tp1 + (tp2 - tp1) / (p2.w - p1.w) * (value - p1.w);
		else
			p = tp1;
		return p;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	A macro that defines Calculate Graduated Vertical 0. </summary>
	///
	/// <remarks>	Tzwn, 2017/12/21. </remarks>
	///
	/// <param name="verts">	The vertices. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

#define CALC_GRAD_VERT_0(verts)                                                \
  vec4(getPoint(i - 1, j, k).w - (verts[1]).w,                                 \
       getPoint(i, j - 1, k).w - (verts[4]).w,                                 \
       (verts[3]).w - getPoint(i, j, k - 1).w,                                 \
       (verts[0]).w);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	A macro that defines Calculate Graduated Vertical 1. </summary>
	///
	/// <remarks>	Tzwn, 2017/12/21. </remarks>
	///
	/// <param name="verts">	The vertices. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

#define CALC_GRAD_VERT_1(verts)                                                \
  vec4((verts[0]).w - getPoint(i + 2, j, k).w,                                 \
       getPoint(i + 1, j - 1, k).w - (verts[5]).w,                             \
       (verts[2]).w - getPoint(i + 1, j, k - 1).w,                             \
       (verts[1]).w);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	A macro that defines Calculate Graduated Vertical 2. </summary>
	///
	/// <remarks>	Tzwn, 2017/12/21. </remarks>
	///
	/// <param name="verts">	The vertices. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

#define CALC_GRAD_VERT_2(verts)                                                \
  vec4((verts[3]).w - getPoint(i + 2, j, k + 1).w,                             \
       getPoint(i + 1, j - 1, k + 1).w - (verts[6]).w,                         \
       getPoint(i + 1, j, k + 2).w - (verts[1]).w,                             \
       (verts[2]).w);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	A macro that defines Calculate Graduated Vertical 3. </summary>
	///
	/// <remarks>	Tzwn, 2017/12/21. </remarks>
	///
	/// <param name="verts">	The vertices. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

#define CALC_GRAD_VERT_3(verts)                                                \
  vec4(getPoint(i - 1, j, k + 1).w - (verts[2]).w,                             \
       getPoint(i, j - 1, k + 1).w - (verts[7]).w,                             \
       getPoint(i, j, k + 2).w - (verts[0]).w,                                 \
       (verts[3]).w);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	A macro that defines Calculate Graduated Vertical 4. </summary>
	///
	/// <remarks>	Tzwn, 2017/12/21. </remarks>
	///
	/// <param name="verts">	The vertices. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

#define CALC_GRAD_VERT_4(verts)                                                \
  vec4(getPoint(i - 1, j + 1, k).w - (verts[5]).w,                             \
       (verts[0]).w - getPoint(i, j + 2, k).w,                                 \
       (verts[7]).w - getPoint(i, j + 1, k - 1).w,                             \
       (verts[4]).w);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	A macro that defines Calculate Graduated Vertical 5. </summary>
	///
	/// <remarks>	Tzwn, 2017/12/21. </remarks>
	///
	/// <param name="verts">	The vertices. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

#define CALC_GRAD_VERT_5(verts)                                                \
  vec4((verts[4]).w - getPoint(i + 2, j + 1, k).w,                             \
       (verts[1]).w - getPoint(i + 1, j + 2, k).w,                             \
       (verts[6]).w - getPoint(i + 1, j + 1, k - 1).w,                         \
       (verts[5]).w);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	A macro that defines Calculate Graduated Vertical 6. </summary>
	///
	/// <remarks>	Tzwn, 2017/12/21. </remarks>
	///
	/// <param name="verts">	The vertices. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

#define CALC_GRAD_VERT_6(verts)                                                \
  vec4((verts[7]).w - getPoint(i + 2, j + 1, k + 1).w,                         \
       (verts[2]).w - getPoint(i + 1, j + 2, k + 1).w,                         \
       getPoint(i + 1, j + 1, k + 2).w - (verts[5]).w,                         \
       (verts[6]).w);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	A macro that defines Calculate Graduated Vertical 7. </summary>
	///
	/// <remarks>	Tzwn, 2017/12/21. </remarks>
	///
	/// <param name="verts">	The vertices. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

#define CALC_GRAD_VERT_7(verts)                                                \
  vec4(getPoint(i - 1, j + 1, k + 1).w - (verts[6]).w,                         \
       (verts[3]).w - getPoint(i, j + 2, k + 1).w,                             \
       getPoint(i, j + 1, k + 2).w - (verts[4]).w,                             \
       (verts[7]).w);

	void
	Chunk::genNormal()
	{
		auto points = m_chunkInfo->mcPoints;
		auto ncellsZ = MAX_BLOCK;
		auto ncellsY = MAX_BLOCK;
		auto ncellsX = MAX_BLOCK;
		float minValue = -0.0f;
		int meshIndex = 0;
		int pointsZ = ncellsZ + 1;
		int YtimeZ = (ncellsY + 1) * (ncellsZ + 1);

		// initialize vertices
		vec4 verts[8];
		vec4 gradVerts[8]; // gradients at each vertex of a cube
		vec3 grads[12]; // linearly interpolated gradients on each edge

		float gradFactorX = 0.5;
		float gradFactorY = 0.5;
		float gradFactorZ = 0.5;
		vec3 factor(1.0 / (2.0 * gradFactorX),
					1.0 / (2.0 * gradFactorY),
					1.0 / (2.0 * gradFactorZ));
		int lodStride = lodList[m_lod];
		// go through all the points
		for (int i = 0; i < ncellsX; i += lodStride) // x axis
		{
			for (int j = 0; j < ncellsY; j += lodStride) // y axis
			{
				for (int k = 0; k < ncellsZ; k += lodStride) // z axis
				{
					int ind = i * YtimeZ + j * (ncellsZ + 1) + k;
					int lastX = ncellsX; // left from older version
					int lastY = ncellsY;
					int lastZ = ncellsZ;

					verts[0] = points[ind].getV();
					verts[1] = points[ind + lodStride * YtimeZ].getV();
					verts[2] = points[ind + lodStride * (YtimeZ + 1)].getV();
					verts[3] = points[ind + lodStride * 1].getV();
					verts[4] = points[ind + lodStride * (ncellsZ + 1)].getV();
					verts[5] = points[ind + lodStride * (YtimeZ + (ncellsZ + 1))].getV();
					verts[6] = points[ind + lodStride * (YtimeZ + (ncellsZ + 1) + 1)].getV();
					verts[7] = points[ind + lodStride * ((ncellsZ + 1) + 1)].getV();

					// get the index
					int cubeIndex = int(0);
					for (int n = 0; n < 8; n++)
						/*(step 4)*/
						if (verts[n].w <= minValue)
							cubeIndex |= (1 << n);

					// check if its completely inside or outside
					/*(step 5)*/
					if (!edgeTable[cubeIndex])
						continue;
					vec3 intVerts[12];
					int indGrad = 0;
					auto edgeIndex = edgeTable[cubeIndex];
					if (edgeIndex & 1)
					{
						intVerts[0] = LinearInterp(verts[0], verts[1], minValue);
						gradVerts[0] = CALC_GRAD_VERT_0(verts)
						gradVerts[1] =
							CALC_GRAD_VERT_1(verts)
						indGrad |= 3;
						grads[0] = LinearInterp(gradVerts[0], gradVerts[1], minValue);
						grads[0].x *= factor.x;
						grads[0].y *= factor.y;
						grads[0].z *= factor.z;
					}
					if (edgeIndex & 2)
					{
						intVerts[1] = LinearInterp(verts[1], verts[2], minValue);
						if (!(indGrad & 2))
						{
							gradVerts[1] = CALC_GRAD_VERT_1(verts)
							indGrad |= 2;
						}
						gradVerts[2] = CALC_GRAD_VERT_2(verts)
						indGrad |= 4;
						grads[1] = LinearInterp(gradVerts[1], gradVerts[2], minValue);
						grads[1].x *= factor.x;
						grads[1].y *= factor.y;
						grads[1].z *= factor.z;
					}
					if (edgeIndex & 4)
					{
						intVerts[2] = LinearInterp(verts[2], verts[3], minValue);
						if (!(indGrad & 4))
						{
							gradVerts[2] = CALC_GRAD_VERT_2(verts)
							indGrad |= 4;
						}
						gradVerts[3] = CALC_GRAD_VERT_3(verts)
						indGrad |= 8;
						grads[2] = LinearInterp(gradVerts[2], gradVerts[3], minValue);
						grads[2].x *= factor.x;
						grads[2].y *= factor.y;
						grads[2].z *= factor.z;
					}
					if (edgeIndex & 8)
					{
						intVerts[3] = LinearInterp(verts[3], verts[0], minValue);
						if (!(indGrad & 8))
						{
							gradVerts[3] = CALC_GRAD_VERT_3(verts)
							indGrad |= 8;
						}
						if (!(indGrad & 1))
						{
							gradVerts[0] = CALC_GRAD_VERT_0(verts)
							indGrad |= 1;
						}
						grads[3] = LinearInterp(gradVerts[3], gradVerts[0], minValue);
						grads[3].x *= factor.x;
						grads[3].y *= factor.y;
						grads[3].z *= factor.z;
					}
					if (edgeIndex & 16)
					{
						intVerts[4] = LinearInterp(verts[4], verts[5], minValue);
						gradVerts[4] = CALC_GRAD_VERT_4(verts)

						gradVerts[5] = CALC_GRAD_VERT_5(verts)

						indGrad |= 48;
						grads[4] = LinearInterp(gradVerts[4], gradVerts[5], minValue);
						grads[4].x *= factor.x;
						grads[4].y *= factor.y;
						grads[4].z *= factor.z;
					}
					if (edgeIndex & 32)
					{
						intVerts[5] = LinearInterp(verts[5], verts[6], minValue);
						if (!(indGrad & 32))
						{
							gradVerts[5] = CALC_GRAD_VERT_5(verts)
							indGrad |= 32;
						}

						gradVerts[6] = CALC_GRAD_VERT_6(verts)
						indGrad |= 64;
						grads[5] = LinearInterp(gradVerts[5], gradVerts[6], minValue);
						grads[5].x *= factor.x;
						grads[5].y *= factor.y;
						grads[5].z *= factor.z;
					}
					if (edgeIndex & 64)
					{
						intVerts[6] = LinearInterp(verts[6], verts[7], minValue);
						if (!(indGrad & 64))
						{
							gradVerts[6] = CALC_GRAD_VERT_6(verts)
							indGrad |= 64;
						}
						gradVerts[7] = CALC_GRAD_VERT_7(verts);
						indGrad |= 128;
						grads[6] = LinearInterp(gradVerts[6], gradVerts[7], minValue);
						grads[6].x *= factor.x;
						grads[6].y *= factor.y;
						grads[6].z *= factor.z;
					}
					if (edgeIndex & 128)
					{
						intVerts[7] = LinearInterp(verts[7], verts[4], minValue);
						if (!(indGrad & 128))
						{
							gradVerts[7] = CALC_GRAD_VERT_7(verts)
							indGrad |= 128;
						}
						if (!(indGrad & 16))
						{
							gradVerts[4] = CALC_GRAD_VERT_4(verts)
							indGrad |= 16;
						}
						grads[7] = LinearInterp(gradVerts[7], gradVerts[4], minValue);
						grads[7].x *= factor.x;
						grads[7].y *= factor.y;
						grads[7].z *= factor.z;
					}
					if (edgeIndex & 256)
					{
						intVerts[8] = LinearInterp(verts[0], verts[4], minValue);
						if (!(indGrad & 1))
						{
							gradVerts[0] = CALC_GRAD_VERT_0(verts)
							indGrad |= 1;
						}
						if (!(indGrad & 16))
						{
							gradVerts[4] = CALC_GRAD_VERT_4(verts)
							indGrad |= 16;
						}
						grads[8] = LinearInterp(gradVerts[0], gradVerts[4], minValue);
						grads[8].x *= factor.x;
						grads[8].y *= factor.y;
						grads[8].z *= factor.z;
					}
					if (edgeIndex & 512)
					{
						intVerts[9] = LinearInterp(verts[1], verts[5], minValue);
						if (!(indGrad & 2))
						{
							gradVerts[1] = CALC_GRAD_VERT_1(verts)
							indGrad |= 2;
						}
						if (!(indGrad & 32))
						{
							gradVerts[5] = CALC_GRAD_VERT_5(verts)
							indGrad |= 32;
						}
						grads[9] = LinearInterp(gradVerts[1], gradVerts[5], minValue);
						grads[9].x *= factor.x;
						grads[9].y *= factor.y;
						grads[9].z *= factor.z;
					}
					if (edgeIndex & 1024)
					{
						intVerts[10] = LinearInterp(verts[2], verts[6], minValue);
						if (!(indGrad & 4))
						{
							gradVerts[2] = CALC_GRAD_VERT_2(verts)
							indGrad |= 4;
						}
						if (!(indGrad & 64))
						{
							gradVerts[6] = CALC_GRAD_VERT_6(verts)
							indGrad |= 64;
						}
						grads[10] = LinearInterp(gradVerts[2], gradVerts[6], minValue);
						grads[10].x *= factor.x;
						grads[10].y *= factor.y;
						grads[10].z *= factor.z;
					}
					if (edgeIndex & 2048)
					{
						intVerts[11] = LinearInterp(verts[3], verts[7], minValue);
						if (!(indGrad & 8))
						{
							gradVerts[3] = CALC_GRAD_VERT_3(verts)
							indGrad |= 8;
						}
						if (!(indGrad & 128))
						{
							gradVerts[7] = CALC_GRAD_VERT_7(verts)
							indGrad |= 128;
						}
						grads[11] = LinearInterp(gradVerts[3], gradVerts[7], minValue);
						grads[11].x *= factor.x;
						grads[11].y *= factor.y;
						grads[11].z *= factor.z;
					}

					// now build the triangles using triTable
					for (int n = 0; triTable[cubeIndex][n] != -1; n += 3)
					{
						m_mesh->m_vertices[meshIndex].m_normal =
							grads[triTable[cubeIndex][n + 2]].normalized();
						m_mesh->m_vertices[meshIndex + 1].m_normal =
							grads[triTable[cubeIndex][n + 1]].normalized();
						m_mesh->m_vertices[meshIndex + 2].m_normal =
							grads[triTable[cubeIndex][n]].normalized();
						meshIndex += 3;
					}
				} // END OF FOR LOOP
			}
		}
	}


	voxelInfo Chunk::getPoint(int index)
	{
		if (index >= 0 &&
			index < (MAX_BLOCK + 1) * (MAX_BLOCK + 1) * (MAX_BLOCK + 1))
		{
			return m_chunkInfo->mcPoints[index];
		}
		return m_chunkInfo->mcPoints[index];
	}

	voxelInfo
	Chunk::getPoint(int x, int y, int z)
	{
		int size = MAX_BLOCK + 1;
		int theIndex = x * size * size + y * size + z;
		if (x >= 0 && x <= MAX_BLOCK && y >= 0 && y <= MAX_BLOCK && z >= 0 &&
			z <= MAX_BLOCK)
		{
			return m_chunkInfo->mcPoints[theIndex];
		}
		else
		{
			int offsetX = 0;
			int offsetY = 0;
			int offsetZ = 0;
			if (x < 0)
			{
				offsetX = -1;
			}
			if (x > MAX_BLOCK)
			{
				offsetX = 1;
			}
			if (y < 0)
			{
				offsetY = -1;
			}
			if (y > MAX_BLOCK)
			{
				offsetY = 1;
			}
			if (z < 0)
			{
				offsetZ = -1;
			}
			if (z > MAX_BLOCK)
			{
				offsetZ = 1;
			}
			auto neighborChunk = GameWorld::shared()->getChunk(
				this->x + offsetX, this->y + offsetY, this->z + offsetZ);
			if (neighborChunk)
			{
				if (!neighborChunk->m_chunkInfo->isLoaded)
				{
					neighborChunk->initData();
				}
				int nx = x;
				int ny = y;
				int nz = z;
				if (x < 0)
					nx += MAX_BLOCK;
				else if (x > MAX_BLOCK)
					nx -= MAX_BLOCK;
				if (y < 0)
					ny += MAX_BLOCK;
				else if (y > MAX_BLOCK)
					ny -= MAX_BLOCK;
				if (z < 0)
					nz += MAX_BLOCK;
				else if (z > MAX_BLOCK)
					nz -= MAX_BLOCK;
				int neighborIndex = nx * size * size + ny * size + nz;
				return neighborChunk->m_chunkInfo->mcPoints[neighborIndex];
			}
			else
			{
				return voxelInfo(1.0, 1.0, 1.0, 1.0);
			}
		}
	}

	voxelInfo Chunk::getPointInner(int x, int y, int z)
	{
		int size = MAX_BLOCK + MIN_PADDING + MAX_PADDING;
		int theIndex = x * size * size + y * size + z;
		if (x >= 0 && x <= size - 1  && y >= 0 && y <= size - 1 && z >= 0 &&
			z <= size - 1)
		{
			return m_chunkInfo->mcPoints[theIndex];
		}
		return voxelInfo(0, 0, 0, 0);
	}

	int
	Chunk::getIndex(int x, int y, int z)
	{
		int size = MAX_BLOCK + 1;
		int theIndex = x * size * size + y * size + z;
		return theIndex;
	}

	bool
	Chunk::isInRange(int i, int j, int k)
	{
		return i >= 0 && i < MAX_BLOCK && j >= 0 && j < MAX_BLOCK && k >= 0 &&
			k < MAX_BLOCK;
	}

	bool
	Chunk::isInOutterRange(int i, int j, int k)
	{
		return i >= 0 && i <= MAX_BLOCK && j >= 0 && j <= MAX_BLOCK && k >= 0 &&
			k <= MAX_BLOCK;
	}

	bool
	Chunk::isInInnerRange(int i, int j, int k)
	{
		return i > MIN_PADDING + 1 && i < MAX_BLOCK && j > MIN_PADDING + 1 && j < MAX_BLOCK && k > MIN_PADDING + 1 &&
			k < MAX_BLOCK;
	}

	bool
	Chunk::isEdge(int i)
	{
		return i == 0 || i == MAX_BLOCK;
	}

	void
	Chunk::genMesh(int lodLevel)
	{
		if (!m_mesh)
			return;
		m_mesh->clear();
		m_meshLOD1->clear();
		//lod Data
		//sampleForLod(1, m_chunkInfo->mcPoints_lod1);
		// MarchingCubes::shared()->generateWithoutNormal(m_basePoint,
		// 												m_mesh, MAX_BLOCK, MAX_BLOCK, MAX_BLOCK, m_chunkInfo->mcPoints,
		// 												0.0f, m_lod);
		int targetSize = MAX_BLOCK + MIN_PADDING + MAX_PADDING;
		auto VoxelBuffer = m_chunkInfo->mcPoints;
		TransVoxel::shared()->generateWithoutNormal(m_basePoint,
														m_mesh, targetSize, VoxelBuffer,
														0.0f, 0);
		//generate LOD
		TransVoxel::shared()->generateWithoutNormal(m_basePoint,
														m_meshLOD1, (MAX_BLOCK>>1) + MIN_PADDING + MAX_PADDING, m_chunkInfo->mcPoints_lod1,
														0.0f, 1);
		if (m_mesh->isEmpty())
			return;
		// genNormal();
		// m_mesh->caclNormals();
		calculateMatID();
		loading_mutex.lock();
		m_isNeedSubmitMesh = true;
		loading_mutex.unlock();
	}

	void Chunk::initData()
	{
		if (m_chunkInfo->isLoaded) return;
		m_chunkInfo->initData();
		int YtimeZ = (MAX_BLOCK + MIN_PADDING + MAX_PADDING) * (MAX_BLOCK + MIN_PADDING + MAX_PADDING);
		vec4 verts;
		vec3 tmpV3;
		int offset = MIN_PADDING;
		//前MIN_PADDING的元素((i, j, k)<MIN_PADDING)是上一个Chunk的，这里要做减法处理
		for (int i = 0; i < MAX_BLOCK + MIN_PADDING + MAX_PADDING; i++)
		{
			for (int k = 0; k < MAX_BLOCK + MIN_PADDING + MAX_PADDING; k++)
			{
				for (int j = 0; j < MAX_BLOCK + MIN_PADDING + MAX_PADDING;
					j++) // Y in the most inner loop, cache friendly
				{
					auto w = GameMap::shared()->getDensityI(x * MAX_BLOCK + (i - offset) + LOD_SHIFT, y * MAX_BLOCK + (j - offset) + LOD_SHIFT, z * MAX_BLOCK + (k - offset) + LOD_SHIFT);
					// x y z
					int ind = i * YtimeZ + j * (MAX_BLOCK + MIN_PADDING + MAX_PADDING) + k;
					m_chunkInfo->mcPoints[ind].w = w;
					m_chunkInfo->mcPoints[ind].index = ind;
				}
			}
		}
		//前MIN_PADDING的元素((i, j, k)<MIN_PADDING)是上一个Chunk的，这里要做减法处理
		//gen material
		for (int i = 0; i < MAX_BLOCK + MIN_PADDING + MAX_PADDING; i++)
		{
			for (int k = 0; k < MAX_BLOCK + MIN_PADDING + MAX_PADDING; k++)
			{
				bool isSet = false;
				for (int j = 0; j < MAX_BLOCK + MIN_PADDING + MAX_PADDING;
					j++) // Y in the most inner loop, cache friendly
				{
					verts = vec4((i - offset) * BLOCK_SIZE, (j - offset) * BLOCK_SIZE, -1 * (k - offset) * BLOCK_SIZE, -1) +
						vec4(m_basePoint, 0);
					tmpV3.x = verts.x;
					tmpV3.y = verts.y;
					tmpV3.z = verts.z;
					auto points = m_chunkInfo->mcPoints;
					int ind = i * YtimeZ + j * (MAX_BLOCK  + MIN_PADDING + MAX_PADDING) + k;
					if ((points[ind]).w >= -0.5f || true) //TODO skip the Empty block 
					{
						auto x1 = getPointInner(i - 1, j, k).w;
						auto x2 = getPointInner(i + 1, j, k).w;
						auto y1 = getPointInner(i, j - 1, k).w;
						auto y2 = getPointInner(i, j + 1, k).w;
						auto z1 = getPointInner(i, j, k - 1).w;
						auto z2 = getPointInner(i, j, k + 1).w;
						auto gradientVec4 = vec4(x1 - x2,
												y1 - y2,
												z1 - z2,
												(points[ind]).w);
						float slope = 1.0 - TbaseMath::clampf(
							vec3::DotProduct(gradientVec4.toVec3().normalized(), vec3(0, -1, 0)), 0.0f, 1.0f);
						auto matID = GameMap::shared()->getMat(tmpV3, slope);
						// x y z
						m_chunkInfo->mcPoints[ind].setMat(matID, 0, 0, vec3(1,0, 0));
					}
				}
			}
		}

		//for LOD 1
		//前MIN_PADDING的元素((i, j, k)<MIN_PADDING)是上一个Chunk的，这里要做减法处理,注意LOD的元素涉及到前一个的也是在LOD的范围内的
		int lodLevel = 1;
		int stride = 1 << lodLevel;
		for (int i = 0; i < (MAX_BLOCK>>lodLevel) + MIN_PADDING + MAX_PADDING; i++)
		{
			for (int k = 0; k < (MAX_BLOCK>>lodLevel)  + MIN_PADDING + MAX_PADDING; k++)
			{
				for (int j = 0; j < (MAX_BLOCK>>lodLevel)  + MIN_PADDING + MAX_PADDING;
					j++) // Y in the most inner loop, cache friendly
				{
					int BlockROW = ((MAX_BLOCK>>lodLevel) + MIN_PADDING + MAX_PADDING);
					//verts = vec4((i - offset) * BLOCK_SIZE * stride, (j - offset) * BLOCK_SIZE * stride, -1 * (k - offset) * BLOCK_SIZE * stride, -1) +
					//	vec4(m_basePoint, 0);


					auto w = GameMap::shared()->getDensityI(x * MAX_BLOCK + (i - offset)*stride + LOD_SHIFT, y * MAX_BLOCK + (j - offset)*stride + LOD_SHIFT, z * MAX_BLOCK + (k - offset)*stride + LOD_SHIFT);
					//tmpV3.x = verts.x;
					//tmpV3.y = verts.y;
					//tmpV3.z = verts.z;
					//verts.w = GameMap::shared()->getDensity(tmpV3);
					// x y z
					int ind = i * BlockROW*BlockROW + j * BlockROW + k;
					m_chunkInfo->mcPoints_lod1[ind].w = w;
					m_chunkInfo->mcPoints_lod1[ind].index = ind;
					m_chunkInfo->mcPoints_lod1[ind].setMat(9, 0, 0, vec3(1,0, 0));
				}
			}
		}

		sampleForLod(1, m_chunkInfo->mcPoints_lod1);
		m_chunkInfo->isEdit = false;
		m_chunkInfo->isLoaded = true;
	}

	void
	Chunk::checkCollide(ColliderEllipsoid* package)
	{
		if (m_currenState != State::LOADED)
			return;
		auto size = m_mesh->getIndicesSize();
		std::vector<vec3> resultList;
		float t = 0;
		for (size_t i = 0; i < size; i += 3)
		{
			CollisionUtility::checkTriangle(
				package,
				package->toE(m_mesh->m_vertices[i + 2].m_pos),
				package->toE(m_mesh->m_vertices[i + 1].m_pos),
				package->toE(m_mesh->m_vertices[i].m_pos));
		}
	}

	void
	Chunk::setUpTransFormation(TransformationInfo& info)
	{
		auto currCam = g_GetCurrScene()->defaultCamera();
		info.m_projectMatrix = currCam->projection();
		info.m_viewMatrix = currCam->getViewMatrix();
		Matrix44 mat; // mesh it self contain world position
		mat.setToIdentity();
		info.m_worldMatrix = mat;
	}

	void
	Chunk::setLod(unsigned int newLod)
	{
		m_lod = newLod;
	}

	unsigned int
	Chunk::getLod()
	{
		return m_lod;
	}

	void
	Chunk::deform(int X, int Y, int Z, float actualVal)
	{
		if (!isInInnerRange(X, Y, Z))
		{
			deformWithNeighbor(X, Y, Z, [actualVal](Chunk * c, int theX, int theY, int theZ){c->setVoxelScalar(theX, theY, theZ,actualVal);});
			return;
		}
		setVoxelScalar(X, Y, Z, actualVal);
	}

	void Chunk::paint(int X, int Y, int Z, int matIndex)
	{
		if (!isInInnerRange(X, Y, Z))
		{
			//paintWithNeighbor(X, Y, Z, matIndex);
			deformWithNeighbor(X, Y, Z, [matIndex](Chunk * c, int theX, int theY, int theZ){c->setVoxelMat(theX, theY, theZ,matIndex);});
			return;
		}
		setVoxelMat(X, Y, Z, matIndex);
	}

	unsigned int
	Chunk::getTypeId()
	{
		return TYPE_CHUNK;
	}

	void
	Chunk::calculateMatID()
	{
		m_grassPosList.clear();
		m_grass->m_mesh->clearInstances();
		m_grass2->m_mesh->clearInstances();
		size_t indexCount = m_mesh->m_indices.size();
		if (indexCount <= 0) return;
		float grassDensity = 1.0;
		float step = 1.0 / grassDensity;
		for (float x = m_basePoint.x; x <= m_basePoint.x + BLOCK_SIZE * MAX_BLOCK; x += grassDensity)
		{
			for (float z = m_basePoint.z; z > m_basePoint.z - BLOCK_SIZE * MAX_BLOCK; z -= grassDensity)
			{
				auto ox = TbaseMath::randFN() * 0.4;
				auto oz = TbaseMath::randFN() * 0.4;
				auto h = GameMap::shared()->getHeight(vec2(x + ox, z + oz));
				vec3 pos(x + ox, h, z + oz);
				float value = flatNoise.GetValue(pos.x * 0.03, pos.z * 0.03, 0.0);
				// the flat is grass or dirt?
				value = value * 0.5 + 0.5;
				// value = std::max(value - 0.2f, 0.0f);
				value = Tmisc::clamp(powf(value, 4.0), 0.0f, 1.0f);
				if ((1.0 - value > 0.7))
				{
					if (grassNoise.GetValue(pos.x * 0.3, pos.z * 0.3, 0.0) > 0.2)
					{
						auto ox = TbaseMath::randFN() * 0.5;
						auto oz = TbaseMath::randFN() * 0.5;
						auto scale = TbaseMath::randFN() * 0.1;
						Grass* grass = nullptr;
						if (rand() % 100 > 85)
						{
							grass = m_grass;
						}
						else
						{
							grass = m_grass2;
						}
						InstanceData instance;
						vec3 normal = GameMap::shared()->getNormal(vec2(x + ox, z + oz));
						instance.posAndScale = vec4(pos.x, pos.y + 0.35, pos.z, 1.0 + scale);
						instance.extraInfo = vec4(normal, 0);
						auto rotateM = Matrix44();
						rotateM.setToIdentity();
						auto m = rotateM.data();
						auto z = vec3(0, 0, 1);
						auto right = vec3::CrossProduct(normal, z);
						auto zNew = vec3::CrossProduct(right, normal);
						m[0] = right.x;
						m[1] = right.y;
						m[2] = right.z;


						m[4] = normal.x;
						m[5] = normal.y;
						m[6] = normal.z;


						m[8] = zNew.x;
						m[9] = zNew.y;
						m[10] = zNew.z;
						Quaternion q;
						rotateM.getRotation(&q);
						instance.rotateInfo = vec4(q.x, q.y, q.z, q.w);
						grass->m_mesh->pushInstance(instance);
					}
				}
			}
		}



		treeNoise.SetSeed(233);
		treeNoise.SetFrequency(0.04);
		treeNoise.SetNoiseType(FastNoise::Simplex);
		int treeCount = 0;
		vec3 theBasePoint = GameMap::shared()->voxelToWorldPos(this->x * MAX_BLOCK + LOD_SHIFT, this->y * MAX_BLOCK + LOD_SHIFT, this->z * MAX_BLOCK + LOD_SHIFT);
		for (float x = 0; x <= BLOCK_SIZE * MAX_BLOCK; x += 3.0)
		{
			for (float z = 0; z <= BLOCK_SIZE * MAX_BLOCK; z += 3.0)
			{
				auto ox = TbaseMath::randFN() * 0.8;
				auto oz = TbaseMath::randFN() * 0.8;
				float value = treeNoise.GetNoise(x + ox, 0, z + oz);
				// the flat is grass or dirt?
				value = value * 0.5 + 0.5;
				auto h = GameMap::shared()->getHeight(vec2(theBasePoint.x + x + ox, theBasePoint.z + z + oz));
				if (value > 0.5)
				{
					// auto treeModel = Model::create("Models/tree/tree.tzw", true);
					// g_GetCurrScene()->addNode(treeModel);
					// treeModel->getMat(0)->setTex("DiffuseMap",
					// 							TextureMgr::shared()->getByPath("Models/tree/bark.jpg", true));
					// treeModel->getMat(1)->setTex("DiffuseMap",
					// 							TextureMgr::shared()->getByPath("Models/tree/twig.png", true));
					treeCount += 1;
					InstanceData instance;
					vec3 normal = vec3(0, 1,0);
					instance.posAndScale = vec4(m_basePoint.x + x + ox, h, m_basePoint.z + z + oz, 1.0);
					instance.extraInfo = vec4(normal, 0);
					// auto rotateM = Matrix44();
					// rotateM.setToIdentity();
					// auto m = rotateM.data();
					// auto z = vec3(0, 0, 1);
					// auto right = vec3::CrossProduct(normal, z);
					// auto zNew = vec3::CrossProduct(right, normal);
					// m[0] = right.x;
					// m[1] = right.y;
					// m[2] = right.z;
					//
					//
					// m[4] = normal.x;
					// m[5] = normal.y;
					// m[6] = normal.z;
					//
					//
					// m[8] = zNew.x;
					// m[9] = zNew.y;
					// m[10] = zNew.z;
					// Quaternion q;
					// rotateM.getRotation(&q);
					// instance.rotateInfo = vec4(q.x, q.y, q.z, q.w);
 					m_tree->m_instance.push_back(instance);
				}
			}
		}
		m_isTreeloaded = true;
	}

	bool Chunk::getIsInitData()
	{
		return m_chunkInfo->isLoaded;
	}

	ChunkInfo* Chunk::getChunkInfo()
	{
		return m_chunkInfo;
	}

	int Chunk::getCurrentLod()
	{
		return m_currentLOD;
	}

	void Chunk::sampleForLod(int lodLevel, voxelInfo* out)
	{
		return;
		int originalSize = MAX_BLOCK + MIN_PADDING + MAX_PADDING;
		int YtimeZ = (MAX_BLOCK + MIN_PADDING + MAX_PADDING) * (MAX_BLOCK + MIN_PADDING + MAX_PADDING);
		int innerLODSize = (MAX_BLOCK>>1);
		int lodSize = innerLODSize + MIN_PADDING + MAX_PADDING;
		//sample for lod
		for(int i = MIN_PADDING; i <= innerLODSize; i++) 
		{
			for(int j = MIN_PADDING; j <= innerLODSize; j++)
			{
				for(int k = MIN_PADDING; k <= innerLODSize; k++)
				{
					int x = i * 2 - 1;
					int y = j * 2 - 1;
					int z = k * 2 -1;
					int tag = 0;
					int indLOD = i *(lodSize * lodSize) + j * (lodSize) + k;
					int ind = x *(YtimeZ) + y * (MAX_BLOCK  + MIN_PADDING + MAX_PADDING) + z;
					out[indLOD] =m_chunkInfo->mcPoints[ind];//std::clamp(h - pos.y, -1.f, 1.f);
					out[indLOD].index = indLOD;
				}
			}
		}
	}

	bool
	Chunk::isInEdge(int i, int j, int k)
	{
		if (isInOutterRange(i, j, k))
		{
			return (i == 0 || i == MAX_BLOCK) || (j == 0 || j == MAX_BLOCK) ||
				(k == 0 || k == MAX_BLOCK);
		}
		return false;
	}

	bool
	Chunk::hitAny(Ray& ray, vec3& result)
	{
		auto size = m_mesh->getIndicesSize();
		float t = 0;
		for (size_t i = 0; i < size; i += 3)
		{
			if (ray.intersectTriangle(m_mesh->m_vertices[m_mesh->m_indices[i]].m_pos,
									m_mesh->m_vertices[m_mesh->m_indices[i + 1]].m_pos,
									m_mesh->m_vertices[m_mesh->m_indices[i + 2]].m_pos,
									&t))
			{
				result = ray.origin() + ray.direction() * t;
				return true;
			}
		}
		return false;
	}

	bool
	Chunk::hitFirst(const Ray& ray, vec3& result)
	{
		if (m_currenState != State::LOADED)
			return false;
		auto size = m_mesh->getIndicesSize();
		float t = 0;
		bool isFind = false;
		vec3 minP = vec3(-99999, -99999, -99999);
		for (size_t i = 0; i < size; i += 3)
		{
			if (ray.intersectTriangle(m_mesh->m_vertices[m_mesh->m_indices[i + 2]].m_pos,
									m_mesh->m_vertices[m_mesh->m_indices[i + 1]].m_pos,
									m_mesh->m_vertices[m_mesh->m_indices[i]].m_pos,
									&t))
			{
				auto tmp = ray.origin() + ray.direction() * t;
				if (tmp.distance(ray.origin()) < minP.distance(ray.origin()))
				{
					minP = tmp;
					isFind = true;
				}
			}
		}
		result = minP;
		return isFind;
	}
}
