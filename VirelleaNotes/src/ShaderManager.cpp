//============================================================================
//! @file   ShaderManager.cpp
//! @brief  ShaderManager クラスの実装
//============================================================================
#include "ShaderManager.h"

namespace Graphics {

ShaderManager& ShaderManager::GetInstance()
{
    static ShaderManager instance;
    return instance;
}

std::shared_ptr<Shader> ShaderManager::Load(const std::string& name, const std::string& vertexShaderPath, const std::string& pixelShaderPath)
{
    auto it = m_shaders.find(name);
    if(it != m_shaders.end()) {
        return it->second;    // 読み込み済みならそれを返す
    }

    auto shader = std::make_shared<Shader>();
    if(!shader->Load(vertexShaderPath, pixelShaderPath)) {
        return nullptr;    // 読み込み失敗（呼び出し側で nullptr チェックすること）
    }

    m_shaders[name] = shader;
    return shader;
}

std::shared_ptr<Shader> ShaderManager::Get(const std::string& name) const
{
    auto it = m_shaders.find(name);
    if(it == m_shaders.end()) return nullptr;
    return it->second;
}

void ShaderManager::ReleaseAll()
{
    m_shaders.clear();
}

}    // namespace Graphics
