#pragma once
//============================================================================
//! @file   ConstantsDebug.h
//! @brief  デバッグ機能関連の定数定義
//! @details F3デバッグモード（当たり判定表示・ImGui値確認）で使用する定数を定義します。
//! @author レオ
//============================================================================

// --- 当たり判定ワイヤーフレームの描画色 ---
constexpr int DEBUG_HITBOX_COLOR_R = 0;      //!< 当たり判定ワイヤーフレームの赤成分
constexpr int DEBUG_HITBOX_COLOR_G = 255;    //!< 当たり判定ワイヤーフレームの緑成分
constexpr int DEBUG_HITBOX_COLOR_B = 0;      //!< 当たり判定ワイヤーフレームの青成分

// --- ImGui ウィンドウのレイアウト ---
constexpr float DEBUG_IMGUI_WINDOW_MARGIN         = 10.0f;     //!< ウィンドウを画面端から離す余白
constexpr float DEBUG_IMGUI_WINDOW_WIDTH          = 320.0f;    //!< デバッグ系ウィンドウの初期幅
constexpr float DEBUG_IMGUI_MAIN_WINDOW_POS_X     = DEBUG_IMGUI_WINDOW_MARGIN;    //!< メインウィンドウの初期X座標（左上）
constexpr float DEBUG_IMGUI_MAIN_WINDOW_POS_Y     = DEBUG_IMGUI_WINDOW_MARGIN;    //!< メインウィンドウの初期Y座標（左上）
constexpr float DEBUG_IMGUI_STATE_WINDOW_POS_Y    = DEBUG_IMGUI_WINDOW_MARGIN;    //!< ゲーム状態ウィンドウの初期Y座標（右上。X座標は画面幅から算出する）

// --- フレーム時間の計測・表示 ---
constexpr int   DEBUG_FRAME_HISTORY_SIZE      = 120;       //!< フレーム時間の履歴数（平均・グラフに使う直近フレーム数）
constexpr float DEBUG_FRAME_TIME_PLOT_MAX_MS  = 33.3f;     //!< フレーム時間グラフの縦軸上限（ミリ秒。30FPS相当）
constexpr float DEBUG_FRAME_TIME_PLOT_HEIGHT  = 60.0f;     //!< フレーム時間グラフの高さ（ピクセル）
constexpr float DEBUG_US_TO_MS                = 0.001f;    //!< マイクロ秒 → ミリ秒の換算係数
constexpr float DEBUG_MS_TO_SEC               = 0.001f;    //!< ミリ秒 → 秒の換算係数
constexpr float DEBUG_MS_PER_SEC              = 1000.0f;   //!< 1秒あたりのミリ秒（FPS算出用）
constexpr float DEBUG_MIN_DELTA_TIME          = 0.001f;    //!< ImGuiへ渡すDeltaTimeの下限（0除算防止。秒）

// --- シェーダー調整スライダーの範囲 ---
constexpr float DEBUG_SHADOW_THRESHOLD_MIN = 0.0f;     //!< 影の閾値スライダーの最小値
constexpr float DEBUG_SHADOW_THRESHOLD_MAX = 1.0f;     //!< 影の閾値スライダーの最大値
constexpr int   DEBUG_COLOR_LEVELS_MIN     = 2;        //!< 減色レベルスライダーの最小値（1チャンネルあたりの階調数）
constexpr int   DEBUG_COLOR_LEVELS_MAX     = 32;       //!< 減色レベルスライダーの最大値
constexpr float DEBUG_COLOR_LEVELS_ROUND   = 0.5f;     //!< 減色レベル（float保持）→ int変換時の四捨五入用オフセット
