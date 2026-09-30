//----------------------------------------------------------------------------
//! @file   Colors.h
//! @brief  カラー定義
//! @details UI や画面描画で利用する色定数を定義します。DxLib の GetColor を利用する定義を含みます。
//----------------------------------------------------------------------------
#pragma once

#include <DxLib.h>    //!< DXライブラリ利用

//--------------------------------------------------------
// 背景色の浮動小数定数
//--------------------------------------------------------
constexpr float DEFAULT_BG_R = 68.0f;     //!< 背景 R
constexpr float DEFAULT_BG_G = 76.0f;     //!< 背景 G
constexpr float DEFAULT_BG_B = 120.0f;    //!< 背景 B

constexpr float HIGHLIGHT_BG_R = 255.0f;    //!< ハイライト R
constexpr float HIGHLIGHT_BG_G = 127.0f;    //!< ハイライト G
constexpr float HIGHLIGHT_BG_B = 80.0f;     //!< ハイライト B

//--------------------------------------------------------
// GetColor 用のカラー定数
//--------------------------------------------------------
const unsigned int COLOR_WHITE               = GetColor(255, 255, 255);    //!< 白
const unsigned int COLOR_BLACK               = GetColor(0, 0, 0);          //!< 黒
const unsigned int COLOR_RED                 = GetColor(255, 0, 0);        //!< 赤
const unsigned int COLOR_PINK                = GetColor(255, 150, 255);    //!< ピンク
const unsigned int COLOR_GREEN               = GetColor(0, 255, 0);        //!< 緑
const unsigned int COLOR_YELLOW_GREEN        = GetColor(150, 255, 255);    //!< 黄緑
const unsigned int COLOR_LIGHT_BLUISH_PURPLE = GetColor(184, 193, 236);    //!< 淡い青紫
const unsigned int COLOR_BLUE                = GetColor(0, 0, 255);        //!< 青
const unsigned int COLOR_NAVY_BLUE           = GetColor(35, 41, 70);       //!< ネイビーブルー
const unsigned int COLOR_YELLOW              = GetColor(255, 255, 0);      //!< 黄
const unsigned int COLOR_ORANGE              = GetColor(255, 127, 80);     //!< オレンジ
const unsigned int COLOR_CYAN                = GetColor(0, 255, 255);      //!< シアン
const unsigned int COLOR_MAGENTA             = GetColor(255, 0, 255);      //!< マゼンタ
const unsigned int COLOR_GRAY                = GetColor(128, 128, 128);    //!< グレー
const unsigned int COLOR_DARKGRAY            = GetColor(64, 64, 64);       //!< 濃いグレー
const unsigned int COLOR_LIGHTGRAY           = GetColor(192, 192, 192);    //!< 薄いグレー
const unsigned int COLOR_LIGHT_YELLOW_GREEN  = GetColor(193, 208, 169);    //!< UI 補助色
const unsigned int COLOR_LIGHT_BLUE          = GetColor(84, 124, 154);     //!< UI 補助青
