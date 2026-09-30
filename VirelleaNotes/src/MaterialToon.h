#pragma once
//============================================================================
// MaterialToon.h
// アニメ調(セルルック)シェーディング用マテリアルパラメータの保持とGPU転送。
// Toon_PS.fx の cbuffer cbUserToonMaterial (register b5) へ
// 影の量子化・スペキュラ・リムライトのパラメータを転送する。
//
// DxLibのb0～b3を避けたb5を使い、
// Shader::Bind() 直後に UpdateAndBind() を呼ぶ運用を想定している。
//============================================================================
#include "ConstantsEngine.h"
#include <DxLib.h>
#include <cstring>

namespace Graphics {

//-----------------------------------------------------------
// Toon_PS.fx の cbUserToonMaterial と1:1でメモリレイアウトを合わせること
// (float4個 = 16byte境界。並び順を変える場合はシェーダー側も合わせて変更)
//-----------------------------------------------------------
struct ToonMaterialConstants
{
    float ShadowThreshold   = 0.5f;    // 影が始まるNdotLの位置
    float ShadowSoftness    = 0.03f;   // 影境界のぼかし幅(0に近いほどくっきり)
    float SpecularThreshold = 0.97f;   // ハイライトの広さ
    float SpecularSoftness  = 0.02f;   // ハイライト境界のぼかし幅

    float ShadowColorR = 0.55f;        // 影の着色(少し青紫がかった色をデフォルトに)
    float ShadowColorG = 0.55f;
    float ShadowColorB = 0.75f;
    float RimPower     = 3.0f;         // リムライトの鋭さ

    float RimColorR = 1.0f;            // リムライトの色
    float RimColorG = 1.0f;
    float RimColorB = 1.0f;
    float UseAlbedoTexture = 0.0f;     // 0.0f/1.0f

    float SpecularColorR = 1.0f;       // ハイライトの色
    float SpecularColorG = 1.0f;
    float SpecularColorB = 1.0f;
    float RimIntensity   = 0.6f;       // リムライトの強さ(0で無効化)
};

//-----------------------------------------------------------
//! @class MaterialToon
//! @brief 既存 Material にトゥーンシェーディングのパラメータ保持とGPU転送を足したもの
//! @details 使い方:
//!   1. 起動時に一度だけ CreateConstantBuffer() を呼ぶ
//!   2. 毎フレーム、Shader::Bind() の直後に UpdateAndBind() を呼ぶ
//-----------------------------------------------------------
class MaterialToon
{
public:
    void SetShadowThreshold(float v)   { m_constants.ShadowThreshold = v; }
    void SetShadowSoftness(float v)    { m_constants.ShadowSoftness = v; }
    void SetSpecularThreshold(float v) { m_constants.SpecularThreshold = v; }
    void SetSpecularSoftness(float v)  { m_constants.SpecularSoftness = v; }
    void SetRimPower(float v)          { m_constants.RimPower = v; }
    void SetRimIntensity(float v)      { m_constants.RimIntensity = v; }
    void SetUseAlbedoTexture(bool v)   { m_constants.UseAlbedoTexture = v ? 1.0f : 0.0f; }

    void SetShadowColor(float r, float g, float b)
    {
        m_constants.ShadowColorR = r;
        m_constants.ShadowColorG = g;
        m_constants.ShadowColorB = b;
    }

    void SetRimColor(float r, float g, float b)
    {
        m_constants.RimColorR = r;
        m_constants.RimColorG = g;
        m_constants.RimColorB = b;
    }

    void SetSpecularColor(float r, float g, float b)
    {
        m_constants.SpecularColorR = r;
        m_constants.SpecularColorG = g;
        m_constants.SpecularColorB = b;
    }

    //-----------------------------------------------------------
    //! @brief b5 用のコンスタントバッファをGPU上に一度だけ作成する
    //! @return true 成功
    //-----------------------------------------------------------
    bool CreateConstantBuffer()
    {
        if(m_constantBufferHandle != -1) return true;
        m_constantBufferHandle = CreateShaderConstantBuffer(sizeof(ToonMaterialConstants));    // 16の倍数(64byte)
        return m_constantBufferHandle != -1;
    }

    //-----------------------------------------------------------
    //! @brief 現在の値をGPUへ転送し、PS の register(b5) にバインドする
    //! @details Shader::Bind() の直後、Draw呼び出しの直前に呼ぶこと
    //-----------------------------------------------------------
    void UpdateAndBind() const
    {
        if(m_constantBufferHandle == -1) return;

        void* dst = GetBufferShaderConstantBuffer(m_constantBufferHandle);
        if(!dst) return;

        std::memcpy(dst, &m_constants, sizeof(ToonMaterialConstants));
        UpdateShaderConstantBuffer(m_constantBufferHandle);
        SetShaderConstantBuffer(m_constantBufferHandle, DX_SHADERTYPE_PIXEL, TOON_CBUFFER_SLOT);
    }

    //! @brief 定数バッファを解放する（DxLib_End() より前に呼ぶこと）
    void Release()
    {
        if(m_constantBufferHandle != -1) {
            DeleteShaderConstantBuffer(m_constantBufferHandle);
            m_constantBufferHandle = -1;
        }
    }

    bool IsValid() const { return m_constantBufferHandle != -1; }

private:
    ToonMaterialConstants m_constants;
    int                   m_constantBufferHandle = -1;    //!< DxLib の定数バッファハンドル
};

}    // namespace Graphics

//============================================================================
// 使用例 (BallObject::OnDraw など)
//----------------------------------------------------------------------------
// m_toonMaterial.SetShadowThreshold(0.5f);
// m_toonMaterial.SetShadowColor(0.55f, 0.55f, 0.75f); // 青紫がかった影
// m_toonMaterial.SetRimIntensity(0.6f);
// GetMaterial()->Bind();          // Toon_PS.fx を差し替え
// m_toonMaterial.UpdateAndBind(); // トゥーンパラメータ(b5)を転送
// DrawSphere3D(center, m_radius, ...);
// GetMaterial()->Unbind();
//============================================================================
