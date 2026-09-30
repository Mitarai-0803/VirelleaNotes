#pragma once
//============================================================================
//! @file   BoxCollider.h
//! @brief  ボックス当たり判定
//! @details BoxShapeによるコリジョン
//! @author レオ
//============================================================================
#include "CollisionComponent.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/MotionType.h>

class BoxCollider : public CollisionComponent
{
public:
    //-----------------------------------------------------------
    //! @details ボックス形状を生成し、物理システムに Body を登録する
    //! @param system 物理システム
    //! @param pos 初期位置
    //! @param halfExtent ボックスの半サイズ（デフォルト: 0.5f の立方体）
    //! @param motionType Dynamic=動く / Static=動かない（デフォルト: Dynamic）
    //-----------------------------------------------------------
    void Init(JPH::PhysicsSystem* system,
              const JPH::RVec3&   pos,
              JPH::Vec3           halfExtent = JPH::Vec3(0.5f, 0.5f, 0.5f),
              JPH::EMotionType    motionType = JPH::EMotionType::Dynamic);
};
