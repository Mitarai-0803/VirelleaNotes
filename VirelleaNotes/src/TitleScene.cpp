//----------------------------------------------------------------------------
//! @file   TitleScene.cpp
//! @brief  タイトルシーンの実装
//! @detail タイトル画面の描画と入力でのシーン遷移を制御します。
//----------------------------------------------------------------------------
#include "TitleScene.h"
#include "DrawFormatStringObject.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "UIConstants.h"
#include "Colors.h"
#include "Font.h"
#include "ObjectBase.h"
#include "SEComponent.h"
#include "BgmComponent.h"
#include "ConstantsRhythm.h"
#include <DxLib.h>
#include <cmath>

namespace Scene {
// 遊び方表示フラグと画像ハンドル（ファイル内静的）
// ファイルスコープのグローバルは CamelCase を使用
static bool ShowHowto        = false;
static int  HowtoImageHandle = -1;

//-------------------------------------------------------
//! シーン初期化処理
//-------------------------------------------------------
void TitleScene::SceneInit()
{
    // スタートガイド
    AddObjectSetName<UI::DrawFormatStringObject>("StartGuide");
    if(auto guide = GetSceneObject<UI::DrawFormatStringObject>("StartGuide")) {
        guide->SetFormat(TITLE_GUIDE_STR);
        guide->SetFontHandle(GetFont(Font::FONT_SIZE_25_INDEX));
        guide->SetColor(COLOR_WHITE);
        guide->ApplyFormat();

        int gw = GetDrawStringWidthToHandle(TITLE_GUIDE_STR, -1, GetFont(Font::FONT_SIZE_25_INDEX));
        guide->SetPos(TITLE_GUIDE_X, TITLE_GUIDE_Y);
    }

    // 遊び方ガイド
    AddObjectSetName<UI::DrawFormatStringObject>("HowtoGuide");
    if(auto guide = GetSceneObject<UI::DrawFormatStringObject>("HowtoGuide")) {
        guide->SetFormat(HOWTO_GUIDE_STR);
        guide->SetFontHandle(GetFont(Font::FONT_SIZE_25_INDEX));
        guide->SetColor(COLOR_WHITE);
        guide->ApplyFormat();

        int gw = GetDrawStringWidthToHandle(HOWTO_GUIDE_STR, -1, GetFont(Font::FONT_SIZE_25_INDEX));
        guide->SetPos(HOWTO_GUIDE_X, HOWTO_GUIDE_Y);
    }

    // 画像ロード
    m_title_image_handle      = ImageHandle::Load(TITLE_IMAGE_PATH);
    m_violin_image_handle     = ImageHandle::Load(TITLE_VIOLIN_IMAGE_PATH);
    m_violin_bow_image_handle = ImageHandle::Load(TITLE_BOW_IMAGE_PATH);

    // 遊び方画像を初期化時に読み込む（描画は SceneDraw、終了で解放）
    if(HowtoImageHandle == -1) {
        HowtoImageHandle = LoadGraph(HOWTO_IMAGE_PATH);
    }

    // SE 再生用オブジェクトを作成してコンポーネントを追加
    auto se_obj = std::make_shared<Object::ObjectBase>();
    if(se_obj) {
        se_obj->SetName("TitleSEObject");
        se_obj->Init();
        auto se_comp = se_obj->AddComponent<Audio::SEComponent>();
        // 効果音ファイルを設定
        se_comp->SetPath(TITLE_SE_PATH);
        AddObject(se_obj);
    }

    // BGM 再生用オブジェクトを作成してコンポーネントを追加
    auto bgm_obj = std::make_shared<Object::ObjectBase>();
    if(bgm_obj) {
        bgm_obj->SetName("TitleBgmObject");
        bgm_obj->Init();
        auto bgm_comp = bgm_obj->AddComponent<Audio::BgmComponent>();
        // BGM ファイルを設定して再生（ループ）
        bgm_comp->SetPath(TITLE_BGM_PATH);
        bgm_comp->Play(true);
        AddObject(bgm_obj);
    }

    // アニメーションタイマー初期化
    m_animation_timer = 0.0f;
}

//-------------------------------------------------------
//! シーン更新処理
//-------------------------------------------------------
void TitleScene::SceneUpdate()
{
    // アニメーション更新
    m_animation_timer += 0.05f;

    // スペース押下で SE 再生してシーン遷移
    if(InputManager::PushHitKey(KEY_INPUT_SPACE)) {
        // タイトル SE オブジェクト取得して再生
        if(auto obj = GetSceneObject<Object::ObjectBase>("TitleSEObject")) {
            if(auto se_comp = obj->GetComponent<Audio::SEComponent>()) {
                se_comp->Play();
            }
        }

        ChangeScene(Rhythm::SceneName::SONG_SELECT);
    }

    // H キーで遊び方のトグル表示
    if(InputManager::PushHitKey(KEY_INPUT_H)) {
        ShowHowto = !ShowHowto;
        // 表示中は BGM を一時停止
        if(auto obj = GetSceneObject<Object::ObjectBase>("TitleBgmObject")) {
            if(auto bgm = obj->GetComponent<Audio::BgmComponent>()) {
                if(ShowHowto)
                    bgm->Stop();
                else
                    bgm->Play(true);
            }
        }
        // 画像ロード（未ロードなら）
        if(ShowHowto && HowtoImageHandle == -1) {
            HowtoImageHandle = LoadGraph(HOWTO_IMAGE_PATH);
        }
    }
}

//-------------------------------------------------------
//! シーン描画処理
//-------------------------------------------------------
void TitleScene::SceneDraw()
{
    DrawRotaGraph(HALF_SCREEN_W, HALF_SCREEN_H, 1.0f, 0.0f, m_title_image_handle.Get(), true);

    float baseX = (HALF_SCREEN_W);
    float baseY = (HALF_SCREEN_H);

    float violinOffset = std::sin(m_animation_timer * 0.3f) * 8.0f;
    float bowOffset    = std::sin(m_animation_timer * 0.5f) * 6.0f;

    DrawRotaGraphF(baseX, baseY + violinOffset, 1.0f, 0.0f, m_violin_image_handle.Get(), true);

    DrawRotaGraphF(baseX, baseY + bowOffset, 1.0f, 0.0f, m_violin_bow_image_handle.Get(), true);

    if(auto start_guide = GetSceneObject<UI::DrawFormatStringObject>("StartGuide")) {
        start_guide->Draw();
    }

    if(auto howto_guide = GetSceneObject<UI::DrawFormatStringObject>("HowtoGuide")) {
        howto_guide->Draw();
    }

    // 遊び方表示
    if(ShowHowto) {
        // 画像があれば画面中央に描画
        if(HowtoImageHandle != -1) {
            DrawRotaGraphF(HALF_SCREEN_W, HALF_SCREEN_H, 1.0f, 0.0f, HowtoImageHandle, TRUE);
        }
    }
}

//-------------------------------------------------------
//! シーン終了処理
//-------------------------------------------------------
void TitleScene::SceneEnd()
{
    // タイトル BGM を停止
    if(auto obj = GetSceneObject<Object::ObjectBase>("TitleBgmObject")) {
        if(auto bgm_comp = obj->GetComponent<Audio::BgmComponent>()) {
            bgm_comp->Stop();
        }
    }
    // 遊び方画像がロードされていれば解放
    if(HowtoImageHandle != -1) {
        DeleteGraph(HowtoImageHandle);
        HowtoImageHandle = -1;
        ShowHowto        = false;
    }
}
}    // namespace Scene
