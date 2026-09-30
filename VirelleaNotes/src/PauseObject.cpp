//----------------------------------------------------------------------------
//! @file   PauseObject.cpp
//! @brief  ポーズ画面の描画実装
//----------------------------------------------------------------------------
#include "PauseObject.h"
#include "GameScene.h"
#include "ConstantsRhythm.h"
#include "Font.h"
#include "UIConstants.h"

namespace Object {
//----------------------------------------------------------------------------
//! @brief 初期化処理
//----------------------------------------------------------------------------
void PauseObject::OnInit()
{
    // 初期化時にレイアウトを計算してキャッシュする
    m_box_w = PAUSE_BOX_W;
    m_box_h = PAUSE_BOX_H;
    m_box_x = (SCREEN_W / 2) - (m_box_w / 2);
    m_box_y = (SCREEN_H / 2) - (m_box_h / 2);

    m_big_font   = GetFont(Font::FONT_SIZE_50_INDEX);
    m_small_font = GetFont(Font::FONT_SIZE_25_INDEX);

    // PAUSE_TEXT
    m_text_w = GetDrawStringWidthToHandle(PAUSE_TEXT_STR, -1, m_big_font);
    m_text_h = GetFontSizeToHandle(m_big_font);
    m_text_x = m_box_x + (m_box_w - m_text_w) / 2;
    m_text_y = m_box_y + (m_box_h / 2) - m_text_h + PAUSE_TEXT_Y_ADJUST;

    // SUBTEXT
    m_sub_w = GetDrawStringWidthToHandle(PAUSE_SUBTEXT_STR, -1, m_small_font);
    m_sub_h = GetFontSizeToHandle(m_small_font);
    m_sub_x = m_box_x + (m_box_w - m_sub_w) / 2;
    m_sub_y = m_text_y + m_text_h + PAUSE_SUBTEXT_Y_GAP;

    // INFO
    m_info_w1 = GetDrawStringWidthToHandle(PAUSE_INFO_TITLE_STR, -1, m_small_font);
    m_info_w2 = GetDrawStringWidthToHandle(PAUSE_INFO_SONGSELECT_STR, -1, m_small_font);
    m_info_x1 = m_box_x + (m_box_w - m_info_w1) / 2;
    m_info_x2 = m_box_x + (m_box_w - m_info_w2) / 2;
    m_info_y1 = m_sub_y + m_sub_h + PAUSE_INFO_FIRST_GAP;
    m_info_y2 = m_info_y1 + m_sub_h + PAUSE_INFO_SECOND_GAP;
    // 初期状態は非表示
    m_pause_visible = false;
}

//----------------------------------------------------------------------------
//! @brief 描画処理
//----------------------------------------------------------------------------
void PauseObject::OnDraw()
{
    using namespace Scene;
    // ポーズ中のみ描画
    const bool paused = GameScene::IsPaused();
    if(!paused)
        return;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, COUNTDOWN_BG_ALPHA);
    // 背景を半透明黒で塗り潰す
    DrawBox(m_box_x, m_box_y, m_box_x + m_box_w, m_box_y + m_box_h, GetColor(0, 0, 0), TRUE);
    // 枠線をやや透明で描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
    DrawBox(m_box_x, m_box_y, m_box_x + m_box_w, m_box_y + m_box_h, GetColor(255, 255, 255), FALSE);
    // テキスト描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawStringToHandle(m_text_x, m_text_y, PAUSE_TEXT_STR, GetColor(255, 255, 255), m_big_font);
    // サブテキスト
    DrawStringToHandle(m_sub_x, m_sub_y, PAUSE_SUBTEXT_STR, GetColor(200, 200, 200), m_small_font);
    // ポーズ中の追加操作案内
    DrawStringToHandle(m_info_x1, m_info_y1, PAUSE_INFO_TITLE_STR, GetColor(200, 200, 200), m_small_font);
    DrawStringToHandle(m_info_x2, m_info_y2, PAUSE_INFO_SONGSELECT_STR, GetColor(200, 200, 200), m_small_font);
    // ブレンドモードを元に戻す
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
}    // namespace Object
