#pragma once
//============================================================================
//! @file   Sprite2D.h
//! @brief  ObjectBase2D を継承した最小構成の2Dオブジェクトクラスの宣言
//! @details
//!  ObjectBase
//!   └─ ObjectBase2D
//!        └─ Sprite2D   ← 本クラス
//!
//!  テクスチャを使わず、位置・サイズ・色のみを持つ単色矩形として描画する
//!  最小構成のサンプル。UIObject / Text / Image など、他の Object2D 派生
//!  クラスを追加する際の参考実装として使用できる。
//! @author レオ
//============================================================================
#include "ObjectBase2D.h"

namespace Object {

//-----------------------------------------------------------
//! @class Sprite2D
//! @brief 単色矩形として描画される最小構成の2Dオブジェクト
//-----------------------------------------------------------
class Sprite2D : public ObjectBase2D
{
public:
    //-----------------------------------------------------------
    //! @param x 初期X座標
    //! @param y 初期Y座標
    //! @param width 幅
    //! @param height 高さ
    //-----------------------------------------------------------
    Sprite2D(float x, float y, float width, float height);
    virtual ~Sprite2D() = default;

    //-----------------------------------------------------------
    //! @param width 幅
    //! @param height 高さ
    //-----------------------------------------------------------
    void SetSize(float width, float height) { m_width = width; m_height = height; }

    //-----------------------------------------------------------
    //! @param r 赤(0-255)
    //! @param g 緑(0-255)
    //! @param b 青(0-255)
    //-----------------------------------------------------------
    void SetColor(int r, int g, int b) { m_colorR = r; m_colorG = g; m_colorB = b; }

protected:
    void OnInit()   override;
    void OnUpdate() override;
    void OnDraw()   override;
    void OnEnd()    override;

private:
    float m_width  = 32.0f;    //!< 幅
    float m_height = 32.0f;    //!< 高さ

    // NOTE: Texture / UV は未実装（拡張ポイント）。
    //       画像を使う場合は LoadGraph() 等でハンドルを取得し、
    //       DrawRotaGraph() 等に差し替えること。
    int m_colorR = 255;    //!< 赤成分(0-255)
    int m_colorG = 255;    //!< 緑成分(0-255)
    int m_colorB = 255;    //!< 青成分(0-255)
};

}    // namespace Object
