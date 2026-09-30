#pragma once
//============================================================================
//! @file   ObjectBase2D.h
//! @brief  2Dオブジェクト基底クラス
//! @details スクリーン座標（x, y）を持つオブジェクトの基底
//! @author レオ
//============================================================================
#include "ObjectBase.h"

namespace Object {
class ObjectBase2D : public ObjectBase
{
public:
    //! @brief コンストラクタ
    ObjectBase2D() = default;

    //! @brief デストラクタ
    virtual ~ObjectBase2D() = default;

    //-----------------------------------------------------------
    //! @brief X座標の取得
    //! @return スクリーンX座標
    //-----------------------------------------------------------
    float GetX() const;

    //-----------------------------------------------------------
    //! @brief Y座標の取得
    //! @return スクリーンY座標
    //-----------------------------------------------------------
    float GetY() const;

    //-----------------------------------------------------------
    //! @brief X座標の設定
    //! @param x 設定するX座標
    //-----------------------------------------------------------
    void SetX(float x);

    //-----------------------------------------------------------
    //! @brief Y座標の設定
    //! @param y 設定するY座標
    //-----------------------------------------------------------
    void SetY(float y);

    //-----------------------------------------------------------
    //! @brief X・Y座標の一括設定
    //! @param x 設定するX座標
    //! @param y 設定するY座標
    //-----------------------------------------------------------
    void SetPos(float x, float y);

    //! @return 回転角（ラジアン）
    float GetRotation() const { return m_rotation; }

    //! @param rotation 設定する回転角（ラジアン）
    void SetRotation(float rotation) { m_rotation = rotation; }

protected:
    float m_x = 0.0f;           //!< スクリーンX座標
    float m_y = 0.0f;           //!< スクリーンY座標
    float m_rotation = 0.0f;    //!< 回転角（ラジアン）。描画方法は派生クラスに委ねる
};
}    // namespace Object
