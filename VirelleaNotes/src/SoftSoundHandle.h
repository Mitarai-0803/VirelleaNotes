//----------------------------------------------------------------------------
//! @file   SoftSoundHandle.h
//! @brief  ソフトサウンドハンドルの 宣言
//! @detail DxLib のソフトサウンドハンドルを安全に管理するクラスの宣言
//----------------------------------------------------------------------------
#pragma once

#include <string>
#include <DxLib.h>

//============================================================================
//! @class  SoftSoundHandle
//! ソフトサウンドハンドルを で管理するラッパークラス
//============================================================================
class SoftSoundHandle
{
public:
    SoftSoundHandle() noexcept;
    explicit SoftSoundHandle(int h) noexcept;

    static SoftSoundHandle Load(const std::string& path);

    ~SoftSoundHandle();

    SoftSoundHandle(const SoftSoundHandle&)            = delete;
    SoftSoundHandle& operator=(const SoftSoundHandle&) = delete;

    SoftSoundHandle(SoftSoundHandle&& o) noexcept;
    SoftSoundHandle& operator=(SoftSoundHandle&& o) noexcept;

    bool IsValid() const noexcept;

    int Get() const noexcept;

    explicit operator bool() const noexcept;

    int Release() noexcept;

    void swap(SoftSoundHandle& o) noexcept;

private:
    int m_h = -1;    //!< 内部ハンドル
};
