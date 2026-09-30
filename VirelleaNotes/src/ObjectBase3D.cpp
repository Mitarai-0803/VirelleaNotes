//============================================================================
//! @file   ObjectBase3D.cpp
//! @brief  3Dオブジェクト基底クラスの実装
//! @author レオ
//============================================================================
#include "ObjectBase3D.h"
#include "CollisionComponent.h"

namespace Object {

//-----------------------------------------------------------
//! @brief 座標を取得する
//! @details コライダーがあれば物理座標を返す。なければ手動設定の座標を返す。
//! @return ワールド座標
//-----------------------------------------------------------
JPH::RVec3 ObjectBase3D::GetPosition() const
{
    auto col = GetComponent<CollisionComponent>();
    if(col) {
        return col->GetPosition();
    }

    return m_pos;
}

//-----------------------------------------------------------
//! @brief 座標を設定する
//! @details コライダーがあれば物理ボディも同期して移動する。
//! @param pos 設定するワールド座標
//-----------------------------------------------------------
void ObjectBase3D::SetPosition(const JPH::RVec3& pos)
{
    auto col = GetComponent<CollisionComponent>();
    if(col) {
        col->SetPosition(pos);
    }
    m_pos = pos;
}

//-----------------------------------------------------------
//! @brief 回転を取得する
//! @details コライダーがあれば物理回転を返す。なければ手動設定の回転を返す。
//! @return 回転（クォータニオン）
//-----------------------------------------------------------
JPH::Quat ObjectBase3D::GetRotation() const
{
    auto col = GetComponent<CollisionComponent>();
    if(col) {
        return col->GetRotation();
    }

    return m_rot;
}

//-----------------------------------------------------------
//! @brief 回転を設定する
//! @details コライダーがあれば物理ボディも同期して回転する。
//! @param rot 設定する回転（クォータニオン）
//-----------------------------------------------------------
void ObjectBase3D::SetRotation(const JPH::Quat& rot)
{
    auto col = GetComponent<CollisionComponent>();
    if(col) {
        col->SetRotation(rot);
    }

    m_rot = rot;
}

}    // namespace Object
