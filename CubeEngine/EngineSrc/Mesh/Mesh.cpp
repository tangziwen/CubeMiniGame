#include "Mesh.h"
#include <iostream>
#include <assert.h>
#include "Utility/log/Log.h"

namespace tzw {

Mesh::Mesh()
	: m_matIndex(0), m_vbo(0), m_ibo(0)
{
	m_arrayBuf = new RenderBuffer(RenderBuffer::Type::VERTEX);
	m_indexBuf = new RenderBuffer(RenderBuffer::Type::INDEX);
	m_instanceBuf = new RenderBuffer(RenderBuffer::Type::VERTEX);
}

void Mesh::addIndex(unsigned int index)
{
    m_indices.push_back(index);
}

void Mesh::addIndices(unsigned short *index, int size)
{
    for(int i = 0; i<size; i++)
    {
        addIndex(index[i]);
    }
}

void Mesh::addVertex(VertexData vertexData)
{
    m_vertices.push_back(vertexData);
}

void Mesh::addVertices(VertexData *vertices, int size)
{
    for(int i = 0; i<size; i++)
    {
        addVertex(vertices[i]);
    }
}

void Mesh::finish(bool isPassToGPU)
{
	calcTangents();
    calculateAABB();
    if(isPassToGPU)
    {
        submit();
    }
}

void Mesh::submit(RenderFlag::BufferStorageType storageType)
{
	if (m_vertices.empty()) return;
    if(m_ibo == 0)
    {
        createBufferObject();
    }
    //pass data to the VBO
    m_arrayBuf->use();
    m_arrayBuf->allocate(&m_vertices[0], m_vertices.size() * sizeof(VertexData), storageType);

    //pass data to the IBO
    m_indexBuf->use();
    m_indexBuf->allocate(&m_indices[0], m_indices.size() * sizeof(short_u), storageType);

	if (m_instanceOffset.size() > 0)
	{
		m_instanceBuf->use();
		m_instanceBuf->allocate(&m_instanceOffset[0], m_instanceOffset.size() * sizeof(InstanceData), storageType);
	}
}

unsigned int Mesh::getMatIndex() const
{
    return m_matIndex;
}

unsigned int Mesh::getIndex(int id)
{
	return m_indices[id];
}

int Mesh::getInstanceSize()
{
	return m_instanceOffset.size();
}

bool Mesh::isEmpty()
{
	return m_indices.empty();
}

void Mesh::pushInstance(InstanceData instancePos)
{
	m_instanceOffset.push_back(instancePos);
}

void Mesh::pushInstances(std::vector<InstanceData> instancePos)
{
	m_instanceOffset = instancePos;
}

void Mesh::clearInstances()
{
	m_instanceOffset.clear();
}

void Mesh::calcTangents()
{
    size_t indexCount = m_indices.size();
    // Accumulate each triangle normal into each of the triangle vertices
    for (unsigned int i = 0 ; i < indexCount ; i += 3) {
        short_u index0 = m_indices[i];
        short_u index1 = m_indices[i + 1];
        short_u index2 = m_indices[i + 2];


	    auto& v0 = m_vertices[index0];
	    auto& v1 = m_vertices[index1];
	    auto& v2 = m_vertices[index2];
   
	    vec3 Edge1 = v1.m_pos - v0.m_pos;
	    vec3 Edge2 = v2.m_pos - v0.m_pos;

	    float DeltaU1 = v1.m_texCoord.x - v0.m_texCoord.x;
	    float DeltaV1 = v1.m_texCoord.y - v0.m_texCoord.y;
	    float DeltaU2 = v2.m_texCoord.x - v0.m_texCoord.x;
	    float DeltaV2 = v2.m_texCoord.y - v0.m_texCoord.y;

	    float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

	    vec3 Tangent, Bitangent;

	    Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
	    Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
	    Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

	    Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
	    Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
	    Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);

	    v0.m_tangent += Tangent;
	    v1.m_tangent += Tangent;
	    v2.m_tangent += Tangent;
    }

    size_t vertexCount = m_vertices.size();
    // Normalize all the vertex normals
    for (unsigned int i = 0 ; i < vertexCount ; i++) {
        m_vertices[i].m_tangent.normalize();
    }
}

void Mesh::submitInstanced(int preserveNumber)
{
	if (m_instanceOffset.size() > 0 || preserveNumber > 0)
	{
		m_instanceBuf->use();
		int size = (preserveNumber > 0) ?preserveNumber:m_instanceOffset.size();
		void * data = (preserveNumber > 0)?NULL:&m_instanceOffset[0];
		m_instanceBuf->allocate(data, size * sizeof(InstanceData));
	}
}

void Mesh::reSubmitInstanced()
{
	if (m_instanceOffset.size() > 0)
	{
		m_instanceBuf->use();
		m_instanceBuf->resubmit(&m_instanceOffset[0], 0, m_instanceOffset.size() * sizeof(InstanceData));
	}
}

void Mesh::reSubmit()
{
	if(m_arrayBuf && !m_vertices.empty())
	{
		m_arrayBuf->use();
		m_arrayBuf->resubmit(&m_vertices[0], 0, m_vertices.size() * sizeof(VertexData));


	    m_indexBuf->use();
	    m_indexBuf->resubmit(&m_indices[0], 0, m_indices.size() * sizeof(short_u));
	}
}

void Mesh::submitOnlyVO_IO()
{
	if (m_vertices.empty()) return;
    if(m_ibo == 0)
    {
        createBufferObject();
    }
    //pass data to the VBO
    m_arrayBuf->use();
    m_arrayBuf->allocate(&m_vertices[0], m_vertices.size() * sizeof(VertexData));

    //pass data to the IBO
    m_indexBuf->use();
    m_indexBuf->allocate(&m_indices[0], m_indices.size() * sizeof(short_u));
}

bool Mesh::intersectWithRay(const Ray& rayInMeshSpace, vec3 * hitPoint)
{
	float hitDist;
	for(int i = 0; i < m_indices.size(); i+=3)
	{
		if(rayInMeshSpace.intersectTriangle(m_vertices[i].m_pos, m_vertices[i + 1].m_pos, m_vertices[i + 2].m_pos, &hitDist))
		{
			if(hitPoint)
			{
				(*hitPoint) = rayInMeshSpace.origin() + rayInMeshSpace.direction() * hitDist;
			}
			return true;
		}
	}
	return false;
}

void Mesh::setMatIndex(unsigned int matIndex)
{
    m_matIndex = matIndex;
}

void Mesh::caclNormals()
{
    size_t indexCount = m_indices.size();
    // Accumulate each triangle normal into each of the triangle vertices
    for (unsigned int i = 0 ; i < indexCount ; i += 3) {
        short_u index0 = m_indices[i];
        short_u index1 = m_indices[i + 1];
        short_u index2 = m_indices[i + 2];
        vec3 v1 = m_vertices[index1].m_pos - m_vertices[index0].m_pos;
        vec3 v2 = m_vertices[index2].m_pos - m_vertices[index0].m_pos;
        vec3 normal = vec3::CrossProduct(v1,v2);
        normal.normalize();

        m_vertices[index0].m_normal += normal;
        m_vertices[index1].m_normal += normal;
        m_vertices[index2].m_normal += normal;
    }

    size_t vertexCount = m_vertices.size();
    // Normalize all the vertex normals
    for (unsigned int i = 0 ; i < vertexCount ; i++) {
        m_vertices[i].m_normal.normalize();
    }
}

void Mesh::calBaryCentric()
{
    size_t indexCount = m_indices.size();
    // Accumulate each triangle normal into each of the triangle vertices
    for (unsigned int i = 0 ; i < indexCount ; i += 3) {
        short_u index0 = m_indices[i];
        short_u index1 = m_indices[i + 1];
        short_u index2 = m_indices[i + 2];

        m_vertices[index0].m_barycentric += vec3(1.0, 0.0, 0.0);
        m_vertices[index1].m_barycentric += vec3(0.0, 1.0, 0.0);
        m_vertices[index2].m_barycentric += vec3(0.0, 0.0, 1.0);
    }

    size_t vertexCount = m_vertices.size();
    // Normalize all the vertex normals
    for (unsigned int i = 0 ; i < vertexCount ; i++) {
        m_vertices[i].m_barycentric.normalize();
    }
}

VertexData Mesh::getVertex(unsigned int index)
{
    return m_vertices[index];
}

void Mesh::setVertex(unsigned int index, VertexData vertex)
{
    m_vertices[index] = vertex;
}

void Mesh::subDivide(int level)
{
    triangles_.clear();
    newverts_.clear();
    midTriangles_.clear();
    for (size_t i = 0; i < m_indices.size(); i += 3)
    {
        Triangle t;
        t.v[0] = m_indices[i];
        t.v[2] = m_indices[i + 1];
        t.v[1] = m_indices[i + 2];
        triangles_.push_back(t);
    }
    findAdjTriangles();
    for(int i = 0; i < level; i++)
    {
        subDivideIter();
    }
}

void Mesh::cloneFrom(Mesh *other)
{
    m_vertices = other->m_vertices;
    m_indices = other->m_indices;
}

void Mesh::triangleSplit(int index, int callee, int newPoint, int t1, int t2)
{
    if ( processed_[index] )
        return;

    int sz;
    int midPoints[3];
    int indices[3];
    bool process[3];

    const Triangle& cur = triangles_[index];
    Triangle tri[4];
    int off = tmpTriangles_.size ();

    if ( callee < 0 ) {
        sz = 3;
        indices[0] =0; indices[1] =1; indices[2] = 2;
    }
    else
    {
        sz = 2;
        int update = cur.findNeighborIndex ( callee );

        if ( update < 0 )
        {
            std::cerr << "ObjFile::triangleSplit: semantic error !\n";
        }

        midPoints[update] = newPoint;
        midPoints_[3*index + update] = newPoint;

        tri[ update ].t[0] = t1;
        tri[ (update+1)% 3].t[2] = t2;

        tmpTriangles_[t1].t[2] = off + update;
        tmpTriangles_[t2].t[0] = off + (update+1) % 3;

        if ( update == 0 ) {
            indices[0] = 1; indices[1] = 2;
        } else if ( update == 1 ) {
            indices[0] = 0; indices[1] = 2;
        } else {
            indices[0] = 0; indices[1] = 1;
        }
    }

    // process the other ones
    for ( int i =0; i < sz; ++i )
    {
        // check whether already processed ?
        // get the neighbour of current edge
        int ti = cur.t[indices[i]];
        Triangle& t = triangles_[ti];
        int tv = 3*ti + t.findNeighborIndex (index);
        int tvnext = 3*ti + (t.findNeighborIndex (index)+1)%3;

        if ( midPoints_[tv] >= 0 ) {
            midPoints[indices[i]] = midPoints_[3*index + indices[i] ] = midPoints_ [tv];
            process[i] = false;
            // update triangle connectivity

            if(midTriangles_[tv] == -1)
            {
                tlogError("assert A\n");
            }
            //assert ( midTriangles_[tvnext] != -1 );

            tri[indices[i]].t[0] = midTriangles_[tvnext];
            tri[(indices[i]+1)%3].t[2] = midTriangles_[tv];

            tmpTriangles_[ midTriangles_[tvnext] ].t[2] = off + indices[i];
            tmpTriangles_[ midTriangles_[tv] ].t[0] = off + (indices[i]+1)%3;

        }
        else {
            //insert new one
            int k = indices[i];
            int knext = (k+1) % 3;
            int kprev = (k+2) % 3;
            int opposite = (t.findNeighborIndex (index) + 2) % 3;

            vec3 p = ( m_vertices[ cur.v[k] ].m_pos + m_vertices [ cur.v[knext] ].m_pos ) * (3.0f / 8.0f) +
                    ( m_vertices[ cur.v[kprev] ].m_pos + m_vertices[ t.v[ opposite ]].m_pos ) * (1.0f / 8.0f);
            auto v = VertexData(p);
            m_vertices.push_back ( v );
            midPoints[indices[i]] = midPoints_[3*index + indices[i] ] = m_vertices.size() -1;
            process[i] = true;
        }
    }

    // add subdivided triangles to new list
    //					2
    //					/\
    //			       /  \
    //				  /  2 \
    //               /      \
    //			    *--------*
    //             /  \  3 /  \
    //            /  0 \  /  1 \
    //           *------**------*
    //           0               1

    assert ( midPoints[0] != -1 );
    assert ( midPoints[1] != -1 );
    assert ( midPoints[2] != -1 );

    tri[0].v[0] = cur.v[0];
    tri[0].v[1] = midPoints[0];
    tri[0].v[2] = midPoints[2];
    tri[0].t[1] = off +3;

    tri[3].v[0] = midPoints[2];
    tri[3].v[1] = midPoints[0];
    tri[3].v[2] = midPoints[1];
    tri[3].t[0] = off;
    tri[3].t[1] = off +1;
    tri[3].t[2] = off +2;

    tri[1].v[0] = cur.v[1];
    tri[1].v[1] = midPoints[1];
    tri[1].v[2] = midPoints[0];
    tri[1].t[1] = off +3;

    tri[2].v[0] = cur.v[2];
    tri[2].v[1] = midPoints[2];
    tri[2].v[2] = midPoints[1];
    tri[2].t[1] = off +3;
    //	vertexTriangle_[cur.v[0]] = vertexTriangle_[midPoints[0]] = off;
    //	vertexTriangle_[cur.v[1]] = vertexTriangle_[midPoints[1]] = off+1;
    //	vertexTriangle_[cur.v[2]] = vertexTriangle_[midPoints[2]] = off+2;

    midTriangles_[3*index ] = off;
    midTriangles_[3*index+1] = off+1;
    midTriangles_[3*index+2] = off+2;

    for ( int i =0; i < 4; ++ i)
        tmpTriangles_.push_back (tri[i]);

    processed_[index] = true;

    // call the neigbor triangles
    for ( int i =0; i < sz; ++i ) {
        if ( process[i] )
        {
            splits_.push_back ( SplitData ( cur.t[indices[i]], index, midPoints[indices[i]],
                    off + (indices[i]+1)%3, off + indices[i] ) );
        }
    }
}

void Mesh::computeNewVerts()
{
    std::vector<int> ks ( newverts_.size (), 0 );
    for ( std::vector<Triangle>::const_iterator t = triangles_.begin(); t != triangles_.end (); ++t )
    {
        for ( int i =0; i < 3; ++i )
        {
            // all vertices are summed double
            auto p = newverts_[ t->v[i] ].m_pos + m_vertices[ t->v[(i+1)%3] ].m_pos + m_vertices[ t->v[(i+2)%3] ].m_pos;
            newverts_[ t->v[i] ] = VertexData(p);
            ++ks[t->v[i]];
        }
    }
    for ( auto nv = newverts_.begin (); nv != newverts_.end(); ++nv)
    {
        int num = ks[ nv-newverts_.begin()];
        float beta = 3.0f / (8.0f * (float)num);
        if ( num == 3 )
            beta = 3.0f / 16.0f;

        //float interm = (3.0/8.0 - 0.25 * cos ( 2 * M_PI / num ));
        //float beta = 1.0/num*(5.0/8.0 - interm*interm);
        auto tmpV = *nv;
        *nv = (tmpV.m_pos * (0.5f * beta)) + m_vertices[nv-newverts_.begin()].m_pos * ( 1 - beta*num);
    }
}

void Mesh::findAdjTriangles()
{
    for ( auto t = triangles_.begin(); t != triangles_.end(); ++t )
        for ( int e =0; e< 3; ++e )
            t->t[e] = -1;

    for ( auto t = triangles_.begin(); t != triangles_.end(); ++t )
    {
        // fill adj triangles
        for ( int e =0; e< 3; ++e ) {
            int enext = (e+1) % 3;

            if ( t->t[e] == -1 )
            {
                for ( auto t2 = t+1; t2 != triangles_.end(); ++t2 )
                {
                    int index;
                    if ( (  index = t2->containsEdge ( t->v[enext], t->v[e] )) >= 0 )
                    {
                        t->t[e] = t2 - triangles_.begin();
                        t2->t[index] = t - triangles_.begin();
                    }
                }
            }
        }
    }
}

void Mesh::subDivideIter()
{
    midPoints_ = std::vector<int> ( triangles_.size()*3, -1) ;
    midTriangles_ = std::vector<int> ( triangles_.size()*3, -1) ;
    tmpTriangles_.clear ();
    processed_ = std::vector<bool>(triangles_.size (), false);

    int oldVertSize = m_vertices.size ();

    // use list to avoid stack overflow problem
    splits_.clear ();

    //updates the tmpTriangles_ and vertexTriangle_ structure

    splits_.push_back ( SplitData (0,-1,-1,-1,-1) );
    while ( ! splits_.empty () )
    {
        SplitData sd = splits_.back ();
        splits_.pop_back();
        triangleSplit( sd.index, sd.callee, sd.newPoint, sd.t1, sd.t2 );
    }

    newverts_ = std::vector<VertexData>(oldVertSize);
    computeNewVerts ();

    std::copy ( newverts_.begin(), newverts_.end(), m_vertices.begin () );
    triangles_.swap ( tmpTriangles_ );
    m_indices.clear();
    for (auto triangle : triangles_)
    {
        m_indices.push_back(triangle.v[0]);
        m_indices.push_back(triangle.v[2]);
        m_indices.push_back(triangle.v[1]);
    }
}

integer_u Mesh::vbo() const
{
    return m_vbo;
}

void Mesh::setVbo(const integer_u &vbo)
{
    m_vbo = vbo;
}
integer_u Mesh::ibo() const
{
    return m_ibo;
}

void Mesh::setIbo(const integer_u &ibo)
{
    m_ibo = ibo;
}

size_t Mesh::getIndicesSize()
{
    return m_indices.size();
}

size_t Mesh::getVerticesSize()
{
    return m_vertices.size();
}

RenderBuffer *Mesh::getArrayBuf() const
{
    return m_arrayBuf;
}
RenderBuffer *Mesh::getIndexBuf() const
{
    return m_indexBuf;
}

RenderBuffer * Mesh::getInstanceBuf() const
{
	return m_instanceBuf;
}

void Mesh::clearVertices()
{
    m_vertices.clear();
	m_vertices.shrink_to_fit();
}

void Mesh::clearIndices()
{
    m_indices.clear();
	m_indices.shrink_to_fit();
}
AABB Mesh::getAabb() const
{
    return m_aabb;
}

void Mesh::setAabb(const AABB &aabb)
{
    m_aabb = aabb;
}

void Mesh::calculateAABB()
{
    for(size_t i=0;i<m_vertices.size();i++)
    {
        m_aabb.update(m_vertices[i].m_pos);
    }
}

void Mesh::merge(Mesh *other, const Matrix44 &transform)
{
    auto vOffset = m_vertices.size();
    //merge vertex
    for(auto vertext : other->m_vertices)
    {
        //transform by matrix
        auto pos = vec4(vertext.m_pos.x,vertext.m_pos.y,vertext.m_pos.z,1.0);
        auto resultPos = transform * pos;
        vertext.m_pos = vec3(resultPos.x,resultPos.y,resultPos.z);

        m_vertices.push_back(vertext);
    }

    //merge indices
    for(auto index : other->m_indices)
    {
        m_indices.push_back(index+vOffset);
    }
}

void Mesh::merge(std::vector<VertexData> &vertices, std::vector<short_u> &indices, const Matrix44 &transform)
{
    auto vOffset = m_vertices.size();
    //merge vertex
    for(auto vertext : vertices)
    {
        //transform by matrix
        auto pos = vec4(vertext.m_pos.x,vertext.m_pos.y,vertext.m_pos.z,1.0);
        auto resultPos = transform * pos;
        vertext.m_pos = vec3(resultPos.x,resultPos.y,resultPos.z);

        m_vertices.push_back(vertext);
    }

    //merge indices
    for(auto index : indices)
    {
        m_indices.push_back(index+vOffset);
    }
}

void Mesh::createBufferObject()
{
    //create vbo and ibo buffer
    m_arrayBuf->create();
    m_indexBuf->create();
	m_instanceBuf->create();

    //record the handle
    m_ibo = m_indexBuf->bufferId();
    m_vbo = m_arrayBuf->bufferId();
}

void Mesh::clear()
{
    this->clearIndices();
    this->clearVertices();
}

Triangle::Triangle ()
{
    for ( int i =0; i < 3; ++i )
    {
        v[i] = n[i] = t[i] = -1; // invalid start value
    }
}

int Triangle::containsEdge ( int n1, int n2 ) const
{
    for ( int i =0; i < 3; ++i )
    {
        int inext = (i+1) % 3;
        if ( v[i] == n1 && v[inext] == n2 )
            return i;

    }

    return -1;
}

int Triangle::findNeighborIndex (int n ) const
{
    for ( int i =0; i < 3; ++i )
    {
        if ( t[i] == n ) return i;
    }

    // not found
    return -1;
}

int Triangle::getVertexIndex (int n) const
{
    for ( int i =0; i < 3; ++i )
    {
        if ( v[i] == n ) return i;
    }

    // not found
    return -1;
}





} // namespace tzw

