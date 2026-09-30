#pragma once
#include "SceneBase.h"
#include "ResourceHandles.h"

namespace Scene {
//-----------------------------------------------------------
//! @class TitleScene
//! @detail タイトル画面のシーン
//-----------------------------------------------------------
class TitleScene : public SceneBase
{
public:
    void SceneInit() override;
    void SceneUpdate() override;
    void SceneDraw() override;
    void SceneEnd() override;

private:
    ImageHandle m_title_image_handle;         //!< タイトル画像ハンドル
    ImageHandle m_violin_image_handle;        //!< タイトルのヴァイオリンの画像ハンドル
    ImageHandle m_violin_bow_image_handle;    //!< タイトルのヴァイオリンの弓の画像ハンドル

    float m_animation_timer = 0.0f;    //!< 浮遊用タイマー（sin計算用）
};
}    // namespace Scene
