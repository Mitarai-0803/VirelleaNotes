#pragma once
//============================================================================
//! @file   DxMain.h
//! @brief  アプリケーションの基本設定。画面サイズや背景色などの定数を定義します。
//! @author レオ
//============================================================================
#define NOMINMAX            //!< Windows.h の min/max マクロを無効化

#include "Windows.h"        //!< Windows API利用
#include "DxLib.h"          //!< DXライブラリ利用
#include "SceneManager.h"   //!< シーンマネージャ宣言
#include "memory"           //!< std::shared_ptr 利用
#include "hlsl++.h"         //!< HLSL 利用

// ※ using namespace はヘッダに書くと全インクルード先に汚染されるため .cpp 側で宣言する

//-----------------------------------------------------------
//! @class Application
//! @brief アプリケーションの初期化・メインループ・終了処理をまとめるクラス
//-----------------------------------------------------------
class Application
{
public:
    //! @return true 初期化成功、false 初期化失敗
    bool Init();

    void Update();
    void End();

private:
    Scene::SceneManager m_scene_manager;    //!< シーンマネージャ
};
