#pragma once
#include "EngineSrc/CubeEngine.h"
#include <vector>
#include "3D/Vegetation/Grass.h"
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
		Chunk(int the_x, int the_y, int the_z);
	    int x;
	    int y;
	    int z;
		vec3 getGridPos(int the_x, int the_y, int the_z);
		bool intersectByAABB(const AABB & other, vec3 &overLap) override;
		Drawable3D * intersectByRay(const Ray & ray, vec3 &hitPoint) override;
		bool intersectBySphere(const t_Sphere & sphere, std::vector<vec3> & hitPoint) override;
		void logicUpdate(float delta) override;
		bool getIsAccpectOcTtree() const override;
		void submitDrawCmd(RenderCommand::RenderType passType) override;
		void load();
		void unload();
		void deformSphere(vec3 pos, float value, float range = 1.0f);
		void deformCube(vec3 pos, float value, float range = 1.0f);
	    void deformWithNeighbor(int X, int Y, int Z, float value);
	    void setVoxelScalar(int x, int y, int z, float scalar, bool isAdd = true);
	    void addVoexlScalar(int x, int y, int z, float scalar);
	    void genNormal();
	    voxelInfo getPoint(int index);
	    voxelInfo getPoint(int x, int y, int z);
	    int getIndex(int x, int y, int z);
	    void genMesh();
	    void initData();
		void checkCollide(ColliderEllipsoid * package) override;
		void setUpTransFormation(TransformationInfo & info) override;
		void setLod(unsigned int newLod);
		unsigned int getLod();
		void deform(int x, int y, int z, float actualVal);
		unsigned int getTypeId() override;
		vec3 m_offset;
		void calculateMatID();
		bool getIsInitData();
	private:
		Grass * m_grass;
		Grass * m_grass2;
	    bool m_isLoaded;
	    Mesh * m_mesh;
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
		
	};
}
