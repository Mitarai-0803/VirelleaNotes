//----------------------------------------------------------------------------
//! @file   BgmComponent.h
//! @brief  BGM 再生用コンポーネントの宣言
//! @detail 背景音楽(BGM)をロード・再生・停止・音量制御するコンポーネント
//----------------------------------------------------------------------------
#pragma once

#include "ComponentBase.h"
#include "ResourceHandles.h"

namespace Audio {
//============================================================================
//! @class  BgmComponent
//! @detail BGM の再生管理を行うコンポーネント
//============================================================================
class BgmComponent : public ComponentBase
{
public:
    BgmComponent()  = default;
    ~BgmComponent() = default;

    void Init() override;
    void Update() override;
    void End() override;

    // パスを設定してロード
    void SetPath(const std::string& path);

    // 再生（ループありがデフォルト）
    void Play(bool loop = true);

    // 停止
    void Stop();

    // 音量設定（0-255）
    void SetVolume(int volume);

    // 再生中かどうか
    bool IsPlaying() const;

private:
    SoundHandle m_bgm_handle{};            //!< BGM ハンドル
    int         m_current_volume = 255;    //!< 現在の音量
};
}    // namespace Audio
