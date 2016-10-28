#include "MarchingCubes.h"

#include "MCTable.h"
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


vec3 LinearInterp(vec4 & p1, vec4 & p2, float value)
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
void MarchingCubes::generate(Mesh *mesh, int ncellsX, int ncellsY, int ncellsZ, vec4 *points, float minValue)
{
    mesh->clear();
    int meshIndex = 0;
    int pointsZ = ncellsZ+1;
    int YtimeZ = (ncellsY+1)*(ncellsZ+1);
    //go through all the points
    for(int i=0; i < ncellsX; i++)			//x axis
        for(int j=0; j < ncellsY; j++)		//y axis
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

                //get intersection vertices on edges and save into the array
                vec3 intVerts[12];
                int indGrad = 0;
                auto edgeIndex = edgeTable[cubeIndex];


                if(edgeIndex & 1) {
                    intVerts[0] = LinearInterp(verts[0], verts[1], minValue);
                    if(i != 0 && j != 0 && k != 0) gradVerts[0] = CALC_GRAD_VERT_0(verts)
                            else gradVerts[0] = vec4(1.0, 1.0, 1.0, 1.0);
                    if(i != lastX-1 && j != 0 && k != 0) gradVerts[1] = CALC_GRAD_VERT_1(verts)
                            else gradVerts[1] = vec4(1.0, 1.0, 1.0, 1.0);
                    indGrad |= 3;
                    grads[0] = LinearInterp(gradVerts[0], gradVerts[1], minValue);
                    grads[0].x *= factor.x; grads[0].y *= factor.y; grads[0].z *= factor.z;
                }
                if(edgeIndex & 2) {
                    intVerts[1] = LinearInterp(verts[1], verts[2], minValue);
                    if(! (indGrad & 2)) {
                        if(i != lastX-1 && j != 0 && k != 0) gradVerts[1] = CALC_GRAD_VERT_1(verts)
                                else gradVerts[1] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 2;
                    }
                    if(i != lastX-1 && j != 0 && k != 0) gradVerts[2] = CALC_GRAD_VERT_2(verts)
                            else gradVerts[2] = vec4(1.0, 1.0, 1.0, 1.0);
                    indGrad |= 4;
                    grads[1] = LinearInterp(gradVerts[1], gradVerts[2], minValue);
                    grads[1].x *= factor.x; grads[1].y *= factor.y; grads[1].z *= factor.z;
                }
                if(edgeIndex & 4) {
                    intVerts[2] = LinearInterp(verts[2], verts[3], minValue);
                    if(! (indGrad & 4)) {
                        if(i != lastX-1 && j != 0 && k != 0) gradVerts[2] = CALC_GRAD_VERT_2(verts)
                                else gradVerts[2] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 4;
                    }
                    if(i != 0 && j != 0 && k != lastZ-1) gradVerts[3] = CALC_GRAD_VERT_3(verts)
                            else gradVerts[3] = vec4(1.0, 1.0, 1.0, 1.0);
                    indGrad |= 8;
                    grads[2] = LinearInterp(gradVerts[2], gradVerts[3], minValue);
                    grads[2].x *= factor.x; grads[2].y *= factor.y; grads[2].z *= factor.z;
                }
                if(edgeIndex & 8) {
                    intVerts[3] = LinearInterp(verts[3], verts[0], minValue);
                    if(! (indGrad & 8)) {
                        if(i != 0 && j != 0 && k != lastZ-1) gradVerts[3] = CALC_GRAD_VERT_3(verts)
                                else gradVerts[3] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 8;
                    }
                    if(! (indGrad & 1)) {
                        if(i != 0 && j != 0 && k != 0) gradVerts[0] = CALC_GRAD_VERT_0(verts)
                                else gradVerts[0] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 1;
                    }
                    grads[3] = LinearInterp(gradVerts[3], gradVerts[0], minValue);
                    grads[3].x *= factor.x; grads[3].y *= factor.y; grads[3].z *= factor.z;
                }
                if(edgeIndex & 16) {
                    intVerts[4] = LinearInterp(verts[4], verts[5], minValue);

                    if(i != 0 && j != lastY-1 && k != 0) gradVerts[4] = CALC_GRAD_VERT_4(verts)
                            else gradVerts[4] = vec4(1.0, 1.0, 1.0, 1.0);

                    if(i != lastX-1 && j != lastY-1 && k != 0) gradVerts[5] = CALC_GRAD_VERT_5(verts)
                            else gradVerts[5] = vec4(1.0, 1.0, 1.0, 1.0);

                    indGrad |= 48;
                    grads[4] = LinearInterp(gradVerts[4], gradVerts[5], minValue);
                    grads[4].x *= factor.x; grads[4].y *= factor.y; grads[4].z *= factor.z;
                }
                if(edgeIndex & 32) {
                    intVerts[5] = LinearInterp(verts[5], verts[6], minValue);
                    if(! (indGrad & 32)) {
                        if(i != lastX-1 && j != lastY-1 && k != 0) gradVerts[5] = CALC_GRAD_VERT_5(verts)
                                else gradVerts[5] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 32;
                    }

                    if(i != lastX-1 && j != lastY-1 && k != lastZ-1) gradVerts[6] = CALC_GRAD_VERT_6(verts)
                            else gradVerts[6] = vec4(1.0, 1.0, 1.0, 1.0);
                    indGrad |= 64;
                    grads[5] = LinearInterp(gradVerts[5], gradVerts[6], minValue);
                    grads[5].x *= factor.x; grads[5].y *= factor.y; grads[5].z *= factor.z;
                }
                if(edgeIndex & 64) {
                    intVerts[6] = LinearInterp(verts[6], verts[7], minValue);
                    if(! (indGrad & 64)) {
                        if(i != lastX-1 && j != lastY-1 && k != lastZ-1) gradVerts[6] = CALC_GRAD_VERT_6(verts)
                                else gradVerts[6] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 64;
                    }

                    if(i != 0 && j != lastY-1 && k != lastZ-1) gradVerts[7] = CALC_GRAD_VERT_7(verts)
                            else gradVerts[7] = vec4(1.0, 1.0, 1.0, 1.0);
                    indGrad |= 128;
                    grads[6] = LinearInterp(gradVerts[6], gradVerts[7], minValue);
                    grads[6].x *= factor.x; grads[6].y *= factor.y; grads[6].z *= factor.z;
                }
                if(edgeIndex & 128) {
                    intVerts[7] = LinearInterp(verts[7], verts[4], minValue);
                    if(! (indGrad & 128)) {
                        if(i != 0 && j != lastY-1 && k != lastZ-1) gradVerts[7] = CALC_GRAD_VERT_7(verts)
                                else gradVerts[7] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 128;
                    }
                    if(! (indGrad & 16)) {
                        if(i != 0 && j != lastY-1 && k != 0) gradVerts[4] = CALC_GRAD_VERT_4(verts)
                                else gradVerts[4] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 16;
                    }
                    grads[7] = LinearInterp(gradVerts[7], gradVerts[4], minValue);
                    grads[7].x *= factor.x; grads[7].y *= factor.y; grads[7].z *= factor.z;
                }
                if(edgeIndex & 256) {
                    intVerts[8] = LinearInterp(verts[0], verts[4], minValue);
                    if(! (indGrad & 1)) {
                        if(i != 0 && j != 0 && k != 0) gradVerts[0] = CALC_GRAD_VERT_0(verts)
                                else gradVerts[0] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 1;
                    }
                    if(! (indGrad & 16)) {
                        if(i != 0 && j != lastY-1 && k != 0) gradVerts[4] = CALC_GRAD_VERT_4(verts)
                                else gradVerts[4] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 16;
                    }
                    grads[8] = LinearInterp(gradVerts[0], gradVerts[4], minValue);
                    grads[8].x *= factor.x; grads[8].y *= factor.y; grads[8].z *= factor.z;
                }
                if(edgeIndex & 512) {
                    intVerts[9] = LinearInterp(verts[1], verts[5], minValue);
                    if(! (indGrad & 2)) {
                        if(i != lastX-1 && j != 0 && k != 0) gradVerts[1] = CALC_GRAD_VERT_1(verts)
                                else gradVerts[1] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 2;
                    }
                    if(! (indGrad & 32)) {
                        if(i != lastX-1 && j != lastY-1 && k != 0) gradVerts[5] = CALC_GRAD_VERT_5(verts)
                                else gradVerts[5] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 32;
                    }
                    grads[9] = LinearInterp(gradVerts[1], gradVerts[5], minValue);
                    grads[9].x *= factor.x; grads[9].y *= factor.y; grads[9].z *= factor.z;
                }
                if(edgeIndex & 1024) {
                    intVerts[10] = LinearInterp(verts[2], verts[6], minValue);
                    if(! (indGrad & 4)) {
                        if(i != lastX-1 && j != 0 && k != 0) gradVerts[2] = CALC_GRAD_VERT_2(verts)
                                else gradVerts[5] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 4;
                    }
                    if(! (indGrad & 64)) {
                        if(i != lastX-1 && j != lastY-1 && k != lastZ-1) gradVerts[6] = CALC_GRAD_VERT_6(verts)
                                else gradVerts[6] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 64;
                    }
                    grads[10] = LinearInterp(gradVerts[2], gradVerts[6], minValue);
                    grads[10].x *= factor.x; grads[10].y *= factor.y; grads[10].z *= factor.z;
                }
                if(edgeIndex & 2048) {
                    intVerts[11] = LinearInterp(verts[3], verts[7], minValue);
                    if(! (indGrad & 8)) {
                        if(i != 0 && j != 0 && k != lastZ-1) gradVerts[3] = CALC_GRAD_VERT_3(verts)
                                else gradVerts[3] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 8;
                    }
                    if(! (indGrad & 128)) {
                        if(i != 0 && j != lastY-1 && k != lastZ-1) gradVerts[7] = CALC_GRAD_VERT_7(verts)
                                else gradVerts[7] = vec4(1.0, 1.0, 1.0, 1.0);
                        indGrad |= 128;
                    }
                    grads[11] = LinearInterp(gradVerts[3], gradVerts[7], minValue);
                    grads[11].x *= factor.x; grads[11].y *= factor.y; grads[11].z *= factor.z;
                }


                //                /*(step 6)*/ if(edgeTable[cubeIndex] & 1) intVerts[0] = LinearInterp(verts[0], verts[1], minValue);
                //                if(edgeTable[cubeIndex] & 2) intVerts[1] = LinearInterp(verts[1], verts[2], minValue);
                //                if(edgeTable[cubeIndex] & 4) intVerts[2] = LinearInterp(verts[2], verts[3], minValue);
                //                if(edgeTable[cubeIndex] & 8) intVerts[3] = LinearInterp(verts[3], verts[0], minValue);
                //                if(edgeTable[cubeIndex] & 16) intVerts[4] = LinearInterp(verts[4], verts[5], minValue);
                //                if(edgeTable[cubeIndex] & 32) intVerts[5] = LinearInterp(verts[5], verts[6], minValue);
                //                if(edgeTable[cubeIndex] & 64) intVerts[6] = LinearInterp(verts[6], verts[7], minValue);
                //                if(edgeTable[cubeIndex] & 128) intVerts[7] = LinearInterp(verts[7], verts[4], minValue);
                //                if(edgeTable[cubeIndex] & 256) intVerts[8] = LinearInterp(verts[0], verts[4], minValue);
                //                if(edgeTable[cubeIndex] & 512) intVerts[9] = LinearInterp(verts[1], verts[5], minValue);
                //                if(edgeTable[cubeIndex] & 1024) intVerts[10] = LinearInterp(verts[2], verts[6], minValue);
                //                if(edgeTable[cubeIndex] & 2048) intVerts[11] = LinearInterp(verts[3], verts[7], minValue);

                //now build the triangles using triTable
                for (int n=0; triTable[cubeIndex][n] != -1; n+=3)
                {
                    auto v0 = VertexData(intVerts[triTable[cubeIndex][n+2]]);
                    auto v1 = VertexData(intVerts[triTable[cubeIndex][n+1]]);
                    auto v2 = VertexData(intVerts[triTable[cubeIndex][n]]);
                    v0.m_normal = grads[triTable[cubeIndex][n+2]];
                    v1.m_normal = grads[triTable[cubeIndex][n+1]];
                    v2.m_normal = grads[triTable[cubeIndex][n]];
                    mesh->addVertex(v0);
                    mesh->addVertex(v1);
                    mesh->addVertex(v2);

                    mesh->addIndex(meshIndex + 2);
                    mesh->addIndex(meshIndex + 1);
                    mesh->addIndex(meshIndex);
                    meshIndex += 3;
                }
            }	//END OF FOR LOOP
    //    mesh->caclNormals();
    //    mesh->finish();
}

void MarchingCubes::generateWithoutNormal(Mesh *mesh, int ncellsX, int ncellsY, int ncellsZ, vec4 *srcData, float minValue)
{
    mesh->clear();
    int meshIndex = 0;
    int YtimeZ = (ncellsY+1)*(ncellsZ+1);
    //go through all the points
    for(int i=0; i < ncellsX; i++)			//x axis
        for(int j=0; j < ncellsY; j++)		//y axis
            for(int k=0; k < ncellsZ; k++)	//z axis
            {
                //initialize vertices
                vec4 verts[8];
                int ind = i*YtimeZ + j*(ncellsZ+1) + k;
                verts[0] = srcData[ind];
                verts[1] = srcData[ind + YtimeZ];
                verts[2] = srcData[ind + YtimeZ + 1];
                verts[3] = srcData[ind + 1];
                verts[4] = srcData[ind + (ncellsZ+1)];
                verts[5] = srcData[ind + YtimeZ + (ncellsZ+1)];
                verts[6] = srcData[ind + YtimeZ + (ncellsZ+1) + 1];
                verts[7] = srcData[ind + (ncellsZ+1) + 1];

                //get the index
                int cubeIndex = int(0);
                for(int n=0; n < 8; n++)
                    /*(step 4)*/		if(verts[n].w <= minValue) cubeIndex |= (1 << n);

                //check if its completely inside or outside
                /*(step 5)*/ if(!edgeTable[cubeIndex]) continue;

                //get intersection vertices on edges and save into the array
                vec3 intVerts[12];

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
                    auto v0 = VertexData(intVerts[triTable[cubeIndex][n+2]]);
                    auto v1 = VertexData(intVerts[triTable[cubeIndex][n+1]]);
                    auto v2 = VertexData(intVerts[triTable[cubeIndex][n]]);
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
