//============================================================================
//! @file   Font.cpp
//! @brief  フォント管理ユーティリティの実装
//! @details 各フォントサイズのロードとアンロードを行う
//! @author レオ
//============================================================================
#include "Font.h"
#include <DxLib.h>

namespace Font
{
    // FontManager の静的メンバ定義
    int FontManager::m_font_handles[FONT_MAX] {};

    void FontManager::Init()
    {
        m_font_handles[FONT_SIZE_70_INDEX] = LoadFontDataToHandle("data/Font/FOT_RaglanPunch_70.dft");
        m_font_handles[FONT_SIZE_50_INDEX] = LoadFontDataToHandle("data/Font/FOT_RaglanPunch_50.dft");
        m_font_handles[FONT_SIZE_40_INDEX] = LoadFontDataToHandle("data/Font/FOT_RaglanPunch_40.dft");
        m_font_handles[FONT_SIZE_30_INDEX] = LoadFontDataToHandle("data/Font/FOT_RaglanPunch_30.dft");
        m_font_handles[FONT_SIZE_25_INDEX] = LoadFontDataToHandle("data/Font/FOT_RaglanPunch_25.dft");
        m_font_handles[FONT_SIZE_10_INDEX] = LoadFontDataToHandle("data/Font/FOT_RaglanPunch_10.dft");
    }

    void FontManager::Exit()
    {
        for(int i = 0; i < FONT_MAX; ++i) {
            if(m_font_handles[i] != -1) {
                DeleteFontToHandle(m_font_handles[i]);
                m_font_handles[i] = -1;
            }
        }
    }

    int FontManager::GetFont(int font_index)
    {
        if(font_index < 0 || font_index >= FONT_MAX) font_index = FONT_SIZE_70_INDEX;
        return m_font_handles[font_index];
    }

    void FontInit()
    {
        FontManager::Init();
    }

    void FontExit()
    {
        FontManager::Exit();
    }

    int GetFont(int font_index)
    {
        return FontManager::GetFont(font_index);
    }

}    // namespace Font
