#pragma once
//============================================================================
//! @file   SceneManager.h
//! @brief  シーン管理クラスの宣言
//! @details
//!  シーンを名前で登録しておき、ChangeScene（実際にはリクエストの発行）で
//!  「現在のシーン」を切り替えられるようにしたマネージャー。
//!
//!  シーン切り替えの流れ:
//!    現在Sceneの終了処理(End)
//!     ↓
//!    現在Sceneの破棄
//!     ↓
//!    次Scene生成
//!     ↓
//!    次SceneのInit
//!     ↓
//!    CurrentSceneに設定
//!
//!  @note 過去の実装との違いについて
//!  以前の SceneManager は AddScene() で登録した全シーンの Init/Update/Draw/End を
//!  「まとめて」呼び出す設計であり、シーンを1つずつ切り替える仕組みは
//!  存在しなかった（TitleScene / ResultScene 内にも「シーン遷移は
//!  DxMain / SceneManager で別途処理が必要」という実装待ちのコメントが
//!  残っていた）。今回、拡張依頼の Scene 切り替え要件（1つの CurrentScene を
//!  Init/Update/Draw/End する設計）に合わせて、登録は「名前 → 生成方法」の
//!  マッピングとして持ち、実行時は CurrentScene 1つだけを動かす方式に
//!  作り直している。
//! @author レオ
//============================================================================
#include "SceneBase.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace Scene
{

//-----------------------------------------------------------
//! @class SceneManager
//! @brief シーンの登録・現在シーンの切り替え・ライフサイクル呼び出しを行うマネージャ
//! @details 現在シーンの所有権は unique_ptr で管理する。
//-----------------------------------------------------------
class SceneManager
{
public:
    SceneManager()  = default;
    ~SceneManager() = default;

    //-----------------------------------------------------------
    //! @brief シーンを名前付きで登録する
    //! @details 実際のインスタンス生成は ChangeScene() が呼ばれた時点で行う
    //!          （登録時点ではまだ生成しない）
    //! @tparam T 登録するシーンの型（SceneBase の派生型）
    //! @param name シーン名（ChangeScene() で使用する）
    //-----------------------------------------------------------
    template <typename T>
    void RegisterScene(const std::string& name)
    {
        m_factories[name] = []() -> std::unique_ptr<SceneBase> {
            return std::make_unique<T>();
        };
    }

    //-----------------------------------------------------------
    //! @brief シーン切り替えを要求する
    //! @details 即座には切り替えず、次の Update() 冒頭でフェードアウトを開始し、
    //!          画面が暗転しきった時点でシーンを差し替えてフェードインする
    //!          （現在シーンの Update() 実行中に自分自身が破棄されるのを防ぐ効果もある）。
    //!          フェード中に出された要求は保留され、フェード完了後に処理される。
    //!          SceneBase::ChangeScene() から呼ばれることを想定しているが、
    //!          DxMain 等から直接呼んでも良い。
    //! @param name RegisterScene() で登録した名前
    //-----------------------------------------------------------
    void RequestChangeScene(const std::string& name);

    //-----------------------------------------------------------
    //! @brief 現在シーンを即座に切り替える
    //! @details 通常は RequestChangeScene() を使うこと。
    //!          起動直後の最初のシーン設定など、Update() ループの外側から
    //!          安全に呼べる場合にのみ直接使用する。
    //! @param name RegisterScene() で登録した名前
    //! @return true 切り替え成功（未登録の名前の場合は false）
    //-----------------------------------------------------------
    bool ChangeSceneImmediate(const std::string& name);

    //! @brief 保留中のシーン切り替え要求（フェード）を進めてから、現在シーンを更新する
    void Update();

    //! @brief 現在シーンを描画し、フェード中はその上に黒いフェードを重ねる
    void Draw();

    //! @brief 現在シーンのデバッグ描画を行う（F3デバッグモード時のみ呼ぶ想定）
    void DrawDebug();

    //-----------------------------------------------------------
    //! @brief 現在シーンのImGuiウィンドウを描画する
    //! @details DebugManager::DebugManagerBeginFrame() が true を返したフレームのみ、
    //!          EndFrame() より前に呼ぶこと。
    //-----------------------------------------------------------
    void DrawImGui();

    //! @brief 現在シーンの終了処理を行い、破棄する
    void EndCurrent();

    //! @return 現在シーン（未設定なら nullptr）
    SceneBase* GetCurrentScene() const { return m_current_scene.get(); }

    //! @return 現在シーン名（未設定なら空文字列）
    const std::string& GetCurrentSceneName() const { return m_current_scene_name; }

private:
    std::unordered_map<std::string, std::function<std::unique_ptr<SceneBase>()>> m_factories;    //!< 登録済みシーン生成関数

    std::unique_ptr<SceneBase> m_current_scene;         //!< 現在シーン（所有権あり）
    std::string                m_current_scene_name;    //!< 現在シーン名

    //! @brief フェードの状態
    enum class FadeState
    {
        Idle,          //!< フェードしていない
        FadingOut,     //!< 暗転中（0 → 不透明）
        FadingIn,      //!< 暗転からの復帰中（不透明 → 0）
    };

    bool        m_change_pending = false;    //!< 保留中の切り替えがあるか
    std::string m_pending_scene_name;        //!< 保留中の切り替え先シーン名

    FadeState   m_fade_state = FadeState::Idle;    //!< 現在のフェード状態
    int         m_fade_timer = 0;                  //!< 現在のフェード状態に入ってからの経過フレーム数
    std::string m_fade_target_name;                //!< フェードアウト完了時に切り替える先のシーン名
};

}    // namespace Scene
