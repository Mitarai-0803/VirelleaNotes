//============================================================================
//! @file   PostProcessParams.cpp
//! @brief  PostProcessParams クラスの実装
//! @details DxLib 公式の定数バッファ API（CreateShaderConstantBuffer 系）を使う。
//!          生の D3D11 API で PSSetConstantBuffers するより、DxLib の内部状態と
//!          衝突しないため確実に届く。
//============================================================================
#include "PostProcessParams.h"
#include <DxLib.h>
#include <cstring>

namespace Graphics {

PostProcessParams& PostProcessParams::GetInstance()
{
    static PostProcessParams instance;
    return instance;
}

bool PostProcessParams::CreateConstantBuffer()
{
    if(m_constantBufferHandle != -1)
        return true;

    m_constantBufferHandle = CreateShaderConstantBuffer(sizeof(PostProcessConstants));
    return m_constantBufferHandle != -1;
}

void PostProcessParams::UpdateAndBind() const
{
    if(m_constantBufferHandle == -1)
        return;

    void* dst = GetBufferShaderConstantBuffer(m_constantBufferHandle);
    if(!dst)
        return;

    std::memcpy(dst, &m_constants, sizeof(PostProcessConstants));
    UpdateShaderConstantBuffer(m_constantBufferHandle);
    SetShaderConstantBuffer(m_constantBufferHandle, DX_SHADERTYPE_PIXEL, POSTPROCESS_CBUFFER_SLOT);    // register(b4) と一致させる
}

void PostProcessParams::Release()
{
    if(m_constantBufferHandle != -1) {
        DeleteShaderConstantBuffer(m_constantBufferHandle);
        m_constantBufferHandle = -1;
    }
}

}    // namespace Graphics
