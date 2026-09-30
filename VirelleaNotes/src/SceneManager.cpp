//============================================================================
//! @file   SceneManager.cpp
//! @brief  シーン管理クラスの実装
//============================================================================
#include "SceneManager.h"
#include "ConstantsEngine.h"
#include <DxLib.h>

namespace Scene {

void SceneManager::RequestChangeScene(const std::string& name)
{
    m_change_pending      = true;
    m_pending_scene_name  = name;
}

bool SceneManager::ChangeSceneImmediate(const std::string& name)
{
    auto it = m_factories.find(name);
    if(it == m_factories.end()) {
        return false;    // 未登録の名前
    }

    // --- 現在Sceneの終了処理 → 破棄 ---
    if(m_current_scene) {
        m_current_scene->End();
        m_current_scene.reset();
    }

    // --- 次Scene生成 → 次SceneのInit → CurrentSceneに設定 ---
    m_current_scene = it->second();
    m_current_scene->SetSceneManager(this);
    m_current_scene->Init();
    m_current_scene_name = name;

    return true;
}

void SceneManager::Update()
{
    // フェード中でなければ、保留中の切り替え要求を受け付けてフェードアウトを開始する
    if(m_fade_state == FadeState::Idle && m_change_pending) {
        m_change_pending   = false;
        m_fade_target_name = m_pending_scene_name;
        m_fade_state       = FadeState::FadingOut;
        m_fade_timer       = 0;
    }

    if(m_fade_state == FadeState::FadingOut) {
        ++m_fade_timer;
        if(m_fade_timer >= FADE_FRAMES_TOTAL) {
            // 画面が暗転しきったタイミングで実際にシーンを差し替える
            ChangeSceneImmediate(m_fade_target_name);
            m_fade_state = FadeState::FadingIn;
            m_fade_timer = 0;
        }
    }
    else if(m_fade_state == FadeState::FadingIn) {
        ++m_fade_timer;
        if(m_fade_timer >= FADE_FRAMES_TOTAL) {
            m_fade_state = FadeState::Idle;
            m_fade_timer = 0;
        }
    }

    if(m_current_scene) {
        m_current_scene->Update();
    }
}

void SceneManager::Draw()
{
    if(m_current_scene) {
        m_current_scene->Draw();
    }

    if(m_fade_state == FadeState::Idle) {
        return;
    }

    // FadingOut : 0(透明) → 最大(不透明・黒で覆う) / FadingIn : 最大 → 0
    const float progress = static_cast<float>(m_fade_timer) / static_cast<float>(FADE_FRAMES_TOTAL);
    const int   alpha    = (m_fade_state == FadeState::FadingOut) ? static_cast<int>(progress * FADE_MAX_ALPHA)
                                                                   : static_cast<int>((1.0f - progress) * FADE_MAX_ALPHA);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
    DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneManager::DrawDebug()
{
    if(m_current_scene) {
        m_current_scene->DrawDebug();
    }
}

void SceneManager::DrawImGui()
{
    if(m_current_scene) {
        m_current_scene->DrawImGui();
    }
}

void SceneManager::EndCurrent()
{
    if(m_current_scene) {
        m_current_scene->End();
        m_current_scene.reset();
    }
    m_current_scene_name.clear();
}

}    // namespace Scene
