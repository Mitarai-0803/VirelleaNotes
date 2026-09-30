//----------------------------------------------------------------------------
//! @file   BgmComponent.cpp
//! @brief  BGM 再生用コンポーネントの実装
//! @detail 背景音楽(BGM)のロードと再生、停止、音量制御を提供します。
//----------------------------------------------------------------------------
#include "BgmComponent.h"
#include <DxLib.h>

namespace Audio {
//--------------------------------------------------------
// 初期化
//--------------------------------------------------------
void BgmComponent::Init()
{
    m_current_volume = 255;
}

//--------------------------------------------------------
// 更新
//--------------------------------------------------------
void BgmComponent::Update()
{
    // 音量を適用
    if(m_bgm_handle.IsValid())
        ChangeVolumeSoundMem(m_current_volume, m_bgm_handle.Get());
}

//--------------------------------------------------------
// 終了
//--------------------------------------------------------
void BgmComponent::End()
{
    // ハンドルは SoundHandle のデストラクタで解放される
}

//--------------------------------------------------------
// パス設定とロード
//--------------------------------------------------------
void BgmComponent::SetPath(const std::string& path)
{
    m_bgm_handle = SoundHandle::Load(path);
}

//--------------------------------------------------------
// 再生
//--------------------------------------------------------
void BgmComponent::Play(bool loop)
{
    if(!m_bgm_handle.IsValid())
        return;
    int playtype = loop ? DX_PLAYTYPE_BACK : DX_PLAYTYPE_NORMAL;
    PlaySoundMem(m_bgm_handle.Get(), playtype, FALSE);
}

//--------------------------------------------------------
// 停止
//--------------------------------------------------------
void BgmComponent::Stop()
{
    if(!m_bgm_handle.IsValid())
        return;
    StopSoundMem(m_bgm_handle.Get());
}

//--------------------------------------------------------
// 音量設定
//--------------------------------------------------------
void BgmComponent::SetVolume(int volume)
{
    m_current_volume = volume;
    if(m_bgm_handle.IsValid())
        ChangeVolumeSoundMem(m_current_volume, m_bgm_handle.Get());
}

//--------------------------------------------------------
// 再生中チェック
//--------------------------------------------------------
bool BgmComponent::IsPlaying() const
{
    if(!m_bgm_handle.IsValid())
        return false;
    return CheckSoundMem(m_bgm_handle.Get()) != 0;
}
}    // namespace Audio
