//----------------------------------------------------------------------------
//! @file   SEComponent.h
//! @brief  SE 再生用コンポーネントの宣言
//! @detail ショートサウンド（効果音）を読み込み再生するためのコンポーネントです。
//!         ObjectBase にアタッチして再生制御を行います。
//----------------------------------------------------------------------------
#pragma once

#include "ComponentBase.h"
#include "ResourceHandles.h"
#include <string>

namespace Audio {
//============================================================================
//! @class  SEComponent
//! @detail 効果音の読み込みと再生を行うコンポーネント
//============================================================================
class SEComponent : public ComponentBase
{
public:
    SEComponent()  = default;
    ~SEComponent() = default;

    void Init() override;
    void Update() override;
    void End() override;

    // ファイルパスを設定してロードする
    void SetPath(const std::string& path);

    // 即座に再生する
    void Play();

private:
    SoundHandle m_sound_handle{};    //!< 効果音ハンドル
};
}    // namespace Audio
