#include "InstancedMesh.h"
#include <iostream>
#include <assert.h>
#include "Utility/log/Log.h"

namespace tzw {

	InstancedMesh::InstancedMesh()
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

	void InstancedMesh::submitInstanced(int preserveNumber)
	{
		if (m_instanceOffset.size() > 0 || preserveNumber > 0)
		{
			m_instanceBuf->use();
			int size = (preserveNumber > 0) ?preserveNumber:m_instanceOffset.size();
			void * data = (preserveNumber > 0)?NULL:&m_instanceOffset[0];
			m_instanceBuf->allocate(data, size * sizeof(InstanceData));
		}
	}

	int InstancedMesh::getInstanceSize()
	{
		return m_instanceOffset.size();
	}

	void InstancedMesh::reSubmitInstanced()
	{
		if (m_instanceOffset.size() > 0)
		{
			m_instanceBuf->use();
			m_instanceBuf->resubmit(&m_instanceOffset[0], 0, m_instanceOffset.size() * sizeof(InstanceData));
		}
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

