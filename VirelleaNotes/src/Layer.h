#pragma once
//============================================================================
//! @file   Layer.h
//! @brief  描画・所属レイヤーの定義と LayerMask クラスの宣言
//! @details
//!  ・Object が「どのレイヤーに所属するか」（LayerID）と、
//!  ・Camera / Renderer が「どのレイヤーを描画対象とするか」（LayerMask）
//!  を分離して扱うための仕組み。
//!  LayerID はビット位置に対応するインデックスとして扱う（0〜31）。
//! @author レオ
//============================================================================
#include "Typedef.h"

namespace Layer {

//-----------------------------------------------------------
//! @enum LayerID
//! @brief オブジェクトが所属するレイヤーの種類
//! @details
//!  後から末尾（Effect の手前）に追加していくことで、
//!  32個（0〜31）までレイヤーを拡張できる。
//!  LayerMax は「レイヤー数」を表す番兵であり、実際のレイヤーとしては使用しない。
//-----------------------------------------------------------
enum LayerID : u32
{
    Default    = 0,    //!< デフォルトレイヤー
    Player,            //!< プレイヤー
    Enemy,             //!< 敵
    UI,                //!< UI
    Background,        //!< 背景
    Effect,            //!< エフェクト

    LayerMax           //!< レイヤー数（番兵。使用しないこと）
};

static_assert(LayerMax <= 32, "LayerID は LayerMask (u32) のビット数(32)を超えられません");

//-----------------------------------------------------------
//! @class LayerMask
//! @brief 複数レイヤーの有効・無効をビットマスクで表現するクラス
//! @details
//!  Camera::GetLayerMask() などが返すマスクと、
//!  Object::GetLayer() が返す所属レイヤーを比較することで、
//!  「このカメラはこのオブジェクトを描画するか」を判定できる。
//-----------------------------------------------------------
class LayerMask
{
public:
    //-----------------------------------------------------------
    //! @brief デフォルトコンストラクタ
    //! @details 初期状態では全レイヤーを有効にする
    //-----------------------------------------------------------
    LayerMask() = default;

    //! @param bits 初期ビット値
    explicit LayerMask(u32 bits) : m_bits(bits) {}

    //! @return 全レイヤーを有効にした LayerMask
    static LayerMask All() { return LayerMask(0xFFFFFFFFu); }

    //! @return 全レイヤーを無効にした LayerMask
    static LayerMask None() { return LayerMask(0u); }

    //! @param layer 有効にするレイヤー
    void Enable(LayerID layer) { m_bits |= (1u << static_cast<u32>(layer)); }

    //! @param layer 無効にするレイヤー
    void Disable(LayerID layer) { m_bits &= ~(1u << static_cast<u32>(layer)); }

    //-----------------------------------------------------------
    //! @param layer 切り替えるレイヤー
    //! @param enabled true で有効、false で無効
    //-----------------------------------------------------------
    void SetEnabled(LayerID layer, bool enabled)
    {
        if(enabled) Enable(layer);
        else        Disable(layer);
    }

    //-----------------------------------------------------------
    //! @param layer 判定するレイヤー
    //! @return true ならこのマスクに含まれる（描画対象）
    //-----------------------------------------------------------
    bool Contains(LayerID layer) const { return (m_bits & (1u << static_cast<u32>(layer))) != 0; }

    //! @return 生のビット値
    u32 GetBits() const { return m_bits; }

    //! @param bits 設定するビット値
    void SetBits(u32 bits) { m_bits = bits; }

private:
    u32 m_bits = 0xFFFFFFFFu;    //!< レイヤービット（デフォルトは全レイヤー有効）
};

}    // namespace Layer
