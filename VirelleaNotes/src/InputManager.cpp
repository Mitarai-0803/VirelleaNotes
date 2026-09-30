//============================================================================
//! @file   InputManager.cpp
//! @brief  入力管理ユーティリティの実装
//! @details キーボード・マウスの状態を毎フレーム取得し、使用しやすい API を提供します。
//! @author レオ
//============================================================================
#include "InputManager.h"
#include <cstring>

namespace InputManager
{
    // 内部状態変数（このcpp内にのみ閉じ込める。ヘッダには公開しない）
    namespace
    {
        char key_buffer_[256] {};    //!< キー押下状態バッファ
        int  key_frame_[256] {};     //!< キー押下フレームカウンタ
        int  mouse_left_frame_  {};  //!< 左マウスボタン押下フレーム
        int  mouse_right_frame_ {};  //!< 右マウスボタン押下フレーム
        int  mouse_wheel_rot_   {};  //!< マウスホイール回転量

        int  prev_mouse_x_  = 0;     //!< 前フレームのマウスX座標
        int  prev_mouse_y_  = 0;     //!< 前フレームのマウスY座標
        int  mouse_delta_x_ = 0;     //!< マウスX座標の移動量（差分）
        int  mouse_delta_y_ = 0;     //!< マウスY座標の移動量（差分）
    }

    void Update()
    {
        // キー入力状態を取得
        GetHitKeyStateAll(key_buffer_);

        // キーフレームカウンタ更新
        for(int i = 0; i < 256; i++) {
            key_frame_[i] = key_buffer_[i] ? key_frame_[i] + 1 : 0;
        }

        // マウスボタン状態更新
        mouse_left_frame_  = (GetMouseInput() & MOUSE_INPUT_LEFT)  ? mouse_left_frame_  + 1 : 0;
        mouse_right_frame_ = (GetMouseInput() & MOUSE_INPUT_RIGHT) ? mouse_right_frame_ + 1 : 0;

        // マウスホイール回転量取得
        mouse_wheel_rot_ = GetMouseWheelRotVol();

        // マウス座標取得と移動量計算
        int cur_x, cur_y;
        GetMousePoint(&cur_x, &cur_y);
        mouse_delta_x_ = cur_x - prev_mouse_x_;
        mouse_delta_y_ = cur_y - prev_mouse_y_;
        prev_mouse_x_  = cur_x;
        prev_mouse_y_  = cur_y;
    }

    //-----------------------------------------------------------
    //! @param key 判定するキーコード
    //! @return 押下した瞬間なら true
    //-----------------------------------------------------------
    bool PushHitKey(int key)
    {
        return key_frame_[key] == 1;
    }

    //-----------------------------------------------------------
    //! @param key 判定するキーコード
    //! @return 押下中（継続押下を含む）なら true
    //-----------------------------------------------------------
    bool CheckHitKey(int key)
    {
        return key_buffer_[key] != 0;
    }

    //-----------------------------------------------------------
    //! @param button MOUSE_INPUT_LEFT 等
    //! @return 押下中なら true
    //-----------------------------------------------------------
    bool CheckMouseInput(int button)
    {
        return (GetMouseInput() & button) != 0;
    }

    //-----------------------------------------------------------
    //! @param button MOUSE_INPUT_LEFT 等
    //! @return 押下した瞬間なら true
    //-----------------------------------------------------------
    bool PushMouseInput(int button)
    {
        if((button & MOUSE_INPUT_LEFT)  && mouse_left_frame_  == 1) return true;
        if((button & MOUSE_INPUT_RIGHT) && mouse_right_frame_ == 1) return true;
        return false;
    }

    int GetMouseWheel()
    {
        return mouse_wheel_rot_;
    }

    int GetMouseX()
    {
        int x, y;
        GetMousePoint(&x, &y);
        return x;
    }

    int GetMouseY()
    {
        int x, y;
        GetMousePoint(&x, &y);
        return y;
    }

    int GetMouseDeltaX()
    {
        return mouse_delta_x_;
    }

    int GetMouseDeltaY()
    {
        return mouse_delta_y_;
    }

}    // namespace InputManager
