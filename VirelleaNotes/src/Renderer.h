#pragma once
//============================================================================
//! @file   Renderer.h
//! @brief  Scene から呼び出される描画の入口となる Renderer クラスの宣言
//! @details
//!  責務: Scene / Object の描画要求を受け取り、RenderPipeline に橋渡しする。
//!  DXライブラリを直接操作する処理は RenderPass 側に閉じ込め、
//!  Scene 側のコードが DXライブラリ API だらけにならないようにする。
//!
//!  Scene
//!   ↓
//!  Renderer
//!   ↓
//!  RenderPipeline
//!   ↓
//!  RenderPass（GeometryPass 等）
//!   ↓
//!  Object::Draw()（内部で Material / Shader を利用する場合はそちらへ委譲）
//!   ↓
//!  DXライブラリ
//! @author レオ
//============================================================================
#include "RenderPipeline.h"
#include "ObjectBase.h"
#include "CameraObject.h"
#include <memory>
#include <vector>

namespace Graphics {

//-----------------------------------------------------------
//! @class Renderer
//! @brief Scene が保持し、毎フレームの描画を RenderPipeline に委譲するクラス
//-----------------------------------------------------------
class Renderer
{
public:
    //-----------------------------------------------------------
    //! @brief コンストラクタ
    //! @details デフォルトで ShadowMapPass / GeometryPass / PostProcessPass を登録した
    //!          RenderPipeline を構築する。
    //!          TransparentPass 等をさらに有効にしたい場合は GetPipeline() 経由で
    //!          AddPass() すること。
    //-----------------------------------------------------------
    Renderer();

    //-----------------------------------------------------------
    //! @brief 指定オブジェクト一覧を指定カメラで描画する
    //! @param objects 描画候補オブジェクト一覧（通常は Scene の m_scene_objects）
    //! @param camera 描画に使用するカメラ
    //-----------------------------------------------------------
    void RenderScene(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera);

    //! @return RenderPipeline への参照（パスの追加・入れ替えに使用）
    RenderPipeline& GetPipeline() { return m_pipeline; }

    //-----------------------------------------------------------
    //! @brief 画面全体に適用するポストエフェクトのシェーダー（Material）を差し替える
    //! @details コンストラクタで SHADER_NAME_POSTPROCESS を自動で読み込んで設定済みのため、
    //!          通常は呼ぶ必要はない。別のポストエフェクトに切り替えたい場合のみ使用する。
    //! @param material 設定する Material（nullptrなら効果なしの素通しになる）
    //-----------------------------------------------------------
    void SetPostProcessMaterial(std::shared_ptr<Material> material);

private:
    RenderPipeline   m_pipeline;                     //!< このRendererが使用するパイプライン
    PostProcessPass* m_postProcessPass = nullptr;     //!< m_pipeline が所有するPostProcessPassへの非所有参照（Material差し替え用）
};

}    // namespace Graphics
