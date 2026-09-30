//============================================================================
//! @file   PhysicsManager.cpp
//! @brief  物理マネージャークラスの実装
//! @author レオ
//============================================================================
#include "PhysicsManager.h"
#include <thread>
#include <cstdint>

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>

// --- Jolt に必要なレイヤー設定（最小構成） ---

// オブジェクトレイヤー番号の定義
namespace Layers
{
static constexpr JPH::ObjectLayer NON_MOVING = 0;    // 動かないオブジェクト
static constexpr JPH::ObjectLayer MOVING     = 1;    // 動くオブジェクト
static constexpr JPH::ObjectLayer NUM_LAYERS = 2;    // レイヤー数
}

// ブロードフェーズレイヤー番号の定義
namespace BroadPhaseLayers {
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr unsigned int         NUM_LAYERS = 2;
}

// ObjectLayer <-> BroadPhaseLayer のマッピング
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl()
    {
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING]     = BroadPhaseLayers::MOVING;
    }

    virtual JPH::uint GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override { return mObjectToBroadPhase[inLayer]; }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
    {
        switch((JPH::BroadPhaseLayer::Type)inLayer) {
        case(JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
            return "NON_MOVING";
        case(JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
            return "MOVING";
        default:
            return "INVALID";
        }
    }
#endif

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

// ObjectLayer 同士の衝突フィルター
class ObjectLayerPairFilterImpl final : public JPH::ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inObj1, JPH::ObjectLayer inObj2) const override
    {
        switch(inObj1) {
        case Layers::NON_MOVING:
            return inObj2 == Layers::MOVING;
        case Layers::MOVING:
            return true;
        default:
            return false;
        }
    }
};

// ObjectLayer <-> BroadPhaseLayer の衝突フィルター
class ObjectVsBroadPhaseLayerFilterImpl final : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer, JPH::BroadPhaseLayer inBroadPhaseLayer) const override
    {
        switch(inLayer) {
        case Layers::NON_MOVING:
            return inBroadPhaseLayer == BroadPhaseLayers::MOVING;
        case Layers::MOVING:
            return true;
        default:
            return false;
        }
    }
};

// --- PhysicsManager の実装 ---

// レイヤー実装をメンバとして保持するため static で確保
static BPLayerInterfaceImpl              sBPLayerInterface;
static ObjectVsBroadPhaseLayerFilterImpl sObjVsBPFilter;
static ObjectLayerPairFilterImpl         sObjLayerPairFilter;

bool PhysicsManager::PhysicsManagerInit()
{
    if(mInitialized) {
        return true;    //!< 既に初期化済みなら何もしない（二重初期化防止）
    }

    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    mTemp = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

    unsigned int thread_count = std::thread::hardware_concurrency();
    unsigned int worker       = (thread_count > 1) ? (thread_count - 1) : 1;
    mJob                      = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, worker);

    mSystem = new JPH::PhysicsSystem();

    // ★ここが抜けていた：PhysicsSystem の実際の初期化
    // 引数: 最大ボディ数, ボディミューテックス数(0=自動), 最大ボディペア数, 最大接触拘束数,
    //       BroadPhaseLayerInterface, ObjectVsBroadPhaseLayerFilter, ObjectLayerPairFilter
    mSystem->Init(1024,     // 最大ボディ数
                  0,        // ボディミューテックス数（0で自動）
                  65536,    // 最大ボディペア数
                  10240,    // 最大接触拘束数
                  sBPLayerInterface,
                  sObjVsBPFilter,
                  sObjLayerPairFilter);

    mInitialized = true;
    return true;
}

void PhysicsManager::PhysicsManagerUpdate()
{
    if(mSystem && mTemp && mJob) {
        mSystem->Update(1.0f / 60.0f, 1, 1, mTemp, mJob);
    }
}

void PhysicsManager::PhysicsManagerEnd()
{
    if(!mInitialized) {
        return;    //!< 未初期化、または既に終了処理済みなら何もしない（多重End()防止）
    }

    delete mSystem;
    mSystem = nullptr;

    delete mJob;
    mJob = nullptr;

    delete mTemp;
    mTemp = nullptr;

    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;

    mInitialized = false;
}
