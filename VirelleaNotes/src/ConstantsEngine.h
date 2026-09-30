#pragma once
//============================================================================
//! @file   ConstantsEngine.h
//! @brief  エンジン領域（ゲーム内容に依存しない）の定数定義
//! @details
//!  画面設定・シェーダー登録名/パス・レンダーターゲット名など、
//!  「どんなゲームを作っても共通して使う」エンジン側の定数はここに集約する。
//!  ゲーム固有の定数（時間換算・数学定数など）は ConstantsGame.h、
//!  リズムゲーム固有の定数（ノーツ・判定・UIなど）は ConstantsRhythm.h に置くこと。
//! @author レオ
//============================================================================

// --- 画面関連定数 ---
constexpr int   SCREEN_W = 1280;                    //!< 画面幅
constexpr int   SCREEN_H = 720;                      //!< 画面高さ
constexpr float HALF = 0.5f;                         //!< 半分の値
constexpr float TWICE = 2.0f;                        //!< 2倍の値
constexpr float HALF_SCREEN_W = SCREEN_W * HALF;     //!< 画面幅の半分
constexpr float HALF_SCREEN_H = SCREEN_H * HALF;     //!< 画面高さの半分

// 背景色
constexpr int BACKGROUND_R = 35;    //!< 背景色の赤成分
constexpr int BACKGROUND_G = 41;    //!< 背景色の緑成分
constexpr int BACKGROUND_B = 70;    //!< 背景色の青成分

// --- オブジェクト名（ObjectBase::SetName() で使う文字列。GetSceneObject<T>() 等の検索キー） ---
constexpr const char* OBJECT_NAME_CAMERA = "CameraObject";    //!< カメラ

// --- シーン遷移のフェード（Scene::SceneManager で使用） ---
constexpr int FADE_FRAMES_TOTAL = 30;    //!< フェードアウト／フェードインそれぞれにかけるフレーム数
constexpr int FADE_MAX_ALPHA    = 255;   //!< フェード時の最大アルファ値（255で完全な黒）

// --- シェーダー関連定数（Graphics::ShaderManager で使用） ---
// --- ポストエフェクト（画面全体に一括で適用されるシェーダー） ---
constexpr const char* SHADER_NAME_POSTPROCESS        = "PostProcess";
constexpr const char* SHADER_PATH_POSTPROCESS_VERTEX = "data/Shader/PostProcess_VS";
constexpr const char* SHADER_PATH_POSTPROCESS_PIXEL  = "data/Shader/PostProcess_PS";

// --- トゥーンシェーディング（ボール・ブロック等の3Dオブジェクトに個別適用） ---
constexpr const char* SHADER_NAME_TOON        = "Toon";
constexpr const char* SHADER_PATH_TOON_VERTEX = "data/Shader/Toon_VS";
constexpr const char* SHADER_PATH_TOON_PIXEL  = "data/Shader/Toon_PS";

// GeometryPass の描画先 ⇔ PostProcessPass の読み込み元で共有するレンダーターゲット名
constexpr const char* RENDER_TARGET_SCENE_COLOR = "SceneColor";

//---------------------------------------------------------------
// シャドウマップ（Graphics::ShadowMapManager で使用）関連定数
//! @details DxLibが提供するシャドウマップ機能（MakeShadowMap 等）を使用する。
//!          ライトの向き・描画範囲などゲーム世界に依存する値は
//!          ゲーム固有の定数ヘッダ側に置く。
//---------------------------------------------------------------
constexpr int SHADOW_MAP_SIZE = 2048;    //!< シャドウマップの解像度（縦横とも2のn乗である必要がある）

//---------------------------------------------------------------
// ポストエフェクトの調整パラメータ（Graphics::PostProcessParams で使用）
//! @details PostProcess_PS.fx の cbuffer cbUserPostProcess (register b3) と対応する。
//!          DxLib が b0～b3 を使用するため、独自の定数バッファは b4 以降を使う。
//!          （b4: ポストエフェクト / b5: トゥーンマテリアル）
//---------------------------------------------------------------
constexpr int   POSTPROCESS_CBUFFER_SLOT             = 4;       //!< ピクセルシェーダーの定数バッファ番号（register(b4)）
constexpr int   TOON_CBUFFER_SLOT                    = 5;       //!< トゥーンマテリアルの定数バッファ番号（register(b5)）
constexpr float POSTPROCESS_DEFAULT_SHADOW_THRESHOLD = 0.5f;    //!< 影の閾値の初期値（現在シェーダーに直書きしている値に合わせること）
constexpr float POSTPROCESS_DEFAULT_COLOR_LEVELS     = 8.0f;    //!< 減色レベル（1チャンネルあたりの階調数）の初期値（同上）
