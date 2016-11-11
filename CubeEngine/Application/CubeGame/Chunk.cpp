#include "Chunk.h"
#include "GameMap.h"
#include "GameWorld.h"
#include "algorithm"
#include "time.h"
#include "../EngineSrc/Texture/TextureMgr.h"
#include "../EngineSrc/3D/Terrain/MarchingCubes.h"
#include "GameConfig.h"
#include <iostream>
static int CHUNK_OFFSET  =BLOCK_SIZE * MAX_BLOCK / 2;
static int CHUNK_SIZE = BLOCK_SIZE * MAX_BLOCK;
#include "../EngineSrc/3D/Terrain/MCTable.h"
#include "../EngineSrc/Collision/CollisionUtility.h"
namespace tzw {
Chunk::Chunk(int the_x, int the_y,int the_z)
	:m_isLoaded(false),x(the_x),y(the_y),z(the_z)
{
	m_localAABB.update(vec3(0,0,0));
	m_localAABB.update(vec3(MAX_BLOCK * BLOCK_SIZE,MAX_BLOCK * BLOCK_SIZE,-1 * MAX_BLOCK * BLOCK_SIZE));
	m_basePoint = vec3(x*CHUNK_SIZE,y* CHUNK_SIZE , -1 * z* CHUNK_SIZE);
	setPos(m_basePoint);
	m_mesh = nullptr;
	m_needToUpdate = true;
	m_isInitData = false;
	reCacheAABB();
}

vec3 Chunk::getGridPos(int the_x, int the_y, int the_z)
{
	return vec3(x*MAX_BLOCK+the_x,y*MAX_BLOCK+the_y,-1 * (z*MAX_BLOCK+the_z));
}

bool Chunk::intersectByAABB(const AABB &other, vec3 &overLap)
{
	if(!m_isLoaded)
		return false;
	auto size = m_mesh->getIndicesSize();
	for (size_t i =0; i< size; i+=3)
	{
		for(int k = 0; k < 3; k++)
		{
			if(other.isInside(m_mesh->m_vertices[i + k].m_pos))
			{
				return true;
			}
		}
	}
	return false;
}

Drawable3D *Chunk::intersectByRay(const Ray &ray, vec3 &hitPoint)
{
	if(this->hitFirst(ray, hitPoint))
	{
		return this;
	}
	return nullptr;
}

bool Chunk::intersectBySphere(const t_Sphere &sphere, std::vector<vec3> &hitPoint)
{
	if(!m_isLoaded)
		return false;
	auto size = m_mesh->getIndicesSize();
	std::vector<vec3> resultList;
	float t = 0;
	for (auto i =0; i< size; i+=3)
	{
		vec3 tmpHitPoint;
		if(sphere.intersectWithTriangle(m_mesh->m_vertices[i + 2].m_pos, m_mesh->m_vertices[i + 1].m_pos, m_mesh->m_vertices[i].m_pos, tmpHitPoint))
		{
			resultList.push_back(tmpHitPoint);
		}
	}
	if(!resultList.empty())
	{
		std::sort(resultList.begin(),resultList.end(),[sphere](const vec3 & v1, const vec3 & v2)    {
			float dist1 = sphere.centre().distance(v1);
			float dist2 = sphere.centre().distance(v2);
			return dist1<dist2;
		});
		hitPoint = resultList;
		return true;
	}
	return false;
}

void Chunk::logicUpdate(float delta)
{

}

bool Chunk::getIsAccpectOCTtree() const
{
	return true;
}

void Chunk::submitDrawCmd()
{
	if (!m_isLoaded) return;
	RenderCommand command(m_mesh, m_material, RenderCommand::RenderType::Common);
	setUpTransFormation(command.m_transInfo);
	command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
	Renderer::shared()->addRenderCommand(command);
}


void Chunk::load()
{
	if(m_isLoaded)return;
	if(!m_isInitData) initData();
	setCamera(g_GetCurrScene()->defaultCamera());
	//强制重缓存数据
	reCache();
	if (!m_mesh)
	{
		m_mesh = new Mesh();
		m_material = Material::createFromEffect("VoxelTerrain");
		m_material->setTex("GrassTex", TextureMgr::shared()->getByPath("./Res/TestRes/grass_green_d.jpg", true));
		m_material->setTex("DirtTex", TextureMgr::shared()->getByPath("./Res/TestRes/adesert_mntn4v_d.jpg", true));
 		if((x + z + y) % 2 == 0)
 			this->getMaterial()->setVar("color",vec4::fromRGB(255, 0, 0));
 		else
 			this->getMaterial()->setVar("color",vec4::fromRGB(0, 255, 0));
	}
	finish();
	m_isLoaded = true;
}

void Chunk::unload()
{
	m_isLoaded = false;
	m_mesh->clear();
}

void Chunk::deformAround(vec3 pos, float value)
{
	m_tmpNeighborChunk.clear();
	vec3 relativePost = pos - m_basePoint;
	relativePost = relativePost / BLOCK_SIZE;
	relativePost.z *= -1;
	int posX = relativePost.x;
	int posY = relativePost.y;
	int posZ = relativePost.z;
	auto theDrawSize = 2;
	for (int i = -theDrawSize; i <= theDrawSize; i++)
	{
		for(int j = -theDrawSize; j<=theDrawSize; j++)
		{
			for(int k = -theDrawSize; k<= theDrawSize; k++)
			{
				float actualValue = value * (1.0 - sqrt(i * i + j * j + k * k) / sqrt(theDrawSize * theDrawSize + theDrawSize * theDrawSize + theDrawSize * theDrawSize));
				int X = posX + i;
				int Y = posY + j;
				int Z = posZ + k;
				if(!isInInnerRange(X, Y, Z))
				{
					deformWithNeighbor(X, Y, Z, actualValue);
					continue;
				}
				setVoxelScalar(X, Y, Z, actualValue);
			}
		}
	}

	finish();
	if(!m_tmpNeighborChunk.empty())
	{
		for(auto chunk :m_tmpNeighborChunk)
		{
			chunk->finish();
		}
	}
	m_tmpNeighborChunk.clear();
}


void Chunk::deformWithNeighbor(int X, int Y, int Z, float value)
{
	std::vector<int> xList;
	std::vector<int> yList;
	std::vector<int> zList;

	//判断是否需要邻近的chunk
	if(X <= 0 || X >= MAX_BLOCK)
	{
		if ( X <= 0) xList.push_back(-1); else xList.push_back(1);
	}
	if(Y <= 0 || Y >= MAX_BLOCK)
	{
		if ( Y <= 0) yList.push_back(-1); else {yList.push_back(1);}
	}

	if(Z <= 0 || Z >= MAX_BLOCK)
	{
		if ( Z <= 0) zList.push_back(-1); else zList.push_back(1);
	}
//	//判断是否需要把自己加进去
//	if(isInEdge(X,Y, Z))
//	{

//	}
//	if(xList.empty()) xList.push_back(0);
//	if(yList.empty()) yList.push_back(0);
//	if(zList.empty()) zList.push_back(0);

	xList.push_back(0);
	yList.push_back(0);
	zList.push_back(0);
	for(int offsetX : xList)
	{
		for(int offsetY : yList)
		{
			for(int offsetZ: zList)
			{
				auto neighborChunk = GameWorld::shared()->getChunkByChunk(this->x + offsetX, this->y + offsetY, this->z + offsetZ);
				if(neighborChunk)
				{
					int nx = X;
					int ny = Y;
					int nz = Z;
					if(offsetX == -1) nx += MAX_BLOCK; else if(offsetX == 1 ) nx -= MAX_BLOCK;
					if(offsetY == -1) {ny += MAX_BLOCK;} else if(offsetY == 1 ) {ny -= MAX_BLOCK;}
					if(offsetZ == -1) nz += MAX_BLOCK; else if(offsetZ == 1 ) nz -= MAX_BLOCK;
					neighborChunk->setVoxelScalar(nx, ny, nz, value);
					auto result = std::find(m_tmpNeighborChunk.begin(), m_tmpNeighborChunk.end(), neighborChunk);
					if (result == m_tmpNeighborChunk.end() && neighborChunk != this)
					{
						m_tmpNeighborChunk.push_back(neighborChunk);
					}
				}
			}
		}
	}
}
void Chunk::setVoxelScalar(int x, int y, int z, float scalar)
{
	if(!isInOutterRange(x,y,z)) return;
	int YtimeZ = (MAX_BLOCK + 1) * (MAX_BLOCK + 1);
	int ind = x*YtimeZ + y*(MAX_BLOCK + 1) + z;
	mcPoints[ind].w += scalar;
}

void Chunk::addVoexlScalar(int x, int y, int z, float scalar)
{
	//	std::cout<<"chunk ("<<this->x<<this->y << this->z<<")"<< " set!!!!!("<<x <<" , "<< y <<" , "<< z <<") as"<< scalar << std::endl;
	int YtimeZ = (MAX_BLOCK + 1) * (MAX_BLOCK + 1);
	int ind = x*YtimeZ + y*(MAX_BLOCK + 1) + z;
	mcPoints[ind].w += scalar;
	if(mcPoints[ind].w > 1.0)
		mcPoints[ind].w = 1.0;
}

static vec3 LinearInterp(vec4 & p1, vec4 & p2, float value)
{
	vec3 p;
	vec3 tp1 = vec3(p1.x, p1.y, p1.z);
	vec3 tp2 = vec3(p2.x, p2.y, p2.z);
	if(p1.w != p2.w)
		p = tp1 + (tp2 - tp1)/(p2.w - p1.w)*(value - p1.w);
	else
		p = tp1;
	return p;
}
#define CALC_GRAD_VERT_0(verts) vec4(getPoint(i - 1, j, k).w-(verts[1]).w,getPoint(i, j - 1, k).w-(verts[4]).w,getPoint(i, j, k - 1).w-(verts[3]).w, (verts[0]).w);
#define CALC_GRAD_VERT_1(verts) vec4((verts[0]).w-getPoint(i + 2, j, k).w,getPoint(i + 1, j - 1, k ).w-(verts[5]).w,getPoint(i + 1, j, k - 1).w-(verts[2]).w, (verts[1]).w);
#define CALC_GRAD_VERT_2(verts) vec4((verts[3]).w-getPoint(i + 2, j, k + 1).w,getPoint(i + 1, j - 1, k + 1).w-(verts[6]).w,(verts[1]).w-getPoint(i + 1, j, k + 2).w, (verts[2]).w);
#define CALC_GRAD_VERT_3(verts) vec4(getPoint(i - 1, j, k + 1).w-(verts[2]).w,getPoint(i, j - 1, k + 1).w-(verts[7]).w,(verts[0]).w-getPoint(i, j, k + 2).w, (verts[3]).w);
#define CALC_GRAD_VERT_4(verts) vec4(getPoint(i - 1, j + 1, k).w-(verts[5]).w,(verts[0]).w-getPoint(i, j + 2, k).w,getPoint(i, j + 1, k - 1).w-(verts[7]).w, (verts[4]).w);
#define CALC_GRAD_VERT_5(verts) vec4((verts[4]).w-getPoint(i + 2, j + 1, k).w,(verts[1]).w-getPoint(i + 1, j + 2, k).w,getPoint(i + 1, j + 1, k -1 ).w-(verts[6]).w, (verts[5]).w);
#define CALC_GRAD_VERT_6(verts) vec4((verts[7]).w-getPoint(i + 2, j + 1, k + 1).w,(verts[2]).w-getPoint(i + 1, j + 2, k + 1).w,(verts[5]).w-getPoint(i + 1, j + 1, k + 2).w, (verts[6]).w);
#define CALC_GRAD_VERT_7(verts) vec4(getPoint(i - 1, j + 1, k + 1).w-(verts[6]).w,(verts[3]).w-getPoint(i, j + 2, k  + 1).w,(verts[4]).w-getPoint(i, j + 1, k + 2).w, (verts[7]).w);

void Chunk::genNormal()
{
	auto points = mcPoints;
	auto ncellsZ = MAX_BLOCK;
	auto ncellsY = MAX_BLOCK;
	auto ncellsX = MAX_BLOCK;
	float minValue = 0.0f;
	int meshIndex = 0;
	int pointsZ = ncellsZ+1;
	int YtimeZ = (ncellsY+1)*(ncellsZ+1);
	//go through all the points
	for(int i=0; i < ncellsX; i++)//x axis
		for(int j=0; j < ncellsY; j++)//y axis
			for(int k=0; k < ncellsZ; k++)	//z axis
			{
				//initialize vertices
				vec4 verts[8];
				int ind = i*YtimeZ + j*(ncellsZ+1) + k;
				vec4 gradVerts[8];			//gradients at each vertex of a cube
				vec3 grads[12];				//linearly interpolated gradients on each edge
				int lastX = ncellsX;			//left from older version
				int lastY = ncellsY;
				int lastZ = ncellsZ;
				float gradFactorX = 0.5;
				float gradFactorY = 0.5;
				float gradFactorZ = 0.5;
				vec3 factor(1.0/(2.0*gradFactorX), 1.0/(2.0*gradFactorY), 1.0/(2.0*gradFactorZ));
				verts[0] = points[ind];
				verts[1] = points[ind + YtimeZ];
				verts[2] = points[ind + YtimeZ + 1];
				verts[3] = points[ind + 1];
				verts[4] = points[ind + (ncellsZ+1)];
				verts[5] = points[ind + YtimeZ + (ncellsZ+1)];
				verts[6] = points[ind + YtimeZ + (ncellsZ+1) + 1];
				verts[7] = points[ind + (ncellsZ+1) + 1];

				//get the index
				int cubeIndex = int(0);
				for(int n=0; n < 8; n++)
					/*(step 4)*/		if(verts[n].w <= minValue) cubeIndex |= (1 << n);

				//check if its completely inside or outside
				/*(step 5)*/ if(!edgeTable[cubeIndex]) continue;
				vec3 intVerts[12];
				int indGrad = 0;
				auto edgeIndex = edgeTable[cubeIndex];
				if(edgeIndex & 1) {
					intVerts[0] = LinearInterp(verts[0], verts[1], minValue);
					gradVerts[0] = CALC_GRAD_VERT_0(verts)
							gradVerts[1] = CALC_GRAD_VERT_1(verts)
							indGrad |= 3;
					grads[0] = LinearInterp(gradVerts[0], gradVerts[1], minValue);
					grads[0].x *= factor.x; grads[0].y *= factor.y; grads[0].z *= factor.z;
				}
				if(edgeIndex & 2) {
					intVerts[1] = LinearInterp(verts[1], verts[2], minValue);
					if(! (indGrad & 2)) {
						gradVerts[1] = CALC_GRAD_VERT_1(verts)
								indGrad |= 2;
					}
					gradVerts[2] = CALC_GRAD_VERT_2(verts)
							indGrad |= 4;
					grads[1] = LinearInterp(gradVerts[1], gradVerts[2], minValue);
					grads[1].x *= factor.x; grads[1].y *= factor.y; grads[1].z *= factor.z;
				}
				if(edgeIndex & 4) {
					intVerts[2] = LinearInterp(verts[2], verts[3], minValue);
					if(! (indGrad & 4)) {
						gradVerts[2] = CALC_GRAD_VERT_2(verts)
								indGrad |= 4;
					}
					gradVerts[3] = CALC_GRAD_VERT_3(verts)
							indGrad |= 8;
					grads[2] = LinearInterp(gradVerts[2], gradVerts[3], minValue);
					grads[2].x *= factor.x; grads[2].y *= factor.y; grads[2].z *= factor.z;
				}
				if(edgeIndex & 8) {
					intVerts[3] = LinearInterp(verts[3], verts[0], minValue);
					if(! (indGrad & 8)) {
						gradVerts[3] = CALC_GRAD_VERT_3(verts)
								indGrad |= 8;
					}
					if(! (indGrad & 1)) {
						gradVerts[0] = CALC_GRAD_VERT_0(verts)
								indGrad |= 1;
					}
					grads[3] = LinearInterp(gradVerts[3], gradVerts[0], minValue);
					grads[3].x *= factor.x; grads[3].y *= factor.y; grads[3].z *= factor.z;
				}
				if(edgeIndex & 16) {
					intVerts[4] = LinearInterp(verts[4], verts[5], minValue);
					gradVerts[4] = CALC_GRAD_VERT_4(verts)

							gradVerts[5] = CALC_GRAD_VERT_5(verts)

							indGrad |= 48;
					grads[4] = LinearInterp(gradVerts[4], gradVerts[5], minValue);
					grads[4].x *= factor.x; grads[4].y *= factor.y; grads[4].z *= factor.z;
				}
				if(edgeIndex & 32) {
					intVerts[5] = LinearInterp(verts[5], verts[6], minValue);
					if(! (indGrad & 32)) {
						gradVerts[5] = CALC_GRAD_VERT_5(verts)
								indGrad |= 32;
					}

					gradVerts[6] = CALC_GRAD_VERT_6(verts)
							indGrad |= 64;
					grads[5] = LinearInterp(gradVerts[5], gradVerts[6], minValue);
					grads[5].x *= factor.x; grads[5].y *= factor.y; grads[5].z *= factor.z;
				}
				if(edgeIndex & 64) {
					intVerts[6] = LinearInterp(verts[6], verts[7], minValue);
					if(! (indGrad & 64)) {
						gradVerts[6] = CALC_GRAD_VERT_6(verts)
								indGrad |= 64;
					}
					gradVerts[7] = CALC_GRAD_VERT_7(verts);
					indGrad |= 128;
					grads[6] = LinearInterp(gradVerts[6], gradVerts[7], minValue);
					grads[6].x *= factor.x; grads[6].y *= factor.y; grads[6].z *= factor.z;
				}
				if(edgeIndex & 128) {
					intVerts[7] = LinearInterp(verts[7], verts[4], minValue);
					if(! (indGrad & 128)) {
						gradVerts[7] = CALC_GRAD_VERT_7(verts)
								indGrad |= 128;
					}
					if(! (indGrad & 16)) {
						gradVerts[4] = CALC_GRAD_VERT_4(verts)
								indGrad |= 16;
					}
					grads[7] = LinearInterp(gradVerts[7], gradVerts[4], minValue);
					grads[7].x *= factor.x; grads[7].y *= factor.y; grads[7].z *= factor.z;
				}
				if(edgeIndex & 256) {
					intVerts[8] = LinearInterp(verts[0], verts[4], minValue);
					if(! (indGrad & 1)) {
						gradVerts[0] = CALC_GRAD_VERT_0(verts)
								indGrad |= 1;
					}
					if(! (indGrad & 16)) {
						gradVerts[4] = CALC_GRAD_VERT_4(verts)
								indGrad |= 16;
					}
					grads[8] = LinearInterp(gradVerts[0], gradVerts[4], minValue);
					grads[8].x *= factor.x; grads[8].y *= factor.y; grads[8].z *= factor.z;
				}
				if(edgeIndex & 512) {
					intVerts[9] = LinearInterp(verts[1], verts[5], minValue);
					if(! (indGrad & 2)) {
						gradVerts[1] = CALC_GRAD_VERT_1(verts)
								indGrad |= 2;
					}
					if(! (indGrad & 32)) {
						gradVerts[5] = CALC_GRAD_VERT_5(verts)
								indGrad |= 32;
					}
					grads[9] = LinearInterp(gradVerts[1], gradVerts[5], minValue);
					grads[9].x *= factor.x; grads[9].y *= factor.y; grads[9].z *= factor.z;
				}
				if(edgeIndex & 1024) {
					intVerts[10] = LinearInterp(verts[2], verts[6], minValue);
					if(! (indGrad & 4)) {
						gradVerts[2] = CALC_GRAD_VERT_2(verts)
								indGrad |= 4;
					}
					if(! (indGrad & 64)) {
						gradVerts[6] = CALC_GRAD_VERT_6(verts)
								indGrad |= 64;
					}
					grads[10] = LinearInterp(gradVerts[2], gradVerts[6], minValue);
					grads[10].x *= factor.x; grads[10].y *= factor.y; grads[10].z *= factor.z;
				}
				if(edgeIndex & 2048) {
					intVerts[11] = LinearInterp(verts[3], verts[7], minValue);
					if(! (indGrad & 8)) {
						gradVerts[3] = CALC_GRAD_VERT_3(verts)
								indGrad |= 8;
					}
					if(! (indGrad & 128)) {
						gradVerts[7] = CALC_GRAD_VERT_7(verts)
								indGrad |= 128;
					}
					grads[11] = LinearInterp(gradVerts[3], gradVerts[7], minValue);
					grads[11].x *= factor.x; grads[11].y *= factor.y; grads[11].z *= factor.z;
				}

				//now build the triangles using triTable
				for (int n=0; triTable[cubeIndex][n] != -1; n+=3)
				{
					m_mesh->m_vertices[meshIndex].m_normal = grads[triTable[cubeIndex][n+2]];
					m_mesh->m_vertices[meshIndex + 1].m_normal = grads[triTable[cubeIndex][n+1]];
					m_mesh->m_vertices[meshIndex + 2].m_normal = grads[triTable[cubeIndex][n]];
					meshIndex += 3;
				}
			}	//END OF FOR LOOP
}

vec4 Chunk::getPoint(int index)
{
	if( index >= 0 && index < (MAX_BLOCK + 1) * (MAX_BLOCK + 1) * (MAX_BLOCK + 1))
	{
		return mcPoints[index];
	}
	return mcPoints[index];
}

vec4 Chunk::getPoint(int x, int y, int z)
{
	int size = MAX_BLOCK + 1;
	int theIndex = x * size * size + y * size + z;
	if( x >= 0 && x <= MAX_BLOCK && y >=0 && y <= MAX_BLOCK && z >= 0 && z <= MAX_BLOCK)
	{
		return mcPoints[theIndex];
	}else
	{
		//确定象限,这里和deform不同，不需要考虑边缘的情况
		int offsetX = 0;
		int offsetY = 0;
		int offsetZ = 0;
		if(x < 0)
		{
			offsetX = -1;
		}
		if(x > MAX_BLOCK)
		{
			offsetX = 1;
		}
		if(y < 0)
		{
			offsetY = -1;
		}
		if(y > MAX_BLOCK)
		{
			offsetY = 1;
		}
		if(z < 0)
		{
			offsetZ = -1;
		}
		if(z > MAX_BLOCK)
		{
			offsetZ = 1;
		}
		auto neighborChunk = GameWorld::shared()->getChunkByChunk(this->x + offsetX, this->y + offsetY, this->z + offsetZ);
		if(neighborChunk)
		{
			if(!neighborChunk->m_isInitData)
			{
				neighborChunk->initData();
			}
			int nx = x;
			int ny = y;
			int nz = z;
			if(x < 0) nx += MAX_BLOCK; else if( x > MAX_BLOCK) nx -= MAX_BLOCK;
			if(y < 0) ny += MAX_BLOCK; else if( y > MAX_BLOCK) ny -= MAX_BLOCK;
			if(z < 0) nz += MAX_BLOCK; else if( z > MAX_BLOCK) nz -= MAX_BLOCK;
			int neighborIndex = nx * size * size + ny * size + nz;
			return neighborChunk->mcPoints[neighborIndex];
		}
		else
		{
			return vec4(1.0, 1.0, 1.0, 1.0);
		}
	}

}

int Chunk::getIndex(int x, int y, int z)
{
	int size = MAX_BLOCK + 1;
	int theIndex = x * size * size + y * size + z;
	return theIndex;
}

bool Chunk::isInRange(int i, int j, int k)
{
	return i>=0 && i< MAX_BLOCK && j>=0 && j< MAX_BLOCK && k>=0 && k<MAX_BLOCK;
}

bool Chunk::isInOutterRange(int i, int j, int k)
{
	return i>= 0 && i <= MAX_BLOCK && j >= 0 && j <= MAX_BLOCK && k >= 0 && k <= MAX_BLOCK;
}

bool Chunk::isInInnerRange(int i, int j, int k)
{
	return i> 0 && i < MAX_BLOCK && j > 0 && j < MAX_BLOCK && k > 0 && k < MAX_BLOCK;
}

bool Chunk::isEdge(int i)
{
	return i == 0 || i == MAX_BLOCK;
}

void Chunk::finish()
{
	MarchingCubes::shared()->generateWithoutNormal(m_mesh, MAX_BLOCK, MAX_BLOCK, MAX_BLOCK, mcPoints, 0.0f);
	//genNormal();
	m_mesh->caclNormals();
	m_mesh->calBaryCentric();
	m_mesh->finish();
}

void Chunk::initData()
{
	mcPoints = new vec4[(MAX_BLOCK +1) * (MAX_BLOCK+1) * (MAX_BLOCK + 1)];
	int YtimeZ = (MAX_BLOCK + 1) * (MAX_BLOCK + 1);
	for(int i =0;i<MAX_BLOCK + 1;i++)
	{
		for(int j=0;j<MAX_BLOCK + 1;j++)
		{
			for(int k=0;k<MAX_BLOCK + 1;k++)
			{
				vec4 verts;
				verts = vec4(i * BLOCK_SIZE, j * BLOCK_SIZE, -1 * k * BLOCK_SIZE, -1) + vec4(m_basePoint, 0);
				verts.w = GameMap::shared()->getDensity(verts.toVec3());
				//x y z
				int ind = i*YtimeZ + j*(MAX_BLOCK + 1) + k;
				mcPoints[ind] = verts;
			}
		}
	}
	m_isInitData = true;
}

void Chunk::checkCollide(ColliderEllipsoid *package)
{
	if(!m_isLoaded)
		return ;
	auto size = m_mesh->getIndicesSize();
	std::vector<vec3> resultList;
	float t = 0;
	for (auto i =0; i< size; i+=3)
	{
		CollisionUtility::checkTriangle(package,
										package->toE(m_mesh->m_vertices[i + 2].m_pos),
				package->toE(m_mesh->m_vertices[i + 1].m_pos),
				package->toE(m_mesh->m_vertices[i].m_pos));
	}
}

void Chunk::setUpTransFormation(TransformationInfo &info)
{
	info.m_projectMatrix = camera()->projection();
	info.m_viewMatrix = camera()->getViewMatrix();
	Matrix44 mat;
	mat.setToIdentity();
	info.m_worldMatrix = mat;
}

bool Chunk::isInEdge(int i, int j, int k)
{
	if (isInOutterRange(i, j, k))
	{
		return (i == 0 || i == MAX_BLOCK) || (j == 0 || j == MAX_BLOCK) || (k == 0 || k == MAX_BLOCK);
	}
	return false;
}

bool Chunk::hitAny(Ray &ray, vec3 &result)
{
	auto size = m_mesh->getIndicesSize();
	float t = 0;
	for (auto i =0; i< size; i+=3)
	{
		if(ray.intersectTriangle(m_mesh->m_vertices[i].m_pos,m_mesh->m_vertices[i + 1].m_pos,m_mesh->m_vertices[i + 2].m_pos, &t))
		{
			result = ray.origin() + ray.direction() * t;
			return true;
		}
	}
	return false;
}

bool Chunk::hitFirst(const Ray &ray, vec3 &result)
{
	if(!m_isLoaded)
		return false;
	auto size = m_mesh->getIndicesSize();
	float t = 0;
	bool isFind = false;
	vec3 minP = vec3(-99999, -99999, -99999);
	for (auto i =0; i< size; i+=3)
	{
		if(ray.intersectTriangle(m_mesh->m_vertices[i + 2].m_pos,m_mesh->m_vertices[i + 1].m_pos,m_mesh->m_vertices[i].m_pos, &t))
		{
			auto tmp = ray.origin() + ray.direction() * t;
			if(tmp.distance(ray.origin()) < minP.distance(ray.origin()))
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




