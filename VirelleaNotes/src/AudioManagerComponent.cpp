//----------------------------------------------------------------------------
//! @file   AudioManagerComponent.cpp
//! @brief  音源管理コンポーネントの実装
//! @details 音源の再生・停止、音量変更、現在位置の取得などの機能を提供します。
//----------------------------------------------------------------------------
#include "AudioManagerComponent.h"
#include <DxLib.h>    //!< DXライブラリ利用

//============================================================================
//! @class  AudioManagerComponent
//! @details 音源再生管理の具体的実装を提供するクラスです。
//============================================================================
namespace Audio {
//--------------------------------------------------------
//! @brief コンストラクタ
//--------------------------------------------------------
AudioManagerComponent::AudioManagerComponent()
    : m_load_handle(0)       // 音源ハンドル初期化
    , m_stop_position(0)     // 停止位置（ミリ秒）初期化
    , m_prev_volume(0)       // 前フレームの音量初期化
    , m_current_volume(0)    // 現在の音量初期化
{
}

//--------------------------------------------------------
//! @brief 初期化処理
//--------------------------------------------------------
void AudioManagerComponent::Init()
{
    m_load_handle     = 0;        //!< 音源ハンドル初期化
    m_stop_position   = 0;        //!< 停止位置初期化
    m_prev_volume     = 0;        //!< 以前の音量初期化
    m_current_volume  = 0;        //!< 現在の音量初期化
    m_sample_time_pos = 0;        //!< サンプル時間位置初期化
    m_play_mode       = false;    //!< 再生中かどうかフラグの初期化(停止中)
}

//--------------------------------------------------------
//! @brief 更新処理
//--------------------------------------------------------
void AudioManagerComponent::Update()
{
    ChangeVolumeSoundMem(m_current_volume, m_load_handle);            //!< 現在音量を反映
    m_sample_time_pos = GetCurrentPositionSoundMem(m_load_handle);    //!< 現在の再生位置(サンプル)を取得
    m_ms_time_pos     = GetSoundCurrentTime(m_load_handle);           //!< 現在の再生位置(ミリ秒)を取得
}

//--------------------------------------------------------
//! @brief 終了処理
//--------------------------------------------------------
void AudioManagerComponent::End()
{
    // 音源ハンドルの所有権は CSVLoadObject 等が持つため、ここでは解放しない
    m_load_handle = 0;    // ハンドル参照を解除
}

//--------------------------------------------------------
//! @brief 音源の再生
//--------------------------------------------------------
void AudioManagerComponent::HandlePlaySound()
{
    // 音源ハンドルが有効で、再生中でない場合に再生開始
    if(m_load_handle != 0 && CheckSoundMem(m_load_handle) == 0) {
        // 停止位置が記録されていれば反映
        if(m_stop_position != 0) {
            SetSoundCurrentTime(m_stop_position, m_load_handle);    //!< 停止位置反映
        }

        // 音源の再生開始
        PlaySoundMem(m_load_handle, DX_PLAYTYPE_BACK, FALSE);    //!< ループ再生
        m_play_mode = true;                                      //!< 再生状態に変更
    }
}

//--------------------------------------------------------
//! @brief 音源の停止
//--------------------------------------------------------
void AudioManagerComponent::HandleStopSound()
{
    if(m_load_handle > 0 && CheckSoundMem(m_load_handle) == 1) {
        StopSoundMem(m_load_handle);                             //!< サウンド停止
        m_stop_position = GetSoundCurrentTime(m_load_handle);    //!< 停止位置記録
        m_play_mode     = false;                                 //!< 停止状態に変更
    }
}

//--------------------------------------------------------
//! @brief 音源ハンドルを設定
//--------------------------------------------------------
void AudioManagerComponent::SetHandle(int handle)
{
    m_load_handle = handle;    //!< メンバ変数に設定
}

//--------------------------------------------------------
//! @brief 音量設定
//--------------------------------------------------------
void AudioManagerComponent::SetSoundVolume(int volume)
{
    m_current_volume = volume;    //!< メンバ変数に設定
}

//--------------------------------------------------------
//! @brief 停止位置取得
//--------------------------------------------------------
int AudioManagerComponent::GetStopPosition() const
{
    return m_stop_position;    //!< 停止位置返却
}

//--------------------------------------------------------
//! @brief 再生位置取得（サンプル単位）
//--------------------------------------------------------
int AudioManagerComponent::GetSampleTime() const
{
    return m_sample_time_pos;    //!< 再生位置返却
}

//--------------------------------------------------------
//! @brief 再生位置取得（ミリ秒単位）
//--------------------------------------------------------
int AudioManagerComponent::GetMsTime() const
{
    return m_ms_time_pos;    //!< 再生位置返却
}

//--------------------------------------------------------
//! @brief 再生中かどうかの取得
//--------------------------------------------------------
bool AudioManagerComponent::GetPlayeMode() const
{
    return m_play_mode;
}
}    // namespace Audio
