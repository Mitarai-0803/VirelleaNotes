//============================================================================
//! @file   SceneBase.cpp
//! @brief  シーン基底クラスの実装
//! @author レオ
//============================================================================
#include "SceneBase.h"
#include "SceneManager.h"
#include <algorithm>

namespace Scene {

//-----------------------------------------------------------
//! @brief シーン初期化
//! @details まずシーン固有初期化でオブジェクトを AddObject し、
//!          その後まとめて初期化する
//-----------------------------------------------------------
void SceneBase::Init()
{
    ObjectInit();    // 既に登録済みのオブジェクトを初期化
    SceneInit();     // シーン固有初期化（ここで AddObject したオブジェクトは追加時に初期化される）
}

void SceneBase::Update()
{
    ++m_frame_count;    // このシーンの経過フレーム数
    ObjectUpdate();     // オブジェクト更新
    SceneUpdate();      // シーン固有更新（オブジェクト間の連携など。オブジェクト更新後の状態を参照できる）
}

void SceneBase::Draw()
{
    ObjectDraw();    // オブジェクト描画（描画順の小さいものから）
    SceneDraw();     // シーン固有描画（オブジェクトの上に重ねて描画する）
}

//-----------------------------------------------------------
//! @brief シーンのデバッグ描画
//! @details F3デバッグモード時のみ Application 側から呼ばれる想定
//-----------------------------------------------------------
void SceneBase::DrawDebug()
{
    ObjectDrawDebug();
    SceneDrawDebug();
}

//-----------------------------------------------------------
//! @brief シーンのImGui描画
//! @details F3デバッグモード時のみ Application 側から呼ばれる想定
//-----------------------------------------------------------
void SceneBase::DrawImGui()
{
    SceneDrawImGui();
}

//! @brief シーン終了
void SceneBase::End()
{
    ObjectEnd();    // オブジェクトの終了処理（リストもクリアされる）
    SceneEnd();     // シーン固有終了処理
}

//-----------------------------------------------------------
//! @brief オブジェクトを追加する
//! @param obj 追加するオブジェクト
//-----------------------------------------------------------
void SceneBase::AddObject(std::shared_ptr<Object::ObjectBase> obj)
{
    obj->Init();
    m_scene_objects.push_back(obj);
    m_scene_objects_sorted = true;
}

//-----------------------------------------------------------
//! @brief オブジェクトを描画順つきで追加する
//! @param obj       追加するオブジェクト
//! @param drawOrder 描画順（値が小さいほど先に描画される）
//-----------------------------------------------------------
void SceneBase::AddObject(std::shared_ptr<Object::ObjectBase> obj, int drawOrder)
{
    obj->SetDrawOrder(drawOrder);
    AddObject(obj);
}

void SceneBase::ChangeScene(const std::string& name)
{
    if(m_scene_manager) {
        m_scene_manager->RequestChangeScene(name);
    }
}

void SceneBase::ObjectInit()
{
    for(auto& obj : m_scene_objects) {
        obj->Init();
    }
}

void SceneBase::ObjectUpdate()
{
    for(auto& obj : m_scene_objects) {
        obj->Update();
    }
}

void SceneBase::ObjectDrawDebug()
{
    for(auto& obj : m_scene_objects) {
        obj->DrawDebug();
    }
}

void SceneBase::ObjectDraw()
{
    // 描画順が変更された場合のみソートする（同じ描画順は追加順を保つ）
    if(m_scene_objects_sorted) {
        std::stable_sort(m_scene_objects.begin(),
                         m_scene_objects.end(),
                         [](const std::shared_ptr<Object::ObjectBase>& a, const std::shared_ptr<Object::ObjectBase>& b) {
                             return a->GetDrawOrder() < b->GetDrawOrder();
                         });
        m_scene_objects_sorted = false;
    }

    for(auto& obj : m_scene_objects) {
        obj->Draw();
    }
}

//-----------------------------------------------------------
//! @brief シーン内オブジェクトの終了呼び出し
//! @details 全オブジェクトを終了させた後、リストをクリアする
//-----------------------------------------------------------
void SceneBase::ObjectEnd()
{
    for(auto& obj : m_scene_objects) {
        obj->End();
    }

    m_scene_objects.clear();
}

}    // namespace Scene
