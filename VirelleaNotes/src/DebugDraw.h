#pragma once
//============================================================================
//! @file   DebugDraw.h
//! @brief  F3デバッグモード用の当たり判定ワイヤーフレーム描画ヘルパー
//! @details
//!  パドル・ボール・ブロックなどは物理コライダー(CollisionComponent)を持たず、
//!  自前のAABB／球で判定しているため、CollisionComponent::DrawDebug() では
//!  当たり判定が表示されない。各オブジェクトの OnDrawDebug() から本ヘルパーを
//!  呼び、自前判定の範囲をワイヤーフレームで表示する。
//============================================================================
#include "ConstantsDebug.h"
#include <DxLib.h>

namespace DebugDraw {

//! @brief 当たり判定用ワイヤーフレームの色を返す
inline unsigned int HitboxColor()
{
    return GetColor(DEBUG_HITBOX_COLOR_R, DEBUG_HITBOX_COLOR_G, DEBUG_HITBOX_COLOR_B);
}

//-----------------------------------------------------------
//! @brief 軸に平行なボックス（AABB）をワイヤーフレームで描画する
//! @param center 中心座標
//! @param half 各軸の半サイズ
//-----------------------------------------------------------
inline void Box(const VECTOR& center, const VECTOR& half)
{
    const VECTOR p[8] = {
        VGet(center.x - half.x, center.y - half.y, center.z - half.z),
        VGet(center.x + half.x, center.y - half.y, center.z - half.z),
        VGet(center.x + half.x, center.y + half.y, center.z - half.z),
        VGet(center.x - half.x, center.y + half.y, center.z - half.z),
        VGet(center.x - half.x, center.y - half.y, center.z + half.z),
        VGet(center.x + half.x, center.y - half.y, center.z + half.z),
        VGet(center.x + half.x, center.y + half.y, center.z + half.z),
        VGet(center.x - half.x, center.y + half.y, center.z + half.z),
    };
    static const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7},
    };
    const unsigned int color = HitboxColor();
    for(const auto& e : edges) {
        DrawLine3D(p[e[0]], p[e[1]], color);
    }
}

//-----------------------------------------------------------
//! @brief 球をワイヤーフレームで描画する
//! @param center 中心座標
//! @param radius 半径
//-----------------------------------------------------------
inline void Sphere(const VECTOR& center, float radius)
{
    const unsigned int color = HitboxColor();
    DrawSphere3D(center, radius, 8, color, color, FALSE);
}

}    // namespace DebugDraw
