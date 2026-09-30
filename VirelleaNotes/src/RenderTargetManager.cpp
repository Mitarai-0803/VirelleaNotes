//============================================================================
//! @file   RenderTargetManager.cpp
//! @brief  レンダーターゲット管理クラスの実装
//! @details DxLib のスクリーンハンドルを用いて名前付きレンダーターゲットを管理する
//! @author レオ
//============================================================================
#include "RenderTargetManager.h"
#include <DxLib.h>
#include <unordered_map>
#include <string>
#include <memory>

namespace Graphics
{

//! @brief Impl 構造体（実装詳細）
struct RenderTargetManager::Impl
{
    std::unordered_map<std::string, int> handles;    //!< 管理名 -> グラフィックハンドル
    std::string                          activeName; //!< 現在アクティブなターゲット名
};

RenderTargetManager& RenderTargetManager::GetInstance()
{
    static RenderTargetManager instance;
    return instance;
}

RenderTargetManager::RenderTargetManager()
    : m_impl(std::make_unique<Impl>())
{
}

RenderTargetManager::~RenderTargetManager()
{
    ReleaseAll();
}

int RenderTargetManager::CreateRenderTarget(const std::string& name, int width, int height, bool useAlpha)
{
    if(m_impl->handles.find(name) != m_impl->handles.end()) return m_impl->handles[name];

    int handle = MakeScreen(width, height, useAlpha ? TRUE : FALSE);
    if(handle == -1) return -1;

    m_impl->handles[name] = handle;
    return handle;
}

bool RenderTargetManager::SetActive(const std::string& name)
{
    auto it = m_impl->handles.find(name);
    if(it == m_impl->handles.end()) return false;

    SetDrawScreen(it->second);
    m_impl->activeName = name;
    return true;
}

void RenderTargetManager::ResetToScreen()
{
    SetDrawScreen(DX_SCREEN_BACK);
    m_impl->activeName.clear();
}

void RenderTargetManager::Present(const std::string& name, int x, int y)
{
    auto it = m_impl->handles.find(name);
    if(it == m_impl->handles.end()) return;

    int h = it->second;
    DrawGraph(x, y, h, TRUE);
}

void RenderTargetManager::Release(const std::string& name)
{
    auto it = m_impl->handles.find(name);
    if(it == m_impl->handles.end()) return;

    DeleteGraph(it->second);
    m_impl->handles.erase(it);
}

void RenderTargetManager::ReleaseAll()
{
    for(auto& kv : m_impl->handles) {
        DeleteGraph(kv.second);
    }
    m_impl->handles.clear();
}

int RenderTargetManager::GetHandle(const std::string& name) const
{
    auto it = m_impl->handles.find(name);
    if(it == m_impl->handles.end()) return -1;
    return it->second;
}

}    // namespace Graphics
