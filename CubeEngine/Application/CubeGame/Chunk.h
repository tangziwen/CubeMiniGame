#pragma once
#include "EngineSrc/CubeEngine.h"
#include <vector>
#include "3D/Vegetation/Grass.h"
#include "3D/Vegetation/Tree.h"
#include "GameMap.h"


struct vertexInfo
{
    unsigned int index;
};

namespace tzw
{
	class PhysicsRigidBody;
	class ChunkInfo;
	
	class Chunk : public Drawable3D
	{
	public:
		enum class State
		{
			INVALID,
			LOADING,
			LOADED
		};
		Chunk(int the_x, int the_y, int the_z);
	    int m_x;
	    int m_y;
	    int m_z;
		vec3 getGridPos(int the_x, int the_y, int the_z);
		bool intersectByAABB(const AABB & other, vec3 &overLap) override;
		Drawable3D * intersectByRay(const Ray & ray, vec3 &hitPoint) override;
		bool intersectBySphere(const t_Sphere & sphere, std::vector<vec3> & hitPoint) override;
		void logicUpdate(float delta) override;
		bool getIsAccpectOcTtree() const override;
		void submitDrawCmd(RenderFlag::RenderStageType requirementType, RenderQueues * queues, int requirementArg) override;
		void load(int lodLevel);
		void unload();
		void deformSphere(vec3 pos, float value, float range = 1.0f);
		void deformCube(vec3 pos, float value, float range = 1.0f);
		void paintSphere(vec3 pos, int matIndex, float range = 1.0f);
	    void deformWithNeighbor(int X, int Y, int Z, std::function<void(Chunk *, int , int ,int)>neighborTrigger);
	    void setVoxelScalar(int x, int y, int z, float scalar, bool isAdd = true);
	    void addVoexlScalar(int x, int y, int z, float scalar);
		void setVoxelMat(int x, int y, int z, int matIndex, bool isAdd = true);
		voxelInfo getVoxel(int x, int y, int z);
	    voxelInfo getPoint(int index);
	    voxelInfo getPoint(int x, int y, int z);
		voxelInfo getPointInner(int x, int y, int z);
	    int getIndex(int x, int y, int z);
	    void genMesh(int lodLevel);
	    void initData();
		void setUpTransFormation(TransformationInfo & info) override;
		void setLod(unsigned int newLod);
		unsigned int getLod();
		void deform(int x, int y, int z, float actualVal);
		void paint(int x, int y, int z, int matIndex);
		unsigned int getTypeId() override;
		void generateVegetation();
		bool getIsInitData();
		State m_currenState;
		ChunkInfo * getChunkInfo();
		int getCurrentLod();
	private:
		int m_currentLOD;
		TreeGroup * m_grass;
		TreeGroup * m_grass2;
		TreeGroup * m_tree;
	    Mesh * m_mesh[3];
		Mesh * m_meshTransition[3];
		void sampleForLod(int lodLevel, voxelInfo * out);
		bool isInEdge(int i, int j, int k);
	    bool isInRange(int i,int j, int k);
	    bool isInOutterRange(int i, int j, int k);
	    bool isInInnerRange(int i, int j, int k);
	    bool isEdge(int i);

	    bool hitAny(Ray & ray, vec3 & result);
	    bool hitFirst(const Ray &ray, vec3 & result);
		ChunkInfo * m_chunkInfo;
	    vec3 m_basePoint;
	    std::vector<Chunk *> m_tmpNeighborChunk;
		std::vector<vec4> m_grassPosList;
		unsigned int m_lod;
		bool m_isNeedSubmitMesh;
		PhysicsRigidBody * m_rigidBody;
		bool m_isTreeloaded;
	};
}
