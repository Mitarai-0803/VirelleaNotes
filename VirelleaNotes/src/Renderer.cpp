//============================================================================
//! @file   Renderer.cpp
//! @brief  Renderer クラスの実装
//============================================================================
#include "Renderer.h"
#include "ShaderManager.h"
#include "RenderTargetManager.h"
#include "ConstantsGame.h"

namespace Graphics {

Renderer::Renderer()
{
    // GeometryPassの描画先となるオフスクリーンのレンダーターゲットを用意する。
    // RenderTargetManagerはシングルトンなので、シーン切り替えでRendererが
    // 作り直されても、2回目以降のCreateRenderTarget()は既存ハンドルを返すだけの
    // 軽い呼び出しになる（MakeScreenが実行されるのはアプリ全体で最初の1回だけ）。
    RenderTargetManager::GetInstance().CreateRenderTarget(RENDER_TARGET_SCENE_COLOR, SCREEN_W, SCREEN_H);

    //// ポストエフェクト用シェーダー（未配置・コンパイル失敗時は nullptr のままで、
    //// PostProcessPass側がその場合はシェーダーなしの素通しにフォールバックする）。
    auto postProcessShader   = ShaderManager::GetInstance().Get(SHADER_NAME_POSTPROCESS);
    auto postProcessMaterial = std::make_shared<Material>(postProcessShader);

    // 最小構成: 現段階では ShadowMapPass / GeometryPass / PostProcessPass を標準で有効化する。
    // TransparentPass / FinalPass は雛形として用意済みなので、必要になったら以下のように追加する。
    //
    //   m_pipeline.AddPass(std::make_unique<TransparentPass>());
    //   m_pipeline.AddPass(std::make_unique<FinalPass>());

    // ShadowMapPass: ObjectBase3D::SetCastShadow() したオブジェクトの深度をシャドウマップへ焼き込み、
    // 「以降の3D描画でこのシャドウマップを使う」設定を有効にする。
    // 必ず GeometryPass より前に実行し、GeometryPass の通常描画が影を受け取れるようにする。
    m_pipeline.AddPass(std::make_unique<ShadowMapPass>());

    m_pipeline.AddPass(std::make_unique<GeometryPass>(RENDER_TARGET_SCENE_COLOR));

    auto postProcessPass = std::make_unique<PostProcessPass>(RENDER_TARGET_SCENE_COLOR);
    postProcessPass->SetMaterial(postProcessMaterial);
    m_postProcessPass = postProcessPass.get();    // Material差し替え用に非所有ポインタを保持
    m_pipeline.AddPass(std::move(postProcessPass));
}

void Renderer::RenderScene(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera)
{
    m_pipeline.Execute(objects, camera);
}

void Renderer::SetPostProcessMaterial(std::shared_ptr<Material> material)
{
    if(m_postProcessPass) {
        m_postProcessPass->SetMaterial(std::move(material));
    }
}

}    // namespace Graphics
