//============================================================================
//! @file   ShadowMapManager.cpp
//! @brief  ShadowMapManager クラスの実装
//============================================================================
#include "ShadowMapManager.h"

namespace Graphics {

ShadowMapManager& ShadowMapManager::GetInstance()
{
    static ShadowMapManager instance;
    return instance;
}

bool ShadowMapManager::Init(int sizeX, int sizeY)
{
    if(m_handle != -1) {
        return true;    // 既に作成済みなら何もしない（二重作成防止）
    }

    m_handle = MakeShadowMap(sizeX, sizeY);
    return m_handle != -1;
}

void ShadowMapManager::SetLightDirection(const VECTOR& direction)
{
    if(m_handle == -1)
        return;

    SetShadowMapLightDirection(m_handle, direction);
}

void ShadowMapManager::SetDrawArea(const VECTOR& minPosition, const VECTOR& maxPosition)
{
    if(m_handle == -1)
        return;

    SetShadowMapDrawArea(m_handle, minPosition, maxPosition);
}

void ShadowMapManager::BeginRecord()
{
    if(m_handle == -1)
        return;

    ShadowMap_DrawSetup(m_handle);
    m_recording = true;
}

void ShadowMapManager::EndRecord()
{
    if(m_handle == -1)
        return;

    ShadowMap_DrawEnd();
    m_recording = false;
}

void ShadowMapManager::BeginUse(int slot)
{
    if(m_handle == -1)
        return;

    SetUseShadowMap(slot, m_handle);
}

void ShadowMapManager::EndUse(int slot)
{
    SetUseShadowMap(slot, -1);
}

void ShadowMapManager::End()
{
    if(m_handle == -1)
        return;    // 未作成、または既に削除済みなら何もしない（多重削除防止）

    DeleteShadowMap(m_handle);
    m_handle = -1;
}

}    // namespace Graphics
