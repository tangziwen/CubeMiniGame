#ifndef CHUNK_H
#define CHUNK_H
#include "EngineSrc/CubeEngine.h"
#include <string>
#include <vector>
#include "3D/Vegetation/Grass.h"
struct vertexInfo
{
    unsigned int index;
};

namespace tzw
{

/// <summary>
/// 
/// </summary>
/// <seealso cref="Drawable3D" />
	class Chunk : public Drawable3D
{
public:	

	/// <summary>
	/// Initializes a new instance of the <see cref="Chunk"/> class.
	/// </summary>
	/// <param name="the_x">The x.</param>
	/// <param name="the_y">The y.</param>
	/// <param name="the_z">The z.</param>
	Chunk(int the_x, int the_y, int the_z);
    int x;
    int y;
    int z;
	/// <summary>
	/// Gets the grid position.
	/// </summary>
	/// <param name="the_x">The x.</param>
	/// <param name="the_y">The y.</param>
	/// <param name="the_z">The z.</param>
	/// <returns>
	/// tzw.Chunk
	/// </returns>
	vec3 getGridPos(int the_x, int the_y, int the_z);
	/// <summary>
	/// Intersects the by aabb.
	/// </summary>
	/// <param name="other">The other.</param>
	/// <param name="overLap">The over lap.</param>
	/// <returns>
	/// tzw.Chunk
	/// </returns>
	bool intersectByAABB(const AABB & other, vec3 &overLap) override;
	/// <summary>
	/// Intersects the by ray.
	/// </summary>
	/// <param name="ray">The ray.</param>
	/// <param name="hitPoint">The hit point.</param>
	/// <returns>
	/// tzw.Chunk
	/// </returns>
	Drawable3D * intersectByRay(const Ray & ray, vec3 &hitPoint) override;
	/// <summary>
	/// Intersects the by sphere.
	/// </summary>
	/// <param name="sphere">The sphere.</param>
	/// <param name="hitPoint">The hit point.</param>
	/// <returns>
	/// tzw.Chunk
	/// </returns>
	bool intersectBySphere(const t_Sphere & sphere, std::vector<vec3> & hitPoint) override;
	/// <summary>
	/// Logics the update.
	/// </summary>
	/// <param name="delta">The delta.</param>
	void logicUpdate(float delta) override;
	/// <summary>
	/// Gets the is accpect oc ttree.
	/// </summary>
	/// <returns>
	/// tzw.Chunk
	/// </returns>
	bool getIsAccpectOCTtree() const override;
	/// <summary>
	/// Submits the draw command.
	/// </summary>
	void submitDrawCmd(RenderCommand::RenderType passType) override;
	/// <summary>
	/// Loads this instance.
	/// </summary>
	void load();
	/// <summary>
	/// Unloads this instance.
	/// </summary>
	void unload();
	/// <summary>
	/// Deforms the sphere.
	/// </summary>
	/// <param name="pos">The position.</param>
	/// <param name="value">The value.</param>
	/// <param name="range">The range.</param>
	void deformSphere(vec3 pos, float value, float range = 1.0f);
	void deformCube(vec3 pos, float value, float range = 1.0f);
    void deformWithNeighbor(int X, int Y, int Z, float value);
    void setVoxelScalar(int x, int y, int z, float scalar, bool isAdd = true);
    void addVoexlScalar(int x, int y, int z, float scalar);
    void genNormal();
    vec4 getPoint(int index);
    vec4 getPoint(int x, int y, int z);
    int getIndex(int x, int y, int z);
    void genMesh();
    void initData();
	void checkCollide(ColliderEllipsoid * package) override;
	void setUpTransFormation(TransformationInfo & info) override;
	void setLod(unsigned int newLod);
	unsigned int getLod();
	void deform(int x, int y, int z, float actualVal);
	unsigned int getTypeID() override;
	vec3 m_offset;
	void calculatorMatID();
private:
	Grass * m_grass;
	Grass * m_grass2;
    bool m_isLoaded;
    bool m_isInitData;
    Mesh * m_mesh;
	bool isInEdge(int i, int j, int k);
    bool isInRange(int i,int j, int k);
    bool isInOutterRange(int i, int j, int k);
    bool isInInnerRange(int i, int j, int k);
    bool isEdge(int i);

    bool hitAny(Ray & ray, vec3 & result);
    bool hitFirst(const Ray &ray, vec3 & result);
    vec4 * mcPoints;
    vec3 m_basePoint;
    std::vector<Chunk *> m_tmpNeighborChunk;
	std::vector<vec4> m_grassPosList;
	unsigned int m_lod;
	bool m_isNeedSubmitMesh;
};
}


#endif // CHUNK_H
