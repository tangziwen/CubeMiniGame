#include  "NewChunk.h"

#include "GameWorld.h"
#include "3D/Terrain/TransVoxel.h"

namespace tzw
{
tzw::NewChunk::NewChunk(int x, int y, int z):
		x(x)
		, y(y)
		, z(z)
{

	m_basePoint = vec3(x * BLOCK_SIZE * (MAX_BLOCK), y * BLOCK_SIZE * (MAX_BLOCK), -1 * z* BLOCK_SIZE * (MAX_BLOCK));
	// setPos(m_basePoint);
    m_material = Material::createFromTemplate("ModelSTD");
	auto texture =  TextureMgr::shared()->getByPath("Texture/metalgrid3-ue/metalgrid3_basecolor.png");
	m_material->setTex("DiffuseMap", texture);
	m_material->setIsCullFace(false);
	m_mesh = new Mesh();
	
	
	
	// auto offset = GameWorld::shared()->getMapOffset();
	// TransVoxel::shared()->generateWithoutNormal(vec3(0,5,0), m, realSize, reinterpret_cast<voxelInfo*>(theArray),0.0f, 0);
}

void NewChunk::submitDrawCmd(RenderCommand::RenderType passType)
{
	if(getIsVisible())
	{
		RenderCommand command(m_mesh, m_material,passType);
		setUpCommand(command);
		Renderer::shared()->addRenderCommand(command);
	}
}

void NewChunk::gen(int lodLevel)
{
	const int theBlockSize = 16;
	const int realSize = theBlockSize + (MIN_PADDING + MAX_PADDING);
	voxelInfo theArray[realSize][realSize][realSize];
	for(int i = 0; i < realSize; i++)
		for(int j = 0; j < realSize; j++)
			for(int k = 0; k < realSize; k++)
			{
				vec3 pos = vec3(i * BLOCK_SIZE, j * BLOCK_SIZE, -k * BLOCK_SIZE) + m_basePoint;
				float h =GameMap::shared()->getNoiseValue(pos.x, 0, pos.z);
				theArray[i][j][k].w =std::clamp(h - pos.y, -1.f, 1.f);
			}
	if(lodLevel == 1)
	{
		const int lodSize = (theBlockSize >> 1) + (MIN_PADDING + MAX_PADDING);
		voxelInfo theArray1[lodSize][lodSize][lodSize];
		float ratio = realSize * 1.f / lodSize;
		int stride = 1 << lodLevel;
		int gx = 0;
		// //guarantee the padding
		// for(int j = 0; j< lodSize; j++)
		// {
		// 	for(int k = 0; k< lodSize; k++)
		// 	{
		// 		theArray1[0][j][k].w = theArray[0][j][k].w;
		// 		theArray1[lodSize - MAX_PADDING][j][k].w = theArray[lodSize - MAX_PADDING][j][k].w;
		// 		theArray1[lodSize - MAX_PADDING + 1][j][k].w = theArray[lodSize - MAX_PADDING + 1][j][k].w;
		// 	}
		// }
		// for(int i = 0; i< lodSize; i++)
		// {
		// 	for(int k = 0; k< lodSize; k++)
		// 	{
		// 		theArray1[i][0][k].w = theArray[i][0][k].w;
		// 		theArray1[i][lodSize - MAX_PADDING][k].w = theArray[i][lodSize - MAX_PADDING][k].w;
		// 		theArray1[i][lodSize - MAX_PADDING + 1][k].w = theArray[i][lodSize - MAX_PADDING + 1][k].w;
		// 	}
		// }
		// for(int i = 0; i< lodSize; i++)
		// {
		// 	for(int j = 0; j< lodSize; j++)
		// 	{
		// 		theArray1[i][j][0].w = theArray[i][j][0].w;
		// 		theArray1[i][j][lodSize - MAX_PADDING].w = theArray[i][j][lodSize - MAX_PADDING].w;
		// 		theArray1[i][j][lodSize - MAX_PADDING + 1].w = theArray[i][j][lodSize - MAX_PADDING + 1].w;
		// 	}
		// }
		for(int i = 0; i < lodSize; i++, gx+= stride) 
		{
			int gy = 1;
			for(int j = 0; j < lodSize; j++, gy+= stride)
			{
				int gz = 1;
				for(int k = 0; k < lodSize; k++, gz+= stride)
				{
					// vec3 pos = vec3(gx * BLOCK_SIZE, gy * BLOCK_SIZE, -gz * BLOCK_SIZE) + m_basePoint;
					// float h =GameMap::shared()->getNoiseValue(pos.x, 0, pos.z);
					int x = floor(ratio * i);
					int y = floor(ratio * j);
					int z = floor(ratio * k);
					theArray1[i][j][k].w =theArray[x][y][z].w;//std::clamp(h - pos.y, -1.f, 1.f);
				}
				
			}
		}


		TransVoxel::shared()->generateWithoutNormal(m_basePoint, m_mesh, lodSize, reinterpret_cast<voxelInfo*>(theArray1),0.0f, lodLevel);


		// TransVoxel::shared()->generateWithoutNormal(m_basePoint + vec3(0, 15, 0), m_mesh, realSize, reinterpret_cast<voxelInfo*>(theArray),0.0f, 0);
	}
	else
	{
		TransVoxel::shared()->generateWithoutNormal(m_basePoint, m_mesh, realSize, reinterpret_cast<voxelInfo*>(theArray),0.0f, 0);
	}
	m_mesh->finish();
}
}

