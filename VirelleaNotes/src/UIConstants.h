//----------------------------------------------------------------------------
//! @file   UIConstants.h
//! @brief  UI 関連の定数定義
//! @detail 曲選択画面や各種 UI のレイアウトに用いる定数を定義。
//!         描画位置やサイズ、オフセットなどを集中管理する。
//----------------------------------------------------------------------------
#pragma once
#include "ConstantsRhythm.h"

//---------------------------------------------------------------
// サウンドヴィジュアライザー関連
//---------------------------------------------------------------
constexpr float SOUNDL__MAX_W      = 200.0f;             //!< サウンドヴィジュアライザーの幅(最大幅)
constexpr float SOUNDL_H           = 20.0f;              //!< サウンドヴィジュアライザーの高さ
constexpr int   BUFFER_LENGTH      = 14;                 //!< サウンドヴィジュアライザーの線の数
constexpr int   LENGTH_SPACE       = 30;                 //!< サウンドヴィジュアライザーの線の間隔(縦)
constexpr int   SOUND_VISUALIZER_X = SCREEN_W * HALF;    //!< サウンドヴィジュアライザーのX座標
constexpr int   SOUND_VISUALIZER_Y = 150.0f;             //!< サウンドヴィジュアライザーのY座標

// 遊び方 表示関連
constexpr int HOWTO_BOX_W              = 900;    //!< 遊び方ダイアログの幅
constexpr int HOWTO_BOX_H              = 420;    //!< 遊び方ダイアログの高さ
constexpr int HOWTO_TEXT_X_OFFSET      = 40;     //!< 遊び方テキストのXオフセット
constexpr int HOWTO_TEXT_Y_OFFSET      = 30;     //!< 遊び方テキストのYオフセット
constexpr int HOWTO_LINE_GAP_MEDIUM    = 10;     //!< 遊び方テキスト中の中間ギャップ
constexpr int HOWTO_LINE_GAP_SMALL     = 8;      //!< 遊び方テキスト中の小ギャップ
constexpr int HOWTO_LINE_GAP_LARGE     = 16;     //!< 遊び方テキスト中の大ギャップ
constexpr int HOWTO_IMAGE_MARGIN_RIGHT = 20;     //!< 遊び方画像の右側余白

// ブレンド / アルファ値
constexpr int DRAW_BLEND_ALPHA_SEMI = 200;    //!< 半透明ブレンドのアルファ値
constexpr int DRAW_BLEND_ALPHA_FULL = 255;    //!< 不透明ブレンドのアルファ値

// カウントダウン背景関連
constexpr int COUNTDOWN_BG_ALPHA = 160;    //!< カウントダウン背景の半透明アルファ
constexpr int COUNTDOWN_BG_PAD_X = 20;     //!< カウントダウン背景の左右パディング
constexpr int COUNTDOWN_BG_PAD_Y = 10;     //!< カウントダウン背景の上下パディング

// ラベル描画の微調整
constexpr int NOTE_DEBUG_LABEL_Y_OFFSET = 16;    //!< デバッグラベルのYオフセット
constexpr int KEY_LABEL_Y_OFFSET        = 32;    //!< ライン上に表示するキーラベルのYオフセット
constexpr int KEY_LABEL_X_ADJUST        = 8;     //!< ライン上に表示するキーラベルのX微調整

// 音量表示関連
constexpr int VOLUME_DISPLAY_SECONDS       = 2;     //!< 音量表示の継続秒数
constexpr int VOLUME_DISPLAY_MARGIN_RIGHT  = 20;    //!< 音量表示の右余白
constexpr int VOLUME_DISPLAY_MARGIN_BOTTOM = 20;    //!< 音量表示の下余白
constexpr int VOLUME_DISPLAY_EXTRA_Y       = 40;    //!< 音量表示の追加Yオフセット（衝突回避用）
constexpr int VOLUME_DISPLAY_PADDING       = 8;     //!< 音量表示ボックスのパディング

//---------------------------------------------------------------
// 描画文字列関連
//---------------------------------------------------------------
//現在時間表示文字列
static constexpr const char* TIME_STR   = "{minutes:%02d}:{seconds:%02d}";    //<! 現在時間表示文字列
static constexpr const char* TIME_STR_W = "%02d:%02d";                        //<! 現在時間表示文字列(幅取得用)
//パラメーター
constexpr const char* PARAMETER_MINUTES = "minutes";    //!< 秒
constexpr const char* PARAMETER_SECONDS = "seconds";    //!< 分
//座標
constexpr float TIME_STR_X = SCREEN_W * HALF;      //!< 現在時間表示文字列のX座標
constexpr float TIME_STR_Y = SCREEN_H - 100.0f;    //!< 現在時間表示文字列のY座標

//スコア表示文字列
static constexpr const char* SCORE_STR   = "SCORE:{score:%05d}";    //!< スコア表示文字列
static constexpr const char* SCORE_STR_W = "SCORE:%05d";            //!< スコア表示文字列(幅取得用)
//パラメーター
constexpr const char* PARAMETER_SCORE = "score";    //!< スコア
//座標
constexpr float SCORE_STR_X = SCREEN_W * HALF;    //!< スコア表示のX座標
constexpr float SCORE_STR_Y = 50.0f;              //!< スコア表示のY座標

//リザルト画面関連
//リザルトタイトル
static constexpr const char* RESULT_TITLE_STR = "RESULT";           //!< リザルトタイトル文字列
constexpr float              RESULT_TITLE_X   = SCREEN_W * HALF;    //!< リザルトタイトルのX座標
constexpr float              RESULT_TITLE_Y   = 100.0f;             //!< リザルトタイトルのY座標

//判定結果表示文字列
static constexpr const char* RESULT_JUDG_STR_PERFECT = "PERFECT : {perfect:%03d}";    //!< 判定の文字列(PERFECT)
static constexpr const char* RESULT_JUDG_STR_GOOD    = "GOOD    : {good:%03d}";       //!< 判定の文字列(GOOD)
static constexpr const char* RESULT_JUDG_STR_BAD     = "BAD     : {bad:%03d}";        //!< 判定の文字列(BAD)
static constexpr const char* RESULT_JUDG_STR_MISS    = "MISS    : {miss:%03d}";       //!< 判定の文字列(MISS)
static constexpr const char* RESULT_JUDG_STR_SCORE   = "SCORE   : {score:%05d}";      //!< 判定の文字列(SCORE)

//幅取得用フォーマット
static constexpr const char* RESULT_JUDG_STR_W_PERFECT = "PERFECT : %03d";    //!< 判定の文字列(PERFECT)幅取得用
static constexpr const char* RESULT_JUDG_STR_W_GOOD    = "GOOD    : %03d";    //!< 判定の文字列(GOOD)幅取得用
static constexpr const char* RESULT_JUDG_STR_W_BAD     = "BAD     : %03d";    //!< 判定の文字列(BAD)幅取得用
static constexpr const char* RESULT_JUDG_STR_W_MISS    = "MISS    : %03d";    //!< 判定の文字列(MISS)幅取得用
static constexpr const char* RESULT_JUDG_STR_W_SCORE   = "SCORE   : %05d";    //!< 判定の文字列(SCORE)幅取得用

//パラメーター
static constexpr const char* PARAMETER_JUDGE_PERFECT = "perfect";    //!< PERFECT
static constexpr const char* PARAMETER_JUDGE_GOOD    = "good";       //!< GOOD
static constexpr const char* PARAMETER_JUDGE_BAD     = "bad";        //!< BAD
static constexpr const char* PARAMETER_JUDGE_MISS    = "miss";       //!< MISS

//座標とレイアウト
constexpr float RESULT_JUDG_START_X     = 150.0f;    //!< 判定文字列の開始X座標
constexpr float RESULT_JUDG_START_Y     = 220.0f;    //!< 判定文字列の開始Y座標
constexpr float RESULT_JUDG_LINE_HEIGHT = 50.0f;     //!< 判定文字列の行間
constexpr float RESULT_JUDG_SECTION_GAP = 50.0f;     //!< ライン間のセクション間隔

constexpr float RESULT_TOTAL_START_Y = 500.0f;    //!< 合計表示の開始Y座標

//操作ガイド
static constexpr const char* RESULT_GUIDE_STR = "SPACE:曲選択に戻る";    //!< リザルト操作ガイド文字列
constexpr float              RESULT_GUIDE_X   = SCREEN_W * HALF;         //!< 操作ガイドのX座標
constexpr float              RESULT_GUIDE_Y   = SCREEN_H - 130.0f;       //!< 操作ガイドのY座標

// タイトル画面関連
// 開始ガイド
static constexpr const char* TITLE_GUIDE_STR = "Spaceキーを押して開始";     //!< タイトル画面の操作ガイド文字列
constexpr float              TITLE_GUIDE_X   = SCREEN_W * HALF - 150.0f;    //!< タイトル操作ガイドのX座標
constexpr float              TITLE_GUIDE_Y   = SCREEN_H - 170.0f;           //!< タイトル操作ガイドのY座標

// 遊び方ガイド
static constexpr const char* HOWTO_GUIDE_STR = "Hキーを押して遊び方を確認";    //!< 遊び方画面の操作ガイド文字列
constexpr float              HOWTO_GUIDE_X   = SCREEN_W * HALF - 160.0f;       //!< 遊び方ガイドのX座標
constexpr float              HOWTO_GUIDE_Y   = SCREEN_H - 130.0f;              //!< 遊び方ガイドのY座標

//---------------------------------------------------------------
// ボタン関係
//---------------------------------------------------------------
constexpr int   BUTTON_NUMBER           = 2;          //!< ボタンの数
constexpr float BUTTON_HOVER_SCALE      = 1.1f;       //!< ボタンホバー時のスケール倍率
constexpr int   BUTTON_Y                = 50;         //!< ボタンのY座標
constexpr int   DATA_SAVE_BUTTON_X      = 200;        //!< データ保存ボタンのX軸オフセット
constexpr float NOTES_TYPE_BTN_X        = 1000.0f;    //!< ノーツのタイプ変更ボタンのX座標
constexpr float NOTES_TYPE_BTN_OFFSET_X = 50.0f;      //!< ノーツのタイプ変更ボタンのX座標

//---------------------------------------------------------------
// 四角描画オブジェクト関連
//---------------------------------------------------------------
constexpr float DRAW_BOX_X = NOTES_LINE_X * 2.0f;    //!< 描画する四角のX座標
constexpr float DRAW_BOX_Y = 0.0f;                   //!< 描画する四角のY座標
constexpr float DRAW_BOX_W = NOTES_LINE_W * 2.0f;    //!< 描画する四角の幅
constexpr float DRAW_BOX_H = SCREEN_H;               //!< 描画する四角の高さ

//---------------------------------------------------------------
// 曲選択画面のレイアウト関連
//---------------------------------------------------------------
constexpr int SONG_SELECT_START_X          = 100;                                        //!< 曲リストの表示開始 X 座標
constexpr int SONG_SELECT_START_Y          = 120;                                        //!< 曲リストの表示開始 Y 座標
constexpr int SONG_LIST_LINE_HEIGHT        = 40;                                         //!< 曲リストの行高さ
constexpr int SONG_INFO_OFFSET_X           = 400;                                        //!< 曲情報表示の X オフセット
constexpr int SONG_ICON_SIZE               = 490;                                        //!< 曲アイコンの表示サイズ（正方形）
constexpr int SONG_ICON_MARGIN_RIGHT       = 20;                                         //!< 画面右端からアイコンまでの余白
constexpr int SONG_INFO_BOX_MARGIN         = 10;                                         //!< 情報ボックスとアイコン間の余白
constexpr int SONG_SELECT_HEADER_X         = 70;                                         //!< 曲選択画面ヘッダ X 座標
constexpr int SONG_SELECT_HEADER_Y         = 40;                                         //!< 曲選択画面ヘッダ Y 座標
constexpr int SONG_SELECT_HIGHLIGHT_OFFSET = 30;                                         //!< 選択時に左に表示するハイライト文字オフセット
constexpr int SONG_INFO_BPM_OFFSET         = 40;                                         //!< BPM 表示の Y オフセット
constexpr int SONG_INFO_SCORE_OFFSET       = 80;                                         //!< スコア表示の Y オフセット
constexpr int SONG_INFO_BOX_PADDING        = 10;                                         //!< 情報ボックスの上下パディング
constexpr int RESULT_ICON_X                = SCREEN_W - 450 - SONG_ICON_MARGIN_RIGHT;    //!< アイコン表示の X 座標
constexpr int RESULT_ICON_Y                = 130;                                        //!< アイコン表示の Y 座標
constexpr int RESULT_ICON_SIZE             = 450;                                        //!< アイコン表示のサイズ

//---------------------------------------------------------------
// 画像 / サウンド パス
//---------------------------------------------------------------
static constexpr const char* SONG_SELECT_BG_IMAGE_PATH = "data/Image/SongSelect/back.png";    //!< 曲選択画面背景画像パス
static constexpr const char* RESULT_BG_IMAGE_PATH      = "data/Image/Result/back.png";        //!< リザルト背景画像パス
static constexpr const char* TITLE_IMAGE_PATH          = "data/Image/Title/title.png";        //!< タイトル背景画像パス
static constexpr const char* TITLE_VIOLIN_IMAGE_PATH   = "data/Image/Title/violin.png";       //!< バイオリン画像パス
static constexpr const char* TITLE_BOW_IMAGE_PATH      = "data/Image/Title/stick.png";        //!< バイオリン弓画像パス
static constexpr const char* TITLE_SE_PATH             = "data/Sound/SE/title_se.mp3";        //!< タイトル効果音パス
static constexpr const char* TITLE_BGM_PATH            = "data/Sound/BGM/title_bgm.mp3";      //!< タイトル BGM パス
static constexpr const char* HOWTO_IMAGE_PATH          = "data/image/howto.png";              //!< 遊び方表示用画像のパス
