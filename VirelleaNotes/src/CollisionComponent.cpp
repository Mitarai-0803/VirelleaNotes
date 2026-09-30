//============================================================================
//! @file   CollisionComponent.cpp
//! @brief  当たり判定コンポーネント基底クラスの実装
//! @author レオ
//============================================================================
#include "CollisionComponent.h"
#include "PhysicsManager.h"
#include "ConstantsDebug.h"
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <DxLib.h>
#include <cassert>

//-----------------------------------------------------------
//! @brief デストラクタ
//! @details 登録済みの物理ボディを解放する
//-----------------------------------------------------------
CollisionComponent::~CollisionComponent()
{
    if(!m_body_id.IsInvalid()) {
        // マネージャーから登録解除
        PhysicsManager::GetInstance().GetBodyManager().UnregisterComponent(m_body_id);

        // Jolt世界からボディを削除・破棄（nullptrチェック付き）
        if(auto* system = PhysicsManager::GetInstance().GetSystem()) {
            auto& bi = system->GetBodyInterface();
            bi.RemoveBody(m_body_id);
            bi.DestroyBody(m_body_id);
        }
    }
}

//-----------------------------------------------------------
//! @brief 物理ボディの生成・登録（内部共通処理）
//! @details CreateBody が失敗（ボディ数上限など）した場合は assert で検出する
//! @param system 物理システム
//! @param shape 使用するコリジョン形状
//! @param pos 初期位置
//! @param motionType Dynamic=動く / Static=動かない
//-----------------------------------------------------------
void CollisionComponent::CreateAndAddBody(JPH::PhysicsSystem* system,
                                          JPH::Ref<JPH::Shape> shape,
                                          const JPH::RVec3& pos,
                                          JPH::EMotionType motionType)
{
    if(!system)
        return;

    auto& bi = system->GetBodyInterface();

    // ボディ生成設定
    JPH::BodyCreationSettings settings(shape, pos, JPH::Quat::sIdentity(), motionType, 1);

    // ユーザーデータにコンポーネント自身のポインタを保存（衝突応答などでの逆引き用）
    settings.mUserData = reinterpret_cast<JPH::uint64>(this);

    // ① ボディの作成（失敗時はnullptrが返る：ボディ数上限超過などが原因）
    JPH::Body* body = bi.CreateBody(settings);
    assert(body != nullptr && "CollisionComponent: ボディの生成に失敗しました。PhysicsSystemの最大ボディ数を確認してください。");
    if(!body)
        return;

    m_body_id = body->GetID();

    // 物理シミュレーションへの追加と有効化
    bi.AddBody(m_body_id, JPH::EActivation::Activate);

    // マネージャーへ登録
    PhysicsManager::GetInstance().GetBodyManager().RegisterComponent(m_body_id, this);
}

JPH::RVec3 CollisionComponent::GetPosition() const
{
    auto* system = PhysicsManager::GetInstance().GetSystem();
    if(!system)
        return JPH::RVec3::sZero();
    return system->GetBodyInterface().GetPosition(m_body_id);
}

void CollisionComponent::SetPosition(const JPH::RVec3& pos)
{
    if(auto* system = PhysicsManager::GetInstance().GetSystem()) {
        system->GetBodyInterface().SetPosition(m_body_id, pos, JPH::EActivation::Activate);
    }
}

JPH::Quat CollisionComponent::GetRotation() const
{
    if(auto* system = PhysicsManager::GetInstance().GetSystem()) {
        return system->GetBodyInterface().GetRotation(m_body_id);
    }
    return JPH::Quat::sIdentity();
}

void CollisionComponent::SetRotation(const JPH::Quat& rot)
{
    if(auto* system = PhysicsManager::GetInstance().GetSystem()) {
        system->GetBodyInterface().SetRotation(m_body_id, rot, JPH::EActivation::Activate);
    }
}

JPH::Vec3 CollisionComponent::GetLinearVelocity() const
{
    if(auto* system = PhysicsManager::GetInstance().GetSystem()) {
        return system->GetBodyInterface().GetLinearVelocity(m_body_id);
    }
    return JPH::Vec3::sZero();
}

void CollisionComponent::SetLinearVelocity(const JPH::Vec3& velocity)
{
    if(auto* system = PhysicsManager::GetInstance().GetSystem()) {
        system->GetBodyInterface().SetLinearVelocity(m_body_id, velocity);
    }
}

void CollisionComponent::AddForce(const JPH::Vec3& force)
{
    if(auto* system = PhysicsManager::GetInstance().GetSystem()) {
        system->GetBodyInterface().AddForce(m_body_id, force);
    }
}

void CollisionComponent::AddImpulse(const JPH::Vec3& impulse)
{
    if(auto* system = PhysicsManager::GetInstance().GetSystem()) {
        system->GetBodyInterface().AddImpulse(m_body_id, impulse);
    }
}

//-----------------------------------------------------------
//! @brief デバッグ描画（当たり判定のワイヤーフレーム表示）
//! @details Jolt形状のローカルAABBを取得し、現在の位置・回転で
//!          変換した8頂点をDXライブラリのDrawLine3Dで結んで描画する。
//!          BoxShape以外（Sphere等）を将来追加する場合も、
//!          AABBの範囲は取得できるため最低限の可視化は行える。
//-----------------------------------------------------------
void CollisionComponent::DrawDebug()
{
    if(m_body_id.IsInvalid())
        return;

    auto* system = PhysicsManager::GetInstance().GetSystem();
    if(!system)
        return;

    JPH::BodyInterface& body_interface = system->GetBodyInterface();

    // 非アクティブ（未追加）なボディは形状取得ができないためスキップ
    if(!body_interface.IsAdded(m_body_id))
        return;

    JPH::RVec3               pos   = body_interface.GetPosition(m_body_id);
    JPH::Quat                rot   = body_interface.GetRotation(m_body_id);
    JPH::RefConst<JPH::Shape> shape = body_interface.GetShape(m_body_id);
    if(!shape)
        return;

    // ローカルAABB（BoxShapeの場合はそのまま半サイズ・中心となる）
    JPH::AABox local_bounds = shape->GetLocalBounds();
    JPH::Vec3  half_extent  = (local_bounds.mMax - local_bounds.mMin) * 0.5f;
    JPH::Vec3  local_center = (local_bounds.mMax + local_bounds.mMin) * 0.5f;

    // ボックスのローカル8頂点
    const JPH::Vec3 local_corners[8] = {
        local_center + JPH::Vec3(-half_extent.GetX(), -half_extent.GetY(), -half_extent.GetZ()),
        local_center + JPH::Vec3( half_extent.GetX(), -half_extent.GetY(), -half_extent.GetZ()),
        local_center + JPH::Vec3( half_extent.GetX(),  half_extent.GetY(), -half_extent.GetZ()),
        local_center + JPH::Vec3(-half_extent.GetX(),  half_extent.GetY(), -half_extent.GetZ()),
        local_center + JPH::Vec3(-half_extent.GetX(), -half_extent.GetY(),  half_extent.GetZ()),
        local_center + JPH::Vec3( half_extent.GetX(), -half_extent.GetY(),  half_extent.GetZ()),
        local_center + JPH::Vec3( half_extent.GetX(),  half_extent.GetY(),  half_extent.GetZ()),
        local_center + JPH::Vec3(-half_extent.GetX(),  half_extent.GetY(),  half_extent.GetZ()),
    };

    // ワールド行列（回転→平行移動）でローカル頂点をワールド座標へ変換
    JPH::Mat44          world_matrix = JPH::Mat44::sRotationTranslation(rot, JPH::Vec3(pos));
    DxLib::VECTOR       world_points[8];
    for(int i = 0; i < 8; ++i) {
        JPH::Vec3 world_pos = world_matrix * local_corners[i];
        world_points[i]     = DxLib::VGet(world_pos.GetX(), world_pos.GetY(), world_pos.GetZ());
    }

    // 12辺を結んでワイヤーフレームボックスを描画
    static const int edge_indices[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},    // 手前面
        {4, 5}, {5, 6}, {6, 7}, {7, 4},    // 奥面
        {0, 4}, {1, 5}, {2, 6}, {3, 7},    // 手前⇔奥の接続辺
    };

    unsigned int color = DxLib::GetColor(DEBUG_HITBOX_COLOR_R, DEBUG_HITBOX_COLOR_G, DEBUG_HITBOX_COLOR_B);
    for(const auto& edge : edge_indices) {
        DxLib::DrawLine3D(world_points[edge[0]], world_points[edge[1]], color);
    }
}
