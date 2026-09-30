#pragma once
//============================================================================
//! @file   ToonStyle.h
//! @brief  ボール・ブロック等の描画をトゥーンシェーダーに切り替える窓口（ヘッダのみ）
//! @details 使い方（各オブジェクトの OnDraw 内）:
//!            bool toon = Graphics::ToonStyle::GetInstance().Begin();
//!            DrawSphere3D(...);
//!            if(toon) Graphics::ToonStyle::GetInstance().End();
//!          Begin() が false（未ロード・シャドウマップ記録中）のときは
//!          何も変えないので、従来どおりの描画になる。
//============================================================================
#include "ConstantsEngine.h"
#include "MaterialToon.h"
#include "Shader.h"
#include "ShaderManager.h"
#include "ShadowMapManager.h"
#include <DxLib.h>
#include <memory>

namespace Graphics {

class ToonStyle
{
public:
    static ToonStyle& GetInstance()
    {
        static ToonStyle instance;
        return instance;
    }

    //! @brief シェーダー読み込みと定数バッファ作成（DxLib_Init() の後に一度だけ）
    bool Init()
    {
        m_shader = ShaderManager::GetInstance().Load(SHADER_NAME_TOON, SHADER_PATH_TOON_VERTEX, SHADER_PATH_TOON_PIXEL);
        const bool bufferOk = m_material.CreateConstantBuffer();
        m_ready = (m_shader != nullptr) && bufferOk;
        OutputDebugStringA(m_ready ? "[ToonStyle] ready\n" : "[ToonStyle] init FAILED (Toon shader or constant buffer) -> 通常描画にフォールバック\n");
        return m_ready;
    }

    //-----------------------------------------------------------
    //! @brief トゥーンシェーダーをバインドする
    //! @return true なら適用した（描画後に End() を呼ぶこと）
    //-----------------------------------------------------------
    bool Begin() const
    {
        if(!m_ready) return false;
        if(ShadowMapManager::GetInstance().IsRecording()) return false;    // 深度記録中は通常シェーダーのまま

        m_shader->Bind();
        m_material.UpdateAndBind();
        return true;
    }

    void End() const
    {
        if(m_shader) m_shader->Unbind();
    }

    //! @brief 影色・リム・ハイライト等の調整用（全オブジェクト共通）
    MaterialToon& GetMaterial() { return m_material; }

    void Release()
    {
        m_material.Release();
        m_shader.reset();
        m_ready = false;
    }

private:
    ToonStyle()  = default;
    ~ToonStyle() = default;

    std::shared_ptr<Shader> m_shader;
    MaterialToon            m_material;
    bool                    m_ready = false;
};

}    // namespace Graphics
