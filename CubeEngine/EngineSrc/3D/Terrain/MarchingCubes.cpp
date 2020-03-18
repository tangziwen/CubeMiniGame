#include "MarchingCubes.h"

#include "MCTable.h"
#include "CubeGame/GameMap.h"

namespace tzw {
	TZW_SINGLETON_IMPL(MarchingCubes)

		
#define CALC_GRAD_VERT_0(verts) vec4(points[ind-YtimeZ].w-(verts[1]).w,points[ind-pointsZ].w-(verts[4]).w,points[ind-1].w-(verts[3]).w, (verts[0]).w);
#define CALC_GRAD_VERT_1(verts) vec4((verts[0]).w-points[ind+2*YtimeZ].w,points[ind+YtimeZ-pointsZ].w-(verts[5]).w,points[ind+YtimeZ-1].w-(verts[2]).w, (verts[1]).w);
#define CALC_GRAD_VERT_2(verts) vec4((verts[3]).w-points[ind+2*YtimeZ+1].w,points[ind+YtimeZ-ncellsZ].w-(verts[6]).w,(verts[1]).w-points[ind+YtimeZ+2].w, (verts[2]).w);
#define CALC_GRAD_VERT_3(verts) vec4(points[ind-YtimeZ+1].w-(verts[2]).w,points[ind-ncellsZ].w-(verts[7]).w,(verts[0]).w-points[ind+2].w, (verts[3]).w);
#define CALC_GRAD_VERT_4(verts) vec4(points[ind-YtimeZ+ncellsZ+1].w-(verts[5]).w,(verts[0]).w-points[ind+2*pointsZ].w,points[ind+ncellsZ].w-(verts[7]).w, (verts[4]).w);
#define CALC_GRAD_VERT_5(verts) vec4((verts[4]).w-points[ind+2*YtimeZ+ncellsZ+1].w,(verts[1]).w-points[ind+YtimeZ+2*pointsZ].w,points[ind+YtimeZ+ncellsZ].w-(verts[6]).w, (verts[5]).w);
#define CALC_GRAD_VERT_6(verts) vec4((verts[7]).w-points[ind+2*YtimeZ+ncellsZ+2].w,(verts[2]).w-points[ind+YtimeZ+2*ncellsZ+3].w,(verts[5]).w-points[ind+YtimeZ+ncellsZ+3].w, (verts[6]).w);
#define CALC_GRAD_VERT_7(verts) vec4(points[ind-YtimeZ+ncellsZ+2].w-(verts[6]).w,(verts[3]).w-points[ind+2*ncellsZ+3].w,(verts[4]).w-points[ind+ncellsZ+3].w, (verts[7]).w);


		
static int lodList[] = { 1, 2, 4, 8 };
VoxelVertex LinearInterp(voxelInfo & p1, voxelInfo & p2, float value)
{
    VoxelVertex p;
    vec3 tp1 = vec3(p1.x, p1.y, p1.z);
    vec3 tp2 = vec3(p2.x, p2.y, p2.z);
    if(p1.w != p2.w)
    {
    	p.vertex = (tp1 + (tp2 - tp1)/(p2.w - p1.w)*(value - p1.w));
    	p.matInfo = tp1.distance(p.vertex) > tp2.distance(p.vertex)? p2.matInfo:p1.matInfo;
    }
    else
    {
		p.vertex = (tp1);
    	p.matInfo = p1.matInfo;
    }

    return p;
}

VertexData genVertexData(VoxelVertex & v)
{
	VertexData a(v.vertex);
	a.m_matIndex[0] = v.matInfo.matIndex1;
	a.m_matIndex[1] = v.matInfo.matIndex2;
	a.m_matIndex[2] = v.matInfo.matIndex3;
	a.m_matBlendFactor = v.matInfo.matBlendFactor;
	a.m_texCoord = v.vertex.xz();
	return a;
}

void calcMaterial(VertexData & data, voxelInfo * info, voxelInfo *srcData)
{
	return;
	vec3 realPos = data.m_pos;
	int X = realPos.x / 2.0f;
	int Y = realPos.y / 2.0f;
	int Z = realPos.z / 2.0f;
	//单步MarchingCube内计算是不够的，还得扩大范围，以这个最近点基础上再搜相邻得点，然后再算
	std::vector<voxelInfo *> m_voxel;
	for(int i = X -1; i <= X +1; i++)
	{
		for(int j = Y -1; j <= Y +1; j++)
		{
			for(int k = Z -1; k <= Z +1; k++)
			{
				
			}
		}
	}
	/*
	float theMinDist = 9999;
	// voxelInfo minInfo = info[0];
	int minIndex = -1;
	for(int i = 0; i < 8; i++)
	{
		auto dist = info[i].getV3().distance(data.m_pos);
		if( dist < theMinDist)
		{
			theMinDist = dist;
			minIndex = i;
		}
	}
	
	theMinDist = 9999;
	int secondMinIndex = -1;
	for(int i = 0; i < 8; i++)
	{
		auto dist = info[i].getV3().distance(data.m_pos);
		if( dist < theMinDist && i != minIndex)
		{
			theMinDist = dist;
			secondMinIndex = i;
		}
	}
	float factor = data.m_pos.distance(info[minIndex].getV3()) / (data.m_pos.distance(info[secondMinIndex].getV3()));
	data.m_matIndex = vec3(info[minIndex].matIndex1, info[secondMinIndex].matIndex1, 1.0 - factor * 0.5);
	// printf("the factor %f\n",1.0 - factor);
	*/
}
void MarchingCubes::generateWithoutNormal(Mesh *mesh, int ncellsX, int ncellsY, int ncellsZ, voxelInfo *srcData, float minValue, int lodLevel)
{
    mesh->clear();
    int meshIndex = 0;
    int YtimeZ = (ncellsY+1)*(ncellsZ+1);
	voxelInfo verts[8];
	VoxelVertex intVerts[12];
	int lodStride = lodList[lodLevel];
    //go through all the points
    for(int i=0; i < ncellsX; i += lodStride)			//x axis
        for(int j=0; j < ncellsY; j += lodStride)		//y axis
            for(int k=0; k < ncellsZ; k += lodStride)	//z axis
            {
                //initialize vertices
                int ind = i*YtimeZ + j*(ncellsZ+1) + k;
                verts[0] = srcData[ind];
                verts[1] = srcData[ind + lodStride* (YtimeZ)];
                verts[2] = srcData[ind + lodStride* (YtimeZ + 1)];
                verts[3] = srcData[ind + lodStride * 1];
                verts[4] = srcData[ind + lodStride * (ncellsZ+1)];
                verts[5] = srcData[ind + lodStride * (YtimeZ + (ncellsZ+1))];
                verts[6] = srcData[ind + lodStride * (YtimeZ + (ncellsZ+1) + 1)];
                verts[7] = srcData[ind + lodStride * ((ncellsZ+1) + 1)];

                //get the index
                int cubeIndex = 0;
                for(int n=0; n < 8; n++)
                    /*(step 4)*/		if(verts[n].w <= minValue) cubeIndex |= (1 << n);

                //check if its completely inside or outside
                /*(step 5)*/ if(!edgeTable[cubeIndex]) continue;

                //get intersection vertices on edges and save into the array
                /*(step 6)*/ if(edgeTable[cubeIndex] & 1) intVerts[0] = LinearInterp(verts[0], verts[1], minValue);
                if(edgeTable[cubeIndex] & 2) intVerts[1] = LinearInterp(verts[1], verts[2], minValue);
                if(edgeTable[cubeIndex] & 4) intVerts[2] = LinearInterp(verts[2], verts[3], minValue);
                if(edgeTable[cubeIndex] & 8) intVerts[3] = LinearInterp(verts[3], verts[0], minValue);
                if(edgeTable[cubeIndex] & 16) intVerts[4] = LinearInterp(verts[4], verts[5], minValue);
                if(edgeTable[cubeIndex] & 32) intVerts[5] = LinearInterp(verts[5], verts[6], minValue);
                if(edgeTable[cubeIndex] & 64) intVerts[6] = LinearInterp(verts[6], verts[7], minValue);
                if(edgeTable[cubeIndex] & 128) intVerts[7] = LinearInterp(verts[7], verts[4], minValue);
                if(edgeTable[cubeIndex] & 256) intVerts[8] = LinearInterp(verts[0], verts[4], minValue);
                if(edgeTable[cubeIndex] & 512) intVerts[9] = LinearInterp(verts[1], verts[5], minValue);
                if(edgeTable[cubeIndex] & 1024) intVerts[10] = LinearInterp(verts[2], verts[6], minValue);
                if(edgeTable[cubeIndex] & 2048) intVerts[11] = LinearInterp(verts[3], verts[7], minValue);

                //now build the triangles using triTable
                for (int n=0; triTable[cubeIndex][n] != -1; n+=3)
                {
                    auto v0 = genVertexData(intVerts[triTable[cubeIndex][n+2]]);
                    auto v1 = genVertexData(intVerts[triTable[cubeIndex][n+1]]);
                    auto v2 = genVertexData(intVerts[triTable[cubeIndex][n]]);


                	//calculate the material index
                	calcMaterial(v0, verts, srcData);
                	calcMaterial(v1, verts, srcData);
                	calcMaterial(v2, verts, srcData);
                	
                    mesh->addVertex(v0);
                    mesh->addVertex(v1);
                    mesh->addVertex(v2);

                    mesh->addIndex(meshIndex + 2);
                    mesh->addIndex(meshIndex + 1);
                    mesh->addIndex(meshIndex);
                    meshIndex += 3;
                }
            }	//END OF FOR LOOP
}


MarchingCubes::MarchingCubes()
{

}

} // namespace tzw
