#include "InstancedMesh.h"
#include <iostream>
#include <assert.h>
#include "Utility/log/Log.h"
#include <array>
namespace tzw {

	InstancedMesh::InstancedMesh()
	{
		m_instanceBuf = new RenderBuffer(RenderBuffer::Type::VERTEX);
		m_instanceBuf->create();
	}

	InstancedMesh::InstancedMesh(Mesh* mesh):m_mesh(mesh)
	{
		m_instanceBuf = new RenderBuffer(RenderBuffer::Type::VERTEX);
		m_instanceBuf->create();
	}

	RenderBuffer* InstancedMesh::getInstanceBuf() const
	{
		return m_instanceBuf;
	}

	void InstancedMesh::pushInstance(InstanceData instanceData)
	{
		m_instanceOffset.push_back(instanceData);
	}

	void InstancedMesh::pushInstances(std::vector<InstanceData> instancePos)
	{
		m_instanceOffset = instancePos;
	}

	void InstancedMesh::clearInstances()
	{
		m_instanceOffset.clear();
	}
	std::array<int, 4> g_GrowList{16, 32, 64, 128};
	void InstancedMesh::submitInstanced(int preserveNumber)
	{
		if(m_instanceOffset.size() <= 0) return;
		m_instanceBuf->use();
		size_t size = m_instanceOffset.size();
		void * data = &m_instanceOffset[0];
		if ( size > m_resverdSize)
		{
			bool isUsedFixedSize = false;
			for(auto growSize : g_GrowList)
			{
				if(size <growSize)
				{
					size = growSize;
					isUsedFixedSize = true;
					break;
				}
			}

			m_instanceBuf->allocate(data, size * sizeof(InstanceData));
			m_resverdSize = size;
		}
		else //already have enough space
		{
			m_instanceBuf->use();
			m_instanceBuf->copyData(data, size * sizeof(InstanceData));
		}
	}

	int InstancedMesh::getInstanceSize()
	{
		return m_instanceOffset.size();
	}

	InstancedMesh::~InstancedMesh()
	{
		delete m_instanceBuf;
	}

	Mesh* InstancedMesh::getMesh()
	{
		return m_mesh;
	}

	void InstancedMesh::setMesh(Mesh* mesh)
	{
		m_mesh = mesh;
	}

} // namespace tzw

