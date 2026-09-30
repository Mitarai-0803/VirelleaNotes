#pragma once
//============================================================================
//! @file   RenderPipeline.h
//! @brief  RenderPass を順番に実行する RenderPipeline クラスの宣言
//! @author レオ
//============================================================================
#include "RenderPass.h"
#include <memory>
#include <vector>

namespace Graphics {

//-----------------------------------------------------------
//! @class RenderPipeline
//! @brief 登録された RenderPass を順番に実行するクラス
//! @details Renderer が1つ保持し、Scene から渡されたオブジェクト一覧と
//!          カメラを毎フレーム各 RenderPass に渡す。
//-----------------------------------------------------------
class RenderPipeline
{
public:
    RenderPipeline() = default;

    //-----------------------------------------------------------
    //! @brief RenderPass を末尾に追加する
    //! @param pass 追加する RenderPass（所有権を受け取る）
    //-----------------------------------------------------------
    void AddPass(std::unique_ptr<RenderPass> pass);

    //-----------------------------------------------------------
    //! @brief 登録されている全 RenderPass を順番に実行する
    //! @param objects 描画候補オブジェクト一覧
    //! @param camera 描画に使用するカメラ
    //-----------------------------------------------------------
    void Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera);

    //! @brief 登録されている RenderPass を全て削除する
    void Clear();

private:
    std::vector<std::unique_ptr<RenderPass>> m_passes;    //!< 登録された RenderPass（実行順）
};

}    // namespace Graphics
