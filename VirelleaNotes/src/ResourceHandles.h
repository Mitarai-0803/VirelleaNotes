//----------------------------------------------------------------------------
//! @file   ResourceHandles.h
//! @brief  リソースハンドルの ラッパー宣言
//! @detail DxLib の各種ハンドル（ソフトサウンド、画像、サウンド）を安全に管理する
//!         クラスの宣言を提供します。リソースの取得・解放を自動化して安全な
//!         所有権管理を行います。
//----------------------------------------------------------------------------
#pragma once

#include <DxLib.h>
#include <string>
#include <utility>

// 共通の無効ハンドル値
constexpr int kInvalidHandle = -1;

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
    int m_h = kInvalidHandle;    //!< 内部ハンドル
};

//============================================================================
//! @class  ImageHandle
//! 画像ハンドルを で管理するラッパークラス
//============================================================================
class ImageHandle
{
public:
    ImageHandle() noexcept;
    explicit ImageHandle(int h) noexcept;

    static ImageHandle Load(const std::string& path);

    ~ImageHandle();

    ImageHandle(const ImageHandle&)            = delete;
    ImageHandle& operator=(const ImageHandle&) = delete;

    ImageHandle(ImageHandle&& o) noexcept;
    ImageHandle& operator=(ImageHandle&& o) noexcept;

    bool IsValid() const noexcept;

    int Get() const noexcept;

    explicit operator bool() const noexcept;

    int Release() noexcept;

    void swap(ImageHandle& o) noexcept;

private:
    int m_h = kInvalidHandle;    //!< 内部ハンドル
};

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
    int m_h = kInvalidHandle;    //!< 内部ハンドル
};
