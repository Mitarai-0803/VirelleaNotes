#pragma once
//============================================================================
//! @file   CollisionComponent.h
//! @brief  当たり判定コンポーネント基底クラスの宣言
//! @details Jolt の BodyID および物理挙動を管理するコンポーネント基底
//! @author レオ
//============================================================================
#include "ComponentBase.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

//-----------------------------------------------------------
//! @class CollisionComponent
//! @brief 当たり判定コンポーネント基底クラス
//! @details 派生クラス（BoxCollider 等）は CreateAndAddBody() を呼んで
//!          物理システムにボディを登録する。
//-----------------------------------------------------------
class CollisionComponent : public ComponentBase
{
public:
    //! @brief コンストラクタ
    CollisionComponent() = default;

    //-----------------------------------------------------------
    //! @brief デストラクタ
    //! @details 登録済みボディの解放処理を行う
    //-----------------------------------------------------------
    virtual ~CollisionComponent();

    //-----------------------------------------------------------
    //! @brief BodyID の取得
    //! @return Jolt の BodyID
    //-----------------------------------------------------------
    JPH::BodyID GetBodyID() const { return m_body_id; }

    //-----------------------------------------------------------
    //! @brief 座標の取得
    //! @return ワールド座標
    //-----------------------------------------------------------
    JPH::RVec3 GetPosition() const;

    //-----------------------------------------------------------
    //! @brief 座標の設定
    //! @param pos 設定するワールド座標
    //-----------------------------------------------------------
    void SetPosition(const JPH::RVec3& pos);

    //-----------------------------------------------------------
    //! @brief 回転の取得
    //! @return 回転（クォータニオン）
    //-----------------------------------------------------------
    JPH::Quat GetRotation() const;

    //-----------------------------------------------------------
    //! @brief 回転の設定
    //! @param rot 設定する回転（クォータニオン）
    //-----------------------------------------------------------
    void SetRotation(const JPH::Quat& rot);

    //-----------------------------------------------------------
    //! @brief 速度の取得
    //! @return 線形速度
    //-----------------------------------------------------------
    JPH::Vec3 GetLinearVelocity() const;

    //-----------------------------------------------------------
    //! @brief 速度の設定
    //! @param velocity 設定する線形速度
    //-----------------------------------------------------------
    void SetLinearVelocity(const JPH::Vec3& velocity);

    //-----------------------------------------------------------
    //! @brief 継続的な力を加える
    //! @param force 加える力
    //-----------------------------------------------------------
    void AddForce(const JPH::Vec3& force);

    //-----------------------------------------------------------
    //! @brief 瞬間的な衝撃を加える
    //! @param impulse 加える力積
    //-----------------------------------------------------------
    void AddImpulse(const JPH::Vec3& impulse);

    //-----------------------------------------------------------
    //! @brief デバッグ描画（当たり判定のワイヤーフレーム表示）
    //! @details F3デバッグモード中のみ SceneBase 経由で呼び出される想定。
    //!          現状 BoxCollider のみ使用しているため、Jolt形状の
    //!          ローカルAABBをそのままボックスとして描画する。
    //-----------------------------------------------------------
    void DrawDebug() override;

protected:
    //-----------------------------------------------------------
    //! @brief 物理ボディの生成・登録
    //! @details 派生クラスの Init() から呼ばれる共通処理
    //! @param system 物理システム
    //! @param shape 使用するコリジョン形状
    //! @param pos 初期位置
    //! @param motionType Dynamic=動く / Static=動かない
    //-----------------------------------------------------------
    void CreateAndAddBody(JPH::PhysicsSystem* system, JPH::Ref<JPH::Shape> shape, const JPH::RVec3& pos, JPH::EMotionType motionType);

    JPH::BodyID m_body_id;    //!< Jolt のボディID
};
