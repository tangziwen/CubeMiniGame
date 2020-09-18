#include "RenderPath.h"
namespace tzw
{

    RenderPath::RenderPath()
    {
        m_stages.reserve(10);
    }

    void RenderPath::prepare()
    {
        m_stages.clear();
    }

    void RenderPath::addRenderStage(RenderStage* stage)
    {
        m_stages.emplace_back(stage);
    }

    std::vector<RenderStage *> & RenderPath::getStages()
    {
    
        return m_stages;
    }
}