//----------------------------------------------------------------------------
//! @file   SoundHandle.h
//! @brief  サウンドハンドルの 宣言
//! @detail DxLib のサウンドハンドルを安全に管理するクラスの宣言
//----------------------------------------------------------------------------
#pragma once

#include <string>
#include <DxLib.h>

//============================================================================
//! @class  SoundHandle
//! サウンドハンドルを で管理するラッパークラス
//============================================================================
class SoundHandle
{
public:
    SoundHandle() noexcept;
    explicit SoundHandle(int h) noexcept;

    static SoundHandle FromSoft(int soft_handle);
    static SoundHandle Load(const std::string& path);

    ~SoundHandle();

    SoundHandle(const SoundHandle&)            = delete;
    SoundHandle& operator=(const SoundHandle&) = delete;

    SoundHandle(SoundHandle&& o) noexcept;
    SoundHandle& operator=(SoundHandle&& o) noexcept;

    bool IsValid() const noexcept;

    int Get() const noexcept;

    explicit operator bool() const noexcept;

    int Release() noexcept;

    void swap(SoundHandle& o) noexcept;

private:
    int m_h = -1;    //!< 内部ハンドル
};
