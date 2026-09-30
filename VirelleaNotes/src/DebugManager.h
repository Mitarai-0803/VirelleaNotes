#pragma once
//============================================================================
//! @file   DebugManager.h
//! @brief  デバッグモード（F3）管理クラスの宣言
//! @details F3キーでデバッグ表示のON/OFFを切り替え、ImGuiによる値確認用の
//!          ウィンドウ描画（NewFrame〜Render）を一元管理するシングルトン。
//!          当たり判定のワイヤーフレーム表示は CollisionComponent::DrawDebug()
//!          （SceneBase経由）で行い、本クラスはImGui側の初期化・入力伝達・
//!          描画のみを担当する。
//!
//!          メインウィンドウ "Debug (F3)" には以下を表示する。
//!           - Performance : FPS・フレーム時間（直近フレームの平均・最大・グラフ）
//!           - Shader      : ポストエフェクトの調整スライダー（影の閾値・減色レベル）
//!           - Physics Bodies : 物理ボディの座標・速度
//!          ゲーム状態など、シーン固有の情報は SceneBase::SceneDrawImGui() で
//!          別ウィンドウとして描画する（DebugManagerBeginFrame() 〜 EndFrame() の間）。
//! @author レオ
//============================================================================
#include <Windows.h>
#include "ConstantsDebug.h"
#include <array>

//-----------------------------------------------------------
//! @class DebugManager
//! @brief F3デバッグモードとImGui描画を管理するシングルトン
//-----------------------------------------------------------
class DebugManager
{
public:
    //! @return 唯一のインスタンス
    static DebugManager& GetInstance()
    {
        static DebugManager instance;
        return instance;
    }

    DebugManager(const DebugManager&)            = delete;
    DebugManager& operator=(const DebugManager&) = delete;

    //-----------------------------------------------------------
    //! @brief ImGuiコンテキストと描画バックエンドの初期化
    //! @details DxLib_Init() の後（Direct3Dデバイス生成後）に呼ぶこと。
    //! @return true 初期化成功
    //-----------------------------------------------------------
    bool DebugManagerInit();

    //-----------------------------------------------------------
    //! @brief 入力更新とフレーム時間の計測
    //! @details InputManager::Update() の後、毎フレーム呼ぶこと。
    //!          F3キーでデバッグモードをトグルする。フレーム時間は
    //!          デバッグモードOFF中も計測を続けるため、ONにした直後から
    //!          正しいFPS・グラフが表示される。
    //-----------------------------------------------------------
    void DebugManagerUpdate();

    //-----------------------------------------------------------
    //! @brief ImGui描画フレーム開始とメインウィンドウの描画
    //! @details シーンの Update()/Draw() が終わった後、ScreenFlip() より前に呼ぶこと。
    //!          true が返った場合のみ、シーン側のImGuiウィンドウ描画
    //!          （SceneManager::DrawImGui()）を行ってよい。
    //! @return true ならImGuiフレームを開始した（この後 DebugManagerEndFrame() を必ず呼ぶこと）
    //-----------------------------------------------------------
    bool DebugManagerBeginFrame();

    //-----------------------------------------------------------
    //! @brief ImGui描画フレーム終了（ドローコールを積む）
    //! @details ScreenFlip() より前、ClearDrawScreen()後の3D/2D描画がすべて
    //!          終わった後に呼ぶこと。
    //-----------------------------------------------------------
    void DebugManagerEndFrame();

    //! @brief 終了処理（ImGuiコンテキスト破棄）
    void DebugManagerEnd();

    //! @return true ならデバッグモード有効（当たり判定表示・ImGui表示中）
    bool IsEnabled() const { return m_enabled; }

    //! @return true ならImGuiが初期化済みかつデバッグモード有効（ImGuiの描画コマンドを発行してよい）
    bool IsFrameActive() const { return m_initialized && m_enabled; }

private:
    DebugManager()  = default;
    ~DebugManager() = default;

    //! @brief 前回呼び出しからの経過時間を測り、フレーム時間の履歴へ追加する
    void UpdateFrameTime();

    //! @brief FPS・フレーム時間パネルを描画する
    void DrawPerformancePanel();

    //! @brief ポストエフェクト（影の閾値・減色レベル）の調整パネルを描画する
    void DrawShaderPanel();

    //! @brief 物理ボディの座標・速度パネルを描画する
    void DrawBodyPanel();

    bool m_initialized = false;    //!< Init() 済みか
    bool m_enabled     = false;    //!< デバッグモード有効フラグ（F3でトグル）

    LONGLONG m_last_time_us = 0;       //!< 前回 UpdateFrameTime() を呼んだ時刻（マイクロ秒。0は未計測）
    float    m_frame_time_ms = 0.0f;    //!< 直近1フレームの所要時間（ミリ秒）

    std::array<float, DEBUG_FRAME_HISTORY_SIZE> m_frame_history {};    //!< フレーム時間の履歴（リングバッファ。ミリ秒）
    int m_frame_history_index = 0;                                       //!< 次に書き込む履歴位置（＝最も古い要素の位置）
    int m_frame_history_count = 0;                                       //!< 履歴に溜まっている有効なサンプル数
};
