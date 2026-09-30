//============================================================================
//! @file   RenderPipeline.cpp
//! @brief  RenderPipeline クラスの実装
//============================================================================
#include "RenderPipeline.h"

namespace Graphics {

void RenderPipeline::AddPass(std::unique_ptr<RenderPass> pass)
{
    m_passes.push_back(std::move(pass));
}

void RenderPipeline::Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera)
{
    for(auto& pass : m_passes) {
        pass->Execute(objects, camera);
    }
}

void RenderPipeline::Clear()
{
    m_passes.clear();
}

}    // namespace Graphics
