#pragma once
//============================================================================
//! @file   SceneBase.h
//! @brief  シーン基底クラスの宣言
//! @details シーン内のオブジェクト管理およびライフサイクルを提供します。
//! @author レオ
//============================================================================
#include "ObjectBase.h"
#include "Renderer.h"
#include <vector>
#include <memory>
#include <string>

namespace Scene
{

// SceneManager はグローバル名前空間ではなく Scene 名前空間内の前方宣言
// （ヘッダの相互依存を避けるため、実体は SceneManager.h を参照）
class SceneManager;

//-----------------------------------------------------------
//! @class SceneBase
//! @brief シーン基底クラス
//! @details シーン内のオブジェクト管理およびライフサイクルを提供
//-----------------------------------------------------------
class SceneBase
{
public:
    //! @brief コンストラクタ
    SceneBase() = default;

    //! @brief デストラクタ
    virtual ~SceneBase() = default;

    //-----------------------------------------------------------
    //! @brief シーン初期化
    //! @details シーン内オブジェクトを初期化してから SceneInit() を呼ぶ
    //!          （SceneInit() 内で AddObject() したオブジェクトは追加時に初期化される）
    //-----------------------------------------------------------
    virtual void Init();

    //-----------------------------------------------------------
    //! @brief シーン更新
    //! @details シーン内オブジェクトを更新してから SceneUpdate() を呼ぶ
    //-----------------------------------------------------------
    virtual void Update();

    //-----------------------------------------------------------
    //! @brief シーン描画
    //! @details シーン内オブジェクトを描画してから SceneDraw() を呼ぶ
    //-----------------------------------------------------------
    virtual void Draw();

    //-----------------------------------------------------------
    //! @brief シーンのデバッグ描画（F3デバッグモード時のみ呼ばれる想定）
    //! @details シーン内オブジェクト（コンポーネント含む）のDrawDebug()を
    //!          呼び出してから SceneDrawDebug() を呼ぶ
    //-----------------------------------------------------------
    virtual void DrawDebug();

    //-----------------------------------------------------------
    //! @brief シーンのImGui描画（F3デバッグモード時のみ呼ばれる想定）
    //! @details DebugManager のフレーム開始後・終了前に Application 側から呼ばれる。
    //!          SceneDrawImGui() を呼び出す。
    //-----------------------------------------------------------
    virtual void DrawImGui();

    //-----------------------------------------------------------
    //! @brief シーン終了処理
    //! @details シーン内オブジェクトを終了してから SceneEnd() を呼ぶ
    //-----------------------------------------------------------
    virtual void End();

    //-----------------------------------------------------------
    //! @brief このシーンを管理する SceneManager を設定する
    //! @details SceneManager::ChangeScene() 内でシーン生成直後に呼ばれる。
    //!          ユーザーコードから呼ぶ必要はない。
    //! @param manager 設定する SceneManager（所有権は持たない）
    //-----------------------------------------------------------
    void SetSceneManager(SceneManager* manager) { m_scene_manager = manager; }

protected:
    //-----------------------------------------------------------
    //! @brief 別のシーンへの切り替えを要求する
    //! @details 実際の切り替え（現在シーンのEnd/破棄と次シーンのInit）は
    //!          SceneManager::Update() の先頭で安全なタイミングに行われる
    //!          （SceneUpdate() の実行中に自分自身が破棄されるのを防ぐため）。
    //! @param name SceneManager::RegisterScene() で登録した名前
    //-----------------------------------------------------------
    void ChangeScene(const std::string& name);

    //! @return このシーンが保持する Renderer（描画の入口）
    Graphics::Renderer& GetRenderer() { return m_renderer; }

    //-----------------------------------------------------------
    //! @brief オブジェクトの追加
    //! @details 追加と同時に Init() を呼ぶ（シーン実行中に追加した場合も初期化される）。
    //! @param obj 追加するオブジェクト（shared_ptr）
    //-----------------------------------------------------------
    void AddObject(std::shared_ptr<Object::ObjectBase> obj);

    //-----------------------------------------------------------
    //! @brief オブジェクトを描画順つきで追加する
    //! @param obj       追加するオブジェクト（shared_ptr）
    //! @param drawOrder 描画順（値が小さいほど先＝奥に描画される）
    //-----------------------------------------------------------
    void AddObject(std::shared_ptr<Object::ObjectBase> obj, int drawOrder);

    //-----------------------------------------------------------
    //! @brief シーン固有の初期化処理
    //! @details 派生クラスで実装する
    //-----------------------------------------------------------
    virtual void SceneInit() {}

    //-----------------------------------------------------------
    //! @brief シーン固有の更新処理
    //! @details 派生クラスで実装する
    //-----------------------------------------------------------
    virtual void SceneUpdate() {}

    //-----------------------------------------------------------
    //! @brief シーン固有の描画処理
    //! @details 派生クラスで実装する
    //-----------------------------------------------------------
    virtual void SceneDraw() {}

    //-----------------------------------------------------------
    //! @brief シーン固有のデバッグ描画処理
    //! @details 派生クラスで必要に応じて実装する（デフォルトは何もしない）
    //-----------------------------------------------------------
    virtual void SceneDrawDebug() {}

    //-----------------------------------------------------------
    //! @brief シーン固有のImGuiウィンドウ描画処理
    //! @details 派生クラスで ImGui::Begin() 〜 ImGui::End() を実装する
    //!          （デフォルトは何もしない）。ImGuiのフレーム中にのみ呼ばれる。
    //-----------------------------------------------------------
    virtual void SceneDrawImGui() {}

    //-----------------------------------------------------------
    //! @brief シーン固有の終了処理
    //! @details 派生クラスで実装する
    //-----------------------------------------------------------
    virtual void SceneEnd() {}

    //-----------------------------------------------------------
    //! @brief オブジェクトを生成して名前を設定し、シーンに追加する
    //! @tparam T 生成するオブジェクトの型（ObjectBase の派生型）
    //! @param obj_name 追加するオブジェクトの名前
    //-----------------------------------------------------------
    template <typename T>
    void AddObjectSetName(const std::string& obj_name)
    {
        auto obj = std::make_shared<T>();    // オブジェクトの作成
        obj->SetName(obj_name);              // 名前の設定
        obj->Init();                         // オブジェクトの初期化処理
        m_scene_objects.push_back(obj);      // シーンにオブジェクトを追加
    }

    //-----------------------------------------------------------
    //! @brief シーン内オブジェクトの取得
    //! @tparam T 取得したい型（ObjectBase または派生型）
    //! @param name オブジェクト名
    //! @return 見つかれば shared_ptr<T>、なければ nullptr
    //-----------------------------------------------------------
    template <typename T>
    std::shared_ptr<T> GetSceneObject(const std::string& name) const
    {
        for(const auto& obj : m_scene_objects) {
            if(obj->GetName() == name) {
                return std::dynamic_pointer_cast<T>(obj);
            }
        }
        return nullptr;
    }

    //! @brief シーン内オブジェクトの初期化呼び出し
    void ObjectInit();

    //! @brief シーン内オブジェクトの更新呼び出し
    void ObjectUpdate();

    //! @brief シーン内オブジェクトのデバッグ描画呼び出し
    void ObjectDrawDebug();

    //! @brief シーン内オブジェクトの描画呼び出し
    void ObjectDraw();

    //! @brief シーン内オブジェクトの終了呼び出し
    void ObjectEnd();

    std::vector<std::shared_ptr<Object::ObjectBase>> m_scene_objects;    //!< シーンが保持するオブジェクト
    Graphics::Renderer                               m_renderer;        //!< このシーンの描画の入口（Scene→Renderer→RenderPipeline）

    int  m_frame_count           = 0;        //!< このシーンが開始してからの更新フレーム数
    bool m_scene_objects_sorted  = false;    //!< 描画順が変更されたら true（次の描画前にソートする）

private:
    SceneManager* m_scene_manager = nullptr;    //!< このシーンを管理する SceneManager（所有権なし）
};

}    // namespace Scene
