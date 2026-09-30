#pragma once
//============================================================================
//! @file   Material.h
//! @brief  Shader とその描画パラメータをまとめる Material クラスの宣言
//! @details
//!  責務分離: Shader → Material → Renderer → RenderPipeline
//!  Object は Shader を直接操作せず、この Material を経由して間接的に扱う。
//! @author レオ
//============================================================================
#include "Shader.h"
#include <hlsl++.h>
#include <memory>
#include <string>

namespace Graphics {

//-----------------------------------------------------------
//! @class Material
//! @brief Shader + 描画パラメータ（現状は基本色のみ）をまとめるクラス
//! @details
//!  @note 頂点/ピクセルシェーダーへの定数（コンスタントバッファ／レジスタ）の
//!        受け渡しは、DXライブラリのバージョンによって API が異なるため、
//!        本クラスでは値の保持のみを行い、実際の転送処理は追加していない。
//!        シェーダー側で色などを利用したい場合は、Bind() 呼び出し後に
//!        使用しているDXライブラリのバージョンに対応する定数設定API
//!        （例: SetVSConstF 系 / DX11版の定数バッファ設定APIなど）を
//!        呼び出す処理をここに追加すること。
//-----------------------------------------------------------
class Material
{
public:
    Material() = default;

    //! @param shader 使用する Shader
    explicit Material(std::shared_ptr<Shader> shader) : m_shader(std::move(shader)) {}

    //! @param shader 使用する Shader
    void SetShader(std::shared_ptr<Shader> shader) { m_shader = std::move(shader); }

    //! @return 使用している Shader（未設定なら nullptr）
    std::shared_ptr<Shader> GetShader() const { return m_shader; }

    //! @param color 基本色（RGBA、0.0〜1.0）
    void SetColor(const hlslpp::float4& color) { m_color = color; }

    //! @return 基本色（RGBA、0.0〜1.0）
    const hlslpp::float4& GetColor() const { return m_color; }

    //-----------------------------------------------------------
    //! @brief Shader をバインドする（未設定 or 読み込み失敗時は何もしない）
    //! @return true ならバインドを実行した
    //-----------------------------------------------------------
    bool Bind() const
    {
        if(!m_shader || !m_shader->IsValid()) return false;
        m_shader->Bind();
        return true;
    }

    //! @brief Shader のバインドを解除する
    void Unbind() const
    {
        if(m_shader) m_shader->Unbind();
    }

private:
    std::shared_ptr<Shader> m_shader;                                        //!< 使用する Shader（未設定可）
    hlslpp::float4          m_color = hlslpp::float4(1.0f, 1.0f, 1.0f, 1.0f); //!< 基本色（RGBA）
};

}    // namespace Graphics
