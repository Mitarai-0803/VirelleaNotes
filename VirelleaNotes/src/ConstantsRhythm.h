#pragma once
//============================================================================
//! @file   ConstantsRhythm.h
//! @brief  リズムゲーム（VirelleaNotes）固有の定数定義
//! @details
//!  ノーツ・判定・ポーズ・SongSelect の UI など、リズムゲーム固有の定数を置く。
//!  画面サイズ・フェード・背景色などエンジン領域の定数は ConstantsEngine.h を参照。
//! @author レオ
//============================================================================
#include "ConstantsEngine.h"    //!< 画面サイズ・フェードなどのエンジン定数
#include "Colors.h"
#include <string>

//---------------------------------------------------------------
// シーン名（SceneManager::RegisterScene() の登録名 / SceneBase::ChangeScene() の指定名）
//---------------------------------------------------------------
namespace Rhythm {
namespace SceneName {
constexpr const char* TITLE       = "Title";         //!< タイトル
constexpr const char* SONG_SELECT = "SongSelect";    //!< 曲選択
constexpr const char* GAME        = "Game";          //!< プレイ画面
constexpr const char* RESULT      = "Result";        //!< リザルト
}    // namespace SceneName
}    // namespace Rhythm

//---------------------------------------------------------------
// ノーツを流すライン関連
//---------------------------------------------------------------
constexpr int NOTES_LINE_SPACE = 2;                       //!< ノーツを流すラインの空白
constexpr int NOTES_LINE_MAX   = 4 + NOTES_LINE_SPACE;    //!< ノーツを流すラインの数

constexpr float NOTES_LINE_X = SCREEN_W / NOTES_LINE_MAX;    //!< ノーツを流すラインのX座標
constexpr float NOTES_LINE_Y = 0.0f;                         //!< ノーツを流すラインのY座標
constexpr float NOTES_LINE_W = SCREEN_W / NOTES_LINE_MAX;    //!< ノーツを流すラインの幅
constexpr float NOTES_LINE_H = SCREEN_H;                     //!< ノーツを流すラインの高さ

constexpr float NOTES_LINE_OFFSET_H = 100.0f;    //!< ノーツを流すラインの高さのオフセット

constexpr float BG_LERP_SPEED        = 0.1f;    //!< 背景色を既定色へ戻す補間速度(0..1)
constexpr int   HOLD_EFFECT_INTERVAL = 8;       //!< ホールド中にエフェクトを発生させる間隔(フレーム)
constexpr int   MAX_JUDGE_EFFECTS    = 32;      //!< エフェクトの上限

// 描画用の一時制限（描画パスごとの上限）
constexpr int JUDGE_EFFECTS_DRAW_LIMIT = 16;    //!< 判定エフェクト描画上の上限

// ホールドエフェクト関連
constexpr int HOLD_EFFECT_PARTICLE_COUNT = 6;    //!< ホールド中に発生させるパーティクル数

// ホールド近傍の閾値（近接判定に使用）
constexpr float HOLD_REMOVE_NEAR_THRESHOLD = 8.0f;    //!< ホールド開始座標付近の削除閾値

// デバッグ描画関連
constexpr int DEBUG_LINE_THICKNESS = 2;    //!< デバッグ描画のライン太さ

// 音量調整のステップ量
constexpr int VOLUME_STEP = 8;    //!< 音量を増減させるステップ量

//---------------------------------------------------------------
// ノーツの判定関連
//---------------------------------------------------------------
constexpr float LINE_JUDGEMENT_SPACE = 100.0f;    //!< 判定ラインが下から何ピクセルの位置にあるか
constexpr float JUDGEMENT_W          = 5.0f;      //!< 判定ラインの太さ

constexpr float LINE_JUDGEMENT_X_LEFT_UP    = NOTES_LINE_X;                           //!< 判定ライン左上のX座標
constexpr float LINE_JUDGEMENT_Y_LEFT_UP    = NOTES_LINE_H - LINE_JUDGEMENT_SPACE;    //!< 判定ライン左上のY座標
constexpr float LINE_JUDGEMENT_X_RIGHT_DOWN = NOTES_LINE_W;                           //!< 判定ライン右下のX座標
constexpr float LINE_JUDGEMENT_Y_RIGHT_DOWN = LINE_JUDGEMENT_Y_LEFT_UP;               //!< 判定ライン右下のY座標

constexpr float LINE_JUDGEMENT_Y_OFFSET = NOTES_LINE_OFFSET_H;    //!< 判定ライン右下のX座標(短いほう)

constexpr float JUDGEMENT_H   = 20.0f;    //!< 判定の高さ
constexpr int   JUDGEMENT_MAX = 4;        //!< 判定の数

constexpr int JUDGEMENT_DRAW_TIME_MAX = 60;    //!< 判定を出し続ける最大フレーム数

constexpr float DOWN_STRING_JUDGEMENT_X_OFFSET = 100.0f;    //!< 判定文字列のX座標オフセット
constexpr float DOWN_STRING_JUDGEMENT_Y_OFFSET = 50.0f;     //!< 判定文字列のY座標オフセット

//  ノーツの判定
enum JUDGEMENT
{
    MISS = 0,
    PERFECT,
    GOOD,
    BAD,
    NORMAL    //判定なし
};

//---------------------------------------------------------------
// ノーツ関連
//---------------------------------------------------------------
constexpr float NOTES_W             = 100.0f;    //!< ノーツの幅
constexpr float NOTES_H             = 20.0f;     //!< ノーツの高さ
constexpr float NOTES_DEFAULT_SPEED = 10.0f;     //!< ノーツの速度
constexpr float NOTES_H_OFFSET      = 50.0f;     //!< ノーツの高さのオフセット

constexpr float HOLD_NOTES_W = 20.0f;    //!< 長押しのノーツの四角の幅

constexpr int NOTES_IMAGE_MAX = 3;    //!< ノーツの画像の数

// ノーツタイプ
enum NOTE_TYPE
{
    NOTE_TYPE_MIN = -1,      // 最小値
    NOTE_TYPE_TAP,           // タップノーツ
    NOTE_TYPE_HOLD_START,    // ホールドノーツ開始
    NOTE_TYPE_HOLD_END,      // ホールドノーツ終了
    NOTE_TYPE_SYNC,          // 同時押しノーツ
    NOTE_TYPE_DELETE,        // ノーツ削除
    NOTE_TYPE_MAX            // 最大値
};

//---------------------------------------------------------------
// 音源関連
//---------------------------------------------------------------
constexpr int DEFAULT_SOUND_VOLUME = 200;    //!< 初期音量

// フレームレート（生成タイミング変換に使用）
constexpr int FRAME_RATE = 60;    //!< ゲームの想定フレームレート (fps)

//---------------------------------------------------------------
// ホールド関連定数
//---------------------------------------------------------------
constexpr int HOLD_SCORE_PER_TICK = 5;    //!< ホールド継続時の定期加点量

//---------------------------------------------------------------
// カウントダウン関連
//---------------------------------------------------------------
constexpr int         COUNTDOWN_SECONDS = 3;                                 //!< 開始/再開時のカウントダウン秒数
constexpr int         COUNTDOWN_FRAMES  = FRAME_RATE * COUNTDOWN_SECONDS;    //!< カウントダウンのフレーム数
constexpr const char* COUNTDOWN_SE_PATH = "data/Sound/SE/countdown.mp3";     //!< カウントダウンSEのパス

//---------------------------------------------------------------
// ポーズ画面関連
//---------------------------------------------------------------
constexpr int PAUSE_BOX_W           = 800;    //!< ポーズ表示の矩形幅
constexpr int PAUSE_BOX_H           = 300;    //!< ポーズ表示の矩形高さ
constexpr int PAUSE_TEXT_Y_ADJUST   = -50;    //!< ポーズ大見出しのY座標調整（矩形中央からのオフセット）
constexpr int PAUSE_SUBTEXT_Y_GAP   = 30;     //!< サブテキストと大見出しのYギャップ
constexpr int PAUSE_INFO_FIRST_GAP  = 10;     //!< 操作案内とサブテキストの間隔
constexpr int PAUSE_INFO_SECOND_GAP = 6;      //!< 操作案内行同士の垂直間隔
// ポーズ画面で表示する文字列
constexpr const char* PAUSE_TEXT_STR            = "PAUSED";
constexpr const char* PAUSE_SUBTEXT_STR         = "もう一度ESCキーを押してゲームに戻る";
constexpr const char* PAUSE_INFO_TITLE_STR      = "Tキーでタイトルに戻る";
constexpr const char* PAUSE_INFO_SONGSELECT_STR = "Sキーで曲選択に戻る";

// SongSelectScene 用定数（UI レイアウトのマジックナンバー）
constexpr int HINT_X               = 80;     //!< ヒント表示 X
constexpr int HINT_MARGIN_BOTTOM   = 100;    //!< ヒント Y 座標の下マージン
constexpr int SELECT_ARROW_OFFSET  = 20;     //!< 選択矢印の X オフセット
constexpr int VOLUME_MARGIN_RIGHT  = 20;     //!< ボリューム表示の右マージン
constexpr int VOLUME_MARGIN_BOTTOM = 20;     //!< ボリューム表示の下マージン
constexpr int VOLUME_BOX_PADDING   = 8;      //!< ボリューム表示背景のパディング
