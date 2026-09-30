//============================================================================
//! @file   DebugManager.cpp
//! @brief  デバッグモード（F3）管理クラスの実装
//! @details
//!  ImGuiの入力連携について:
//!  DXライブラリはウィンドウプロシージャを内部で保持しており、外部から
//!  WndProcをフックする公式な手段が無いため、本実装では imgui_impl_win32 は
//!  使用せず、毎フレーム DxLib / InputManager から取得したマウス座標・
//!  ボタン状態・ホイール量を ImGuiIO に直接書き込む方式にしている。
//!  そのため文字入力を伴うテキストボックス等は現状未対応（値の確認・
//!  チェックボックス・スライダー等の操作は問題なく動作する）。
//!
//!  レンダリングには公式の Dear ImGui バックエンド一式に含まれる
//!  imgui_impl_dx11.h / .cpp を使用する。プロジェクトに未追加の場合は
//!  Dear ImGui配布物の backends フォルダから追加すること。
//!  ※ 本プロジェクトは DxMain.cpp で SetUseDirect3DVersion(DX_DIRECT3D_11) により
//!    DirectX11モードで動作しているため、DX9用バックエンド（imgui_impl_dx9）では
//!    デバイスが取得できず ImGui が表示されない。必ずDX11用を使うこと。
//!
//!  DxLibとの描画の共存について:
//!  DxLibは頂点を内部バッファに溜めてから描画し、Direct3D11の設定も自前で
//!  キャッシュしている。そのため ImGui（生のD3D11描画）の前に RenderVertex() で
//!  溜まった描画を吐き出し、後に RefreshDxLibDirect3DSetting() でDxLib側の
//!  設定を復帰させている（DxLib公式が他ライブラリ併用時に推奨している手順）。
//!
//!  ImGuiの表示文字列について:
//!  ImGui標準フォントは日本語グリフを含まないため、ラベル・値の書式は
//!  ASCII文字のみで書くこと（日本語を渡すと「?」表示になる）。
//! @author レオ
//============================================================================
#include "DebugManager.h"
#include "InputManager.h"
#include "PhysicsManager.h"
#include "CollisionComponent.h"
#include "PostProcessParams.h"
#include "ConstantsGame.h"
#include "ConstantsDebug.h"
#include <DxLib.h>
#include <d3d11.h>
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <algorithm>

namespace {
constexpr int DEBUG_TOGGLE_KEY = KEY_INPUT_F3;    //!< デバッグモード切替キー
}

bool DebugManager::DebugManagerInit()
{
    if(m_initialized)
        return true;

    // DxLib::GetUseDirect3D11Device() 等の戻り値は const void* で定義されているため、
    // ID3D11Device / ID3D11DeviceContext へ明示的にキャストする
    ID3D11Device*        device  = const_cast<ID3D11Device*>(reinterpret_cast<const ID3D11Device*>(DxLib::GetUseDirect3D11Device()));
    ID3D11DeviceContext* context = const_cast<ID3D11DeviceContext*>(reinterpret_cast<const ID3D11DeviceContext*>(DxLib::GetUseDirect3D11DeviceContext()));
    if(!device || !context) {
        // DirectX11モードでない場合はここに到達する（上部コメント参照）
        OutputDebugStringA("[DebugManager] Direct3D11 device/context を取得できませんでした。ImGuiは無効になります。\n");
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io      = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.DisplaySize   = ImVec2(static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H));

    ImGui::StyleColorsDark();

    if(!ImGui_ImplDX11_Init(device, context)) {
        OutputDebugStringA("[DebugManager] ImGui_ImplDX11_Init に失敗しました。\n");
        ImGui::DestroyContext();
        return false;
    }

    m_initialized = true;
    return true;
}

void DebugManager::DebugManagerUpdate()
{
    if(InputManager::PushHitKey(DEBUG_TOGGLE_KEY)) {
        m_enabled = !m_enabled;
    }

    // デバッグモードOFF中も計測しておく（ON直後から履歴が揃っている状態にするため）
    UpdateFrameTime();
}

//-----------------------------------------------------------
//! @brief 前回呼び出しからの経過時間を測り、フレーム時間の履歴へ追加する
//! @details ミリ秒精度の GetNowCount() では 16ms/17ms の丸め誤差が目立つため、
//!          マイクロ秒精度の GetNowHiPerformanceCount() を使う。
//-----------------------------------------------------------
void DebugManager::UpdateFrameTime()
{
    const LONGLONG now_us = DxLib::GetNowHiPerformanceCount();

    if(m_last_time_us != 0) {
        m_frame_time_ms                        = static_cast<float>(now_us - m_last_time_us) * DEBUG_US_TO_MS;
        m_frame_history[m_frame_history_index] = m_frame_time_ms;
        m_frame_history_index                  = (m_frame_history_index + 1) % DEBUG_FRAME_HISTORY_SIZE;
        m_frame_history_count                  = std::min(m_frame_history_count + 1, DEBUG_FRAME_HISTORY_SIZE);
    }

    m_last_time_us = now_us;
}

//-----------------------------------------------------------
//! @brief ImGui描画フレーム開始とメインウィンドウの描画
//! @return true ならImGuiフレームを開始した
//-----------------------------------------------------------
bool DebugManager::DebugManagerBeginFrame()
{
    if(!IsFrameActive())
        return false;

    ImGuiIO& io = ImGui::GetIO();

    // DeltaTime（0除算防止のため下限を設ける）
    io.DeltaTime = std::max(m_frame_time_ms * DEBUG_MS_TO_SEC, DEBUG_MIN_DELTA_TIME);

    // マウス状態をDxLib/InputManager経由でImGuiへ反映
    io.MousePos     = ImVec2(static_cast<float>(InputManager::GetMouseX()), static_cast<float>(InputManager::GetMouseY()));
    io.MouseDown[0] = InputManager::CheckMouseInput(MOUSE_INPUT_LEFT);
    io.MouseDown[1] = InputManager::CheckMouseInput(MOUSE_INPUT_RIGHT);
    io.MouseWheel   = static_cast<float>(InputManager::GetMouseWheel());

    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();

    // --- メインウィンドウ：FPS・シェーダー調整・物理ボディの値確認 ---
    ImGui::SetNextWindowPos(ImVec2(DEBUG_IMGUI_MAIN_WINDOW_POS_X, DEBUG_IMGUI_MAIN_WINDOW_POS_Y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(DEBUG_IMGUI_WINDOW_WIDTH, 0.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug (F3)");

    DrawPerformancePanel();
    DrawShaderPanel();
    DrawBodyPanel();

    ImGui::End();

    return true;
}

void DebugManager::DrawPerformancePanel()
{
    if(!ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    // 直近フレームの平均・最大を求める（履歴が溜まっている分だけ）
    float sum_ms = 0.0f;
    float max_ms = 0.0f;
    for(int i = 0; i < m_frame_history_count; ++i) {
        sum_ms += m_frame_history[i];
        max_ms  = std::max(max_ms, m_frame_history[i]);
    }
    const float avg_ms = (m_frame_history_count > 0) ? sum_ms / static_cast<float>(m_frame_history_count) : 0.0f;
    const float fps    = (avg_ms > 0.0f) ? DEBUG_MS_PER_SEC / avg_ms : 0.0f;

    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame: %.2f ms", m_frame_time_ms);
    ImGui::Text("Avg: %.2f ms  Max: %.2f ms", avg_ms, max_ms);

    // 古い順に並ぶよう、次の書き込み位置（＝最古の要素）をオフセットに指定する
    ImGui::PlotLines("##frame_time", m_frame_history.data(), DEBUG_FRAME_HISTORY_SIZE, m_frame_history_index,
                     nullptr, 0.0f, DEBUG_FRAME_TIME_PLOT_MAX_MS, ImVec2(0.0f, DEBUG_FRAME_TIME_PLOT_HEIGHT));
}

void DebugManager::DrawShaderPanel()
{
    if(!ImGui::CollapsingHeader("Shader", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    auto& params    = Graphics::PostProcessParams::GetInstance();
    auto& constants = params.GetConstants();

    if(!params.IsValid()) {
        // 定数バッファ未作成だと、スライダーを動かしてもシェーダーへ値が届かない
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Constant buffer not created");
    }

    ImGui::SliderFloat("Shadow Threshold", &constants.ShadowThreshold, DEBUG_SHADOW_THRESHOLD_MIN, DEBUG_SHADOW_THRESHOLD_MAX, "%.2f");

    // 減色レベルは階調数（整数）として扱う。GPUへはfloatのまま渡す
    int color_levels = static_cast<int>(constants.ColorLevels + DEBUG_COLOR_LEVELS_ROUND);
    if(ImGui::SliderInt("Color Levels", &color_levels, DEBUG_COLOR_LEVELS_MIN, DEBUG_COLOR_LEVELS_MAX)) {
        constants.ColorLevels = static_cast<float>(color_levels);
    }

    if(ImGui::Button("Reset##shader")) {
        params.ResetToDefault();
    }
}

void DebugManager::DrawBodyPanel()
{
    if(!ImGui::CollapsingHeader("Physics Bodies"))
        return;

    auto& all_components = PhysicsManager::GetInstance().GetBodyManager().GetAll();
    ImGui::Text("Registered bodies: %d", static_cast<int>(all_components.size()));
    ImGui::Separator();

    int index = 0;
    for(const auto& [body_id, component] : all_components) {
        if(!component)
            continue;

        JPH::RVec3 pos = component->GetPosition();
        JPH::Vec3  vel = component->GetLinearVelocity();

        ImGui::Text("Body[%d] id=%u", index, body_id.GetIndexAndSequenceNumber());
        ImGui::Text("  pos: (%.2f, %.2f, %.2f)", pos.GetX(), pos.GetY(), pos.GetZ());
        ImGui::Text("  vel: (%.2f, %.2f, %.2f)", vel.GetX(), vel.GetY(), vel.GetZ());
        ++index;
    }
}

void DebugManager::DebugManagerEndFrame()
{
    if(!IsFrameActive())
        return;

    ImGui::Render();

    RenderVertex();    //!< DxLibが溜め込んでいる描画を先に吐き出す（ImGuiより前に描かれるべきものを確定させる）
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    RefreshDxLibDirect3DSetting();    //!< 生のD3D11描画で変わった設定をDxLib側へ復帰させる
}

void DebugManager::DebugManagerEnd()
{
    if(!m_initialized)
        return;

    ImGui_ImplDX11_Shutdown();
    ImGui::DestroyContext();
    m_initialized = false;
}
