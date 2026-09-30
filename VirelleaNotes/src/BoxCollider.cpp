//============================================================================
//! @file   BoxCollider.cpp
//! @brief  ボックス当たり判定の実装
//! @details JoltのBoxShapeを生成しBodyを登録
//! @author レオ
//============================================================================
#include "BoxCollider.h"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

void BoxCollider::Init(JPH::PhysicsSystem* system, const JPH::RVec3& pos, JPH::Vec3 halfExtent, JPH::EMotionType motionType)
{
    JPH::Ref<JPH::Shape> shape = new JPH::BoxShape(halfExtent);
    CreateAndAddBody(system, shape, pos, motionType);
}
