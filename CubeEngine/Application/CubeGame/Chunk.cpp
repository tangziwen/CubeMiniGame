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
		: m_x(the_x)
		, m_y(the_y)
		, m_z(the_z)
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
			vec3(m_x * g_chunkSize, m_y * g_chunkSize, m_z * g_chunkSize) + vec3(LOD_SHIFT * BLOCK_SIZE, LOD_SHIFT * BLOCK_SIZE, LOD_SHIFT * BLOCK_SIZE) + GameMap::shared()->getMapOffset();

		setPos(m_basePoint);

		memset(m_mesh, 0,sizeof(m_mesh));
		memset(m_meshTransition, 0,sizeof(m_meshTransition));

		m_needToUpdate = true;

		m_isNeedSubmitMesh = false;

		reCacheAABB();

		m_tmpNeighborChunk.clear();

		m_grass = new TreeGroup(GameMap::shared()->getGrassId());

		m_grass2 = new TreeGroup(GameMap::shared()->getGrassId());

		m_tree = new TreeGroup(GameMap::shared()->getTreeId());

		m_isHitable = true;

		grassNoise.SetSeed(time(nullptr));
		m_chunkInfo = GameMap::shared()->getChunkInfo(m_x, m_y, m_z);
	}

	vec3
	Chunk::getGridPos(int the_x, int the_y, int the_z)
	{
		return vec3(
			m_x * MAX_BLOCK + the_x, m_y * MAX_BLOCK + the_y, -1 * (m_z * MAX_BLOCK + the_z));
	}

	bool
	Chunk::intersectByAABB(const AABB& other, vec3& overLap)
	{
		if (m_currenState != State::LOADED)
			return false;
		auto size = m_mesh[0]->getIndicesSize();
		for (size_t i = 0; i < size; i += 3)
		{
			for (int k = 0; k < 3; k++)
			{
				if (other.isInside(m_mesh[0]->m_vertices[i + k].m_pos))
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
		auto size = m_mesh[0]->getIndicesSize();
		std::vector<vec3> resultList;
		float t = 0;
		for (size_t i = 0; i < size; i += 3)
		{
			vec3 tmpHitPoint;
			if (sphere.intersectWithTriangle(m_mesh[0]->m_vertices[i + 2].m_pos,
											m_mesh[0]->m_vertices[i + 1].m_pos,
											m_mesh[0]->m_vertices[i].m_pos,
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
			for(int i = 0; i < 3;i++)
			{
				m_mesh[i]->finish();
				m_meshTransition[i]->finish();
			}
			
			if (m_rigidBody)
			{
				PhysicsMgr::shared()->removeRigidBody(m_rigidBody);
				delete m_rigidBody;
				m_rigidBody = nullptr;
			}
			m_rigidBody = PhysicsMgr::shared()->createRigidBodyMesh(m_mesh[0], nullptr);

			m_rigidBody->setFriction(10.0);
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
		if (m_mesh[0]->getIndicesSize() == 0)
			return;
		if (m_isNeedSubmitMesh)
			return;
		if (m_mesh[0]->getIndexBuf()->bufferId() == 0)
			return;
		if (passType != RenderCommand::RenderType::Common)
		{
			return;
		}
		auto player = GameWorld::shared()->getPlayer();
		auto pos = player->getPos();
		auto centre = getAABB().centre();
		auto dist = vec3(pos.x, 0, pos.z).distance(vec3(centre.x, 0, centre.z));


		if(dist < 30.0f)
		{
			m_currentLOD = 0;
		}
		else if(dist < 75.0f)
		{
			m_currentLOD = 1;
		} else
		{
			m_currentLOD = 2;
		}
		auto xList = {-1, 0, 1};
		auto yList = {-1, 0, 1};
		auto zList = {-1, 0, 1};
		bool isNeedShowTransition = false;
		for (int offsetX : xList)
		{
			for (int offsetY : yList)
			{
				for (int offsetZ : zList)
				{
					if(offsetX == 0 && offsetY ==0 && offsetZ == 0) continue;// skip self chunk
					auto neighborChunk = GameWorld::shared()->getChunk(
						this->m_x + offsetX, this->m_y + offsetY, this->m_z + offsetZ);
					if (neighborChunk)
					{
						if(neighborChunk->m_currentLOD != m_currentLOD)
						{
							isNeedShowTransition = true;
						}
					}
				}
			}
		}


		RenderCommand command(m_mesh[m_currentLOD], m_material, passType);
		setUpTransFormation(command.m_transInfo);
		command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
		Renderer::shared()->addRenderCommand(command);
		if(isNeedShowTransition)
		{
			RenderCommand command(m_meshTransition[m_currentLOD], m_material, passType);
			setUpTransFormation(command.m_transInfo);
			command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
			Renderer::shared()->addRenderCommand(command);
		}
		
		if (true)
		{
			Tree::shared()->addTreeGroup(m_tree);
			Tree::shared()->addTreeGroup(m_grass);
		}
	}

	void
	Chunk::load(int lodLevel)
	{
		if (m_currenState != State::INVALID)
			return;
		reCache();
		if (!m_mesh[0])
		{
			for(int i =0; i < 3; i++)
			{

				m_mesh[i] = new Mesh();
				m_meshTransition[i] = new Mesh();
			}
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
				generateVegetation();
			}));
		}
		else
		{
			WorkerThreadSystem::shared()->pushOrder(WorkerJob([&]() { genMesh(lodLevel); generateVegetation(); }));
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

		for(int i = 0 ; i< 3; i++)
		{
			delete m_mesh[i];
			m_mesh[i] = nullptr;

			delete m_meshTransition[i];
			m_meshTransition[i] = nullptr;
		}

	}

	void
	Chunk::deformSphere(vec3 pos, float value, float range)
	{
		m_tmpNeighborChunk.clear();
		vec3 relativePost = pos - m_basePoint;
		relativePost = relativePost / BLOCK_SIZE;
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
						(m_basePoint + vec3(X * BLOCK_SIZE, Y * BLOCK_SIZE, Z * BLOCK_SIZE))
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

	void
	Chunk::deformCube(vec3 pos, float value, float range /*= 1.0f*/)
	{
		m_tmpNeighborChunk.clear();
		vec3 relativePost = pos - m_basePoint;
		relativePost = relativePost / BLOCK_SIZE;
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
						(m_basePoint + vec3(X * BLOCK_SIZE, Y * BLOCK_SIZE, Z * BLOCK_SIZE))
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
						this->m_x + offsetX, this->m_y + offsetY, this->m_z + offsetZ);
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

			
			auto w = GameMap::shared()->getVoxelW(m_x * MAX_BLOCK + (i - offset) + LOD_SHIFT, m_y * MAX_BLOCK + (j - offset) + LOD_SHIFT, m_z * MAX_BLOCK + (k - offset) + LOD_SHIFT);
			//short w = m_chunkInfo->mcPoints[ind].w;
			//m_chunkInfo->mcPoints[ind].w = std::clamp(w + scalarInShort, 0, 255);

			GameMap::shared()->setVoxel(m_x * MAX_BLOCK + (i - offset) + LOD_SHIFT, m_y * MAX_BLOCK + (j - offset) + LOD_SHIFT, m_z * MAX_BLOCK + (k - offset) + LOD_SHIFT, std::clamp(w + scalarInShort, 0, 255));
		}
		else
		{
			//m_chunkInfo->mcPoints[ind].w = std::clamp(scalarInShort, short(0), short(255));

			GameMap::shared()->setVoxel(m_x * MAX_BLOCK + (i - offset) + LOD_SHIFT, m_y * MAX_BLOCK + (j - offset) + LOD_SHIFT, m_z * MAX_BLOCK + (k - offset) + LOD_SHIFT, std::clamp(scalarInShort, short(0), short(255)));
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
		m_chunkInfo->mcPoints[0][ind].w += scalar;
		if (m_chunkInfo->mcPoints[0][ind].w > 1.0)
			m_chunkInfo->mcPoints[0][ind].w = 1.0;
	}

	void Chunk::setVoxelMat(int i, int j, int k, int matIndex, bool isAdd)
	{
		//if (!isInOutterRange(x, y, z))
			//return;
		// int YtimeZ = (MAX_BLOCK + MIN_PADDING + MAX_PADDING) * (MAX_BLOCK + MIN_PADDING + MAX_PADDING);
		// int ind = x * YtimeZ + y * (MAX_BLOCK + MIN_PADDING + MAX_PADDING) + z;
		// m_chunkInfo->mcPoints[0][ind].setMat(matIndex, 0, 0, vec3(1, 0, 0));
		int offset = MIN_PADDING;
		auto v = GameMap::shared()->getVoxel(m_x * MAX_BLOCK + (i - offset) + LOD_SHIFT, m_y * MAX_BLOCK + (j - offset) + LOD_SHIFT, m_z * MAX_BLOCK + (k - offset) + LOD_SHIFT);
		v->setMat(matIndex, 0, 0, vec3(1, 0, 0));
		m_chunkInfo->isEdit = true;
	}

	voxelInfo Chunk::getVoxel(int x, int y, int z)
	{
		int YtimeZ = (MAX_BLOCK + 1) * (MAX_BLOCK + 1);
		int ind = x * YtimeZ + y * (MAX_BLOCK + 1) + z;
		return m_chunkInfo->mcPoints[0][ind];
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



	voxelInfo Chunk::getPoint(int index)
	{
		if (index >= 0 &&
			index < (MAX_BLOCK + 1) * (MAX_BLOCK + 1) * (MAX_BLOCK + 1))
		{
			return m_chunkInfo->mcPoints[0][index];
		}
		return m_chunkInfo->mcPoints[0][index];
	}

	voxelInfo
	Chunk::getPoint(int x, int y, int z)
	{
		int size = MAX_BLOCK + 1;
		int theIndex = x * size * size + y * size + z;
		if (x >= 0 && x <= MAX_BLOCK && y >= 0 && y <= MAX_BLOCK && z >= 0 &&
			z <= MAX_BLOCK)
		{
			return m_chunkInfo->mcPoints[0][theIndex];
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
				this->m_x + offsetX, this->m_y + offsetY, this->m_z + offsetZ);
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
				return neighborChunk->m_chunkInfo->mcPoints[0][neighborIndex];
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
			return m_chunkInfo->mcPoints[0][theIndex];
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
		if (!m_mesh[0])
			return;

		
		for(int i = 0; i < 3; i++)
		{
			auto chunkInfo = GameMap::shared()->fetchFromSource(m_x, m_y, m_z, i);
			m_mesh[i]->clear();
			m_meshTransition[i]->clear();
			auto VoxelBuffer = chunkInfo->mcPoints;
			TransVoxel::shared()->generateWithoutNormal(m_basePoint,
															m_mesh[i], m_meshTransition[i], (MAX_BLOCK>>i) + MIN_PADDING + MAX_PADDING, VoxelBuffer[i],
															0.0f, i);
		}
		if (m_mesh[0]->isEmpty())
			return;
		loading_mutex.lock();
		m_isNeedSubmitMesh = true;
		loading_mutex.unlock();
	}

	void Chunk::initData()
	{
		if (m_chunkInfo->isLoaded) return;
		m_chunkInfo->initData();
		// sampleForLod(1, m_chunkInfo->mcPoints_lod1);
		m_chunkInfo->isEdit = false;
		m_chunkInfo->isLoaded = true;
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
	Chunk::generateVegetation()
	{
		m_grassPosList.clear();
		m_tree->m_instance.clear();
		m_grass->m_instance.clear();
		size_t indexCount = m_mesh[0]->m_indices.size();
		if (indexCount <= 0) return;
		float grassDensity = 1.0;
		float step = 1.0 / grassDensity;
		vec3 theBasePoint = GameMap::shared()->voxelToWorldPos(this->m_x * MAX_BLOCK + LOD_SHIFT, this->m_y * MAX_BLOCK + LOD_SHIFT, this->m_z * MAX_BLOCK + LOD_SHIFT);
		for (float x = 0; x <= BLOCK_SIZE * MAX_BLOCK; x += grassDensity)
		{
			for (float z = 0; z <= BLOCK_SIZE * MAX_BLOCK; z += grassDensity)
			{
				auto ox = TbaseMath::randFN() * 0.4;
				auto oz = TbaseMath::randFN() * 0.4;
				vec3 pos(theBasePoint.x + x + ox, 0, theBasePoint.z + z + oz);
				auto h = GameMap::shared()->getHeight(pos.xz());
				pos.y = h;
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
						InstanceData instance;
						vec3 normal = GameMap::shared()->getNormal(vec2(pos.x, pos.z));
						Matrix44 mat;
						mat.setTranslate(vec3(pos.x, pos.y + 0.35, pos.z));
						
						
						instance.extraInfo = vec4(normal, 0);
						auto m = mat.data();
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
						mat.setScale(vec3(1.0 + scale));
						instance.transform = mat;
						m_grass->m_instance.push_back(instance);
					}
				}
			}
		}



		treeNoise.SetSeed(233);
		treeNoise.SetFrequency(0.02);
		treeNoise.SetNoiseType(FastNoise::Perlin);
		int treeCount = 0;
		
		for (float x = 0; x <= BLOCK_SIZE * MAX_BLOCK; x += 1.5)
		{
			for (float z = 0; z <= BLOCK_SIZE * MAX_BLOCK; z += 1.5)
			{
				auto ox = TbaseMath::randFN() * 0.2;
				auto oz = TbaseMath::randFN() * 0.2;
				float value = treeNoise.GetNoise(x + ox, 0, z + oz);
				// the flat is grass or dirt?
				// value = value * 0.5 + 0.5;
				vec3 pos = vec3(theBasePoint.x + x + ox, 0, theBasePoint.z + z + oz);
				auto h = GameMap::shared()->getHeight(pos.xz());
				pos.y = h;
				if (value > 0)
				{
					treeCount += 1;
					InstanceData instance;
					vec3 normal = vec3(0, 1,0);
					Matrix44 mat;
					mat.setTranslate(vec3(pos.x, pos.y, pos.z));
					instance.transform = mat;
					instance.extraInfo = vec4(1, 1, 1, 1);
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
		auto size = m_mesh[0]->getIndicesSize();
		float t = 0;
		for (size_t i = 0; i < size; i += 3)
		{
			if (ray.intersectTriangle(m_mesh[0]->m_vertices[m_mesh[0]->m_indices[i]].m_pos,
									m_mesh[0]->m_vertices[m_mesh[0]->m_indices[i + 1]].m_pos,
									m_mesh[0]->m_vertices[m_mesh[0]->m_indices[i + 2]].m_pos,
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
		auto size = m_mesh[0]->getIndicesSize();
		float t = 0;
		bool isFind = false;
		vec3 minP = vec3(-99999, -99999, -99999);
		for (size_t i = 0; i < size; i += 3)
		{
			if (ray.intersectTriangle(m_mesh[0]->m_vertices[m_mesh[0]->m_indices[i + 2]].m_pos,
									m_mesh[0]->m_vertices[m_mesh[0]->m_indices[i + 1]].m_pos,
									m_mesh[0]->m_vertices[m_mesh[0]->m_indices[i]].m_pos,
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
