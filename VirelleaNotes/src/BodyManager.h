#pragma once
//============================================================================
//! @file   BodyManager.h
//! @brief  物理ボディ管理クラスの宣言
//! @details 生成された CollisionComponent を管理し、BodyID からの逆引きなどを提供する
//! @author レオ
//============================================================================
#include <unordered_map>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

class CollisionComponent;

//-----------------------------------------------------------
//! @class BodyManager
//! @brief 物理ボディ管理クラス
//! @details JPH::BodyID をキーとして CollisionComponent の生ポインタを
//!          保持し、物理コールバックなどから元のコンポーネントを
//!          逆引きできるようにする。所有権は持たない。
//-----------------------------------------------------------
class BodyManager
{
public:
    //! @brief コンストラクタ
    BodyManager() = default;

    //! @brief デストラクタ
    ~BodyManager() = default;

    //-----------------------------------------------------------
    //! @brief コンポーネントの登録
    //! @param id BodyID
    //! @param component 登録するコンポーネント（所有権は持たない）
    //-----------------------------------------------------------
    void RegisterComponent(JPH::BodyID id, CollisionComponent* component) { mComponentMap[id] = component; }

    //-----------------------------------------------------------
    //! @brief コンポーネントの登録解除
    //! @param id 解除する BodyID
    //-----------------------------------------------------------
    void UnregisterComponent(JPH::BodyID id) { mComponentMap.erase(id); }

    //-----------------------------------------------------------
    //! @brief BodyID からコンポーネントを取得する
    //! @param id 検索する BodyID
    //! @return 見つかればそのポインタ、なければ nullptr
    //-----------------------------------------------------------
    CollisionComponent* FindComponent(JPH::BodyID id) const
    {
        auto it = mComponentMap.find(id);
        if(it != mComponentMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    //-----------------------------------------------------------
    //! @brief 登録済みの全コンポーネントを取得する
    //! @details デバッグ表示（ImGuiでの値確認など）で全ボディを
    //!          巡回する用途を想定。所有権は持たない参照を返す。
    //! @return BodyID -> CollisionComponent* の対応表
    //-----------------------------------------------------------
    const std::unordered_map<JPH::BodyID, CollisionComponent*>& GetAll() const { return mComponentMap; }

private:
    std::unordered_map<JPH::BodyID, CollisionComponent*> mComponentMap;    //!< BodyID から CollisionComponent* へのマップ
};
