#pragma once
//============================================================================
//! @file   InputManager.h
//! @brief  入力管理ユーティリティの宣言
//! @details 全キー入力とマウス入力を管理します。
//! @author レオ
//============================================================================
#include "DxLib.h"

namespace InputManager
{
    //! 入力状態更新（毎フレーム呼ぶ）
    void Update();

    //-------------------------------------------------------
    //! @param key 判定するキーコード
    //! @return true 押された瞬間
    //-------------------------------------------------------
    bool PushHitKey(int key);

    //-------------------------------------------------------
    //! @param key 判定するキーコード
    //! @return true 押下中（継続押下を含む）
    //-------------------------------------------------------
    bool CheckHitKey(int key);

    //-------------------------------------------------------
    //! @param button MOUSE_INPUT_LEFT, MOUSE_INPUT_RIGHT など
    //! @return true 押下中
    //-------------------------------------------------------
    bool CheckMouseInput(int button);

    //-------------------------------------------------------
    //! @param button MOUSE_INPUT_LEFT, MOUSE_INPUT_RIGHT など
    //! @return true 押された瞬間
    //-------------------------------------------------------
    bool PushMouseInput(int button);

    //! @return 1以上 = 奥回転, 0 = 回していない, -1以下 = 手前回転
    int GetMouseWheel();

    //! @return マウスのスクリーンX座標
    int GetMouseX();

    //! @return マウスのスクリーンY座標
    int GetMouseY();

    //! @return マウスX座標の前フレームからの変化量
    int GetMouseDeltaX();

    //! @return マウスY座標の前フレームからの変化量
    int GetMouseDeltaY();

    // ※ 内部状態変数（key_buffer_ 等）は InputManager.cpp 内に閉じ込めてあります。
    //   外部から直接参照が必要な場合は上記のアクセサ関数を使ってください。

}    // namespace InputManager
