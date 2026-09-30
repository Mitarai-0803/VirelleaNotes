//============================================================================
//! @file   DxMain.cpp
//! @brief  アプリケーションエントリおよびメインループ
//! @details アプリケーションのエントリポイントとメインループを実装します。
//! @author レオ
//============================================================================
#include "DxMain.h"             //!< 定数利用
#include "TitleScene.h"         //!< タイトルシーン
#include "SongSelectScene.h"    //!< 曲選択シーン
#include "GameScene.h"          //!< プレイシーン
#include "ResultScene.h"        //!< リザルトシーン
#include "SceneManager.h"       //!< シーンマネージャ
#include "InputManager.h"       //!< 入力管理
#include "Font.h"               //!< フォント管理
#include "ConstantsGame.h"      //!< ゲーム用定数ファイル
#include "ConstantsRhythm.h"    //!< リズムゲーム用定数ファイル（シーン登録名など）
#include "PhysicsManager.h"     //!< 物理マネージャー初期化関数
#include "DebugManager.h"       //!< F3デバッグモード（当たり判定・ImGui値確認）管理
#include <cstdlib>
#include <ctime>

using namespace std;

bool Application::Init()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));    //!< 乱数シード初期化

    SetOutApplicationLogValidFlag(FALSE);                            //!< ログ出力をしない
    SetAlwaysRunFlag(TRUE);                                          //!< ウィンドウが非アクティブな状態でも処理を続行する
    ChangeWindowMode(FALSE);                                         //!< フルスクリーンに設定（TRUE でウィンドウモード）
    SetBackgroundColor(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);    //!< 背景色設定
    SetGraphMode(SCREEN_W, SCREEN_H, 32);                            //!< 画面サイズと色深度設定
    SetWindowTextDX("VirelleaNotes");                                //!< ウィンドウテキスト設定
    SetWindowIconID(101);                                            //!< アイコン設定（リソースID）

    // 独自シェーダー（Shader/ShaderManager、ポストエフェクト等）や ImGui は DxLib の
    // DirectX11 モードを前提にしているため、DX11 で初期化する（未指定時のデフォルトは DX9）。
    SetUseDirect3DVersion(DX_DIRECT3D_11);

    // DxLib初期化
    if(DxLib_Init() == -1)
        return false;    //!< 初期化失敗

    SetDrawScreen(DX_SCREEN_BACK);    //!< 裏画面に描画（ちらつき防止）

    // Jolt初期化
    if(!PhysicsManager::GetInstance().PhysicsManagerInit()) {
        return false;    //!< 初期化失敗
    }

    // デバッグ機能初期化（F3で当たり判定・ImGui値確認をトグル表示）
    // ※ ImGui用バックエンド（imgui_impl_dx11）が未追加、またはDirectX11モードで
    //    ない場合はfalseが返るが、デバッグ機能が使えないだけでゲーム自体は続行する。
    //    （失敗理由は出力ウィンドウに "[DebugManager] ..." として表示される）
    DebugManager::GetInstance().DebugManagerInit();

    // フォント初期化
    Font::FontInit();

    // シーン登録
    // ※ 名前と型を対応付けるだけで、この時点ではまだインスタンス化しない。
    //    ChangeSceneImmediate() / ChangeScene() を呼んだ時点で生成される。
    m_scene_manager.RegisterScene<Scene::TitleScene>(Rhythm::SceneName::TITLE);
    m_scene_manager.RegisterScene<Scene::SongSelectScene>(Rhythm::SceneName::SONG_SELECT);
    m_scene_manager.RegisterScene<Scene::GameScene>(Rhythm::SceneName::GAME);
    m_scene_manager.RegisterScene<Scene::ResultScene>(Rhythm::SceneName::RESULT);

    // 起動時の最初のシーンを設定する
    // ※ Title → SongSelect → Game → Result → SongSelect … の順に画面内のキー操作で遷移する。
    m_scene_manager.ChangeSceneImmediate(Rhythm::SceneName::TITLE);

    return true;
}

void Application::Update()
{
    // メインループ：DELETEキーが押されるまで続ける（ESCキーはゲーム内のポーズに使用するため終了には使わない）
    while(ProcessMessage() == 0 && !InputManager::PushHitKey(KEY_INPUT_ESCAPE)) {
        ClearDrawScreen();    //!< 描画バッファクリア

        InputManager::Update();    //!< 入力更新

        DebugManager::GetInstance().DebugManagerUpdate();    //!< F3でデバッグモードをトグル

        PhysicsManager::GetInstance().PhysicsManagerUpdate();    //!< Jolt物理シミュレーション更新

        m_scene_manager.Update();    //!< 現在シーンの更新（保留中のシーン切り替えもここで進められる）
        m_scene_manager.Draw();      //!< 現在シーンの描画

        // デバッグモード中のみデバッグ描画
        if(DebugManager::GetInstance().IsEnabled()) {
            m_scene_manager.DrawDebug();
        }

        // ImGui描画（NewFrame〜Renderまでをここでまとめて行う。ScreenFlip()より前に呼ぶこと）
        // BeginFrame() が true の間だけ、シーン固有のImGuiウィンドウを積む。
        if(DebugManager::GetInstance().DebugManagerBeginFrame()) {
            m_scene_manager.DrawImGui();
        }
        DebugManager::GetInstance().DebugManagerEndFrame();

        ScreenFlip();    //!< バッファ反映
    }
}

void Application::End()
{
    m_scene_manager.EndCurrent();    //!< 現在シーンの終了処理
    Font::FontExit();                //!< フォント終了

    // デバッグ機能終了
    DebugManager::GetInstance().DebugManagerEnd();

    // Jolt終了
    PhysicsManager::GetInstance().PhysicsManagerEnd();

    DxLib_End();    //!< DXライブラリ終了
}

//-----------------------------------------------------------
//! @brief アプリケーションエントリポイント
//! @return 終了コード（0:正常終了、-1:初期化失敗）
//-----------------------------------------------------------
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Application app;

    // 初期化に失敗した場合はエラーコード -1 を返して終了
    if(!app.Init())
        return -1;

    // 初期化成功した場合はメインループを実行して正常終了
    app.Update();

    // 終了処理
    app.End();

    return 0;
}
