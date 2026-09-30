#pragma once
//============================================================================
//! @file   PhysicsManager.h
//! @brief  物理マネージャークラスの宣言
//! @details Jolt Physics の初期化・更新・終了処理をシングルトンで一元管理する
//! @author レオ
//============================================================================
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include "BodyManager.h"

//-----------------------------------------------------------
//! @class PhysicsManager
//! @brief 物理システム（Jolt）を1つだけで安全に管理するクラス（シングルトン）
//-----------------------------------------------------------
class PhysicsManager
{
public:
    //-----------------------------------------------------------
    //! @brief 唯一のインスタンスを取得する
    //! @return PhysicsManager のインスタンス
    //-----------------------------------------------------------
    static PhysicsManager& GetInstance()
    {
        static PhysicsManager instance;
        return instance;
    }

    // コピーや代入を禁止する
    PhysicsManager(const PhysicsManager&)            = delete;
    PhysicsManager& operator=(const PhysicsManager&) = delete;

    //-----------------------------------------------------------
    //! @brief 物理システムの初期化
    //! @details 二重初期化を防ぐため、既に初期化済みの場合は何もせず
    //!          true を返す。
    //! @return true 初期化成功（既に初期化済みの場合も true）
    //-----------------------------------------------------------
    bool PhysicsManagerInit();

    //! @brief 物理シミュレーションを1ステップ進める
    void PhysicsManagerUpdate();

    //-----------------------------------------------------------
    //! @brief 物理システムの終了処理
    //! @details 未初期化、または既に終了処理済みの場合は何もしない
    //!          （多重 End() 呼び出しに対して安全）。
    //-----------------------------------------------------------
    void PhysicsManagerEnd();

    //-----------------------------------------------------------
    //! @brief 物理システム本体の取得
    //! @return JPH::PhysicsSystem へのポインタ
    //-----------------------------------------------------------
    JPH::PhysicsSystem* GetSystem() const { return mSystem; }

    //-----------------------------------------------------------
    //! @brief BodyManager の取得
    //! @return BodyManager への参照
    //-----------------------------------------------------------
    BodyManager& GetBodyManager() { return mBodyManager; }

private:
    //! @brief コンストラクタ
    PhysicsManager() = default;

    //! @brief デストラクタ
    ~PhysicsManager() = default;

    JPH::PhysicsSystem*       mSystem = nullptr;    //!< Jolt 物理システム本体
    JPH::TempAllocatorImpl*   mTemp   = nullptr;    //!< 一時アロケータ
    JPH::JobSystemThreadPool* mJob    = nullptr;    //!< 物理演算用ジョブシステム

    bool mInitialized = false;    //!< PhysicsManagerInit() 済みか（多重初期化・多重終了処理のガード）

    BodyManager mBodyManager;    //!< BodyID からコンポーネントを逆引きするマネージャー
};
