//----------------------------------------------------------------------------
//! @file   SoundVisualizerObject.cpp
//! @brief  サウンドビジュアライザーの実装
//! @detail FFTによる解析と描画処理を提供します。
//----------------------------------------------------------------------------
#include "SoundVisualizerObject.h"
#include <DxLib.h>
#include <cmath>

namespace Object {
//----------------------------------------------------------------------------
//! 初期化処理
//----------------------------------------------------------------------------
void SoundVisualizerObject::OnInit()
{
    // ビジュアライザー位置初期化
    m_sound_visualizer_pos = {0.0f, 0.0f};
}

//----------------------------------------------------------------------------
//! 更新処理
//----------------------------------------------------------------------------
void SoundVisualizerObject::OnUpdate()
{
    // ソフトサウンドからFFTデータを取得
    GetFFTVibrationSoftSound(m_soft_sound_handle, -1, m_sample_pos, 4096, m_param_list, BUFFER_LENGTH);
}

//----------------------------------------------------------------------------
//! 描画処理
//----------------------------------------------------------------------------
void SoundVisualizerObject::OnDraw()
{
    // パラメータ配列を描画
    for(int i = 0; i < BUFFER_LENGTH; ++i) {
        float param = static_cast<float>(std::pow(m_param_list[i], 0.6)) * 8.0f;

        DrawBoxAA(m_sound_visualizer_pos.x - static_cast<int>(param * SOUNDL__MAX_W),
                  ((BUFFER_LENGTH - 1) - i) * LENGTH_SPACE + m_sound_visualizer_pos.y,
                  m_sound_visualizer_pos.x + static_cast<int>(param * SOUNDL__MAX_W),
                  ((BUFFER_LENGTH - 1) - i) * LENGTH_SPACE + SOUNDL_H + m_sound_visualizer_pos.y,
                  GetColor(255, 255, 255),
                  TRUE);

        DrawBoxAA(m_sound_visualizer_pos.x - static_cast<int>(param * SOUNDL__MAX_W),
                  i * LENGTH_SPACE + m_sound_visualizer_pos.y,
                  m_sound_visualizer_pos.x + static_cast<int>(param * SOUNDL__MAX_W),
                  (i * LENGTH_SPACE) + SOUNDL_H + m_sound_visualizer_pos.y,
                  GetColor(255, 255, 255),
                  TRUE);
    }
}

//----------------------------------------------------------------------------
//! 終了処理
//----------------------------------------------------------------------------
void SoundVisualizerObject::OnEnd()
{
    // m_soft_sound_handle の所有権は CSVLoadObject 側にあるため参照をクリアするのみ
    m_soft_sound_handle = 0;

    // m_sound_handle は SetSoftHandle() 内で LoadSoundMemFromSoftSound により
    // このオブジェクト自身が生成したハンドルなので、ここで確実に解放する（リーク修正）
    if(m_sound_handle != 0) {
        DeleteSoundMem(m_sound_handle);
        m_sound_handle = 0;
    }
}

//----------------------------------------------------------------------------
//! ソフトサウンドハンドルを設定
//----------------------------------------------------------------------------
void SoundVisualizerObject::SetSoftHandle(int handle)
{
    // 既に再生用ハンドルがある場合は解放してから再生成
    if(m_sound_handle != 0) {
        DeleteSoundMem(m_sound_handle);
        m_sound_handle = 0;
    }

    // ソフトサウンドハンドルを保持
    m_soft_sound_handle = handle;

    // ソフトハンドルが有効なら再生用ハンドルを作成
    if(m_soft_sound_handle != 0) {
        m_sound_handle  = LoadSoundMemFromSoftSound(m_soft_sound_handle);
        m_one_load_flag = true;
    }
}

//----------------------------------------------------------------------------
//! サンプル位置を設定
//----------------------------------------------------------------------------
void SoundVisualizerObject::SetSampleTime(int sample_time)
{
    m_sample_pos = sample_time;
}

//----------------------------------------------------------------------------
//! 描画位置を設定
//----------------------------------------------------------------------------
void SoundVisualizerObject::SetPosition(float x, float y)
{
    m_sound_visualizer_pos.x = x;
    m_sound_visualizer_pos.y = y;
}
}    // namespace Object
