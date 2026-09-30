//----------------------------------------------------------------------------
//! @file   SoundHandle.cpp
//! @brief  サウンドハンドルの 実装
//! @detail SoundHandle の実装を提供します。
//----------------------------------------------------------------------------
#include "SoundHandle.h"

SoundHandle::SoundHandle() noexcept
    : m_h(-1)
{
}
SoundHandle::SoundHandle(int h) noexcept
    : m_h(h)
{
}
SoundHandle SoundHandle::FromSoft(int soft_handle)
{
    if(soft_handle == -1)
        return SoundHandle(-1);
    int h = LoadSoundMemFromSoftSound(soft_handle);
    return SoundHandle(h);
}
SoundHandle SoundHandle::Load(const std::string& path)
{
    int h = LoadSoundMem(path.c_str());
    return SoundHandle(h);
}
SoundHandle::~SoundHandle()
{
    if(m_h != -1)
        DeleteSoundMem(m_h);
}
SoundHandle::SoundHandle(SoundHandle&& o) noexcept
    : m_h(o.m_h)
{
    o.m_h = -1;
}
SoundHandle& SoundHandle::operator=(SoundHandle&& o) noexcept
{
    if(this != &o) {
        if(m_h != -1)
            DeleteSoundMem(m_h);
        m_h   = o.m_h;
        o.m_h = -1;
    }
    return *this;
}
bool SoundHandle::IsValid() const noexcept
{
    return m_h != -1;
}
int SoundHandle::Get() const noexcept
{
    return m_h;
}
SoundHandle::operator bool() const noexcept
{
    return IsValid();
}
int SoundHandle::Release() noexcept
{
    int t = m_h;
    m_h   = -1;
    return t;
}
void SoundHandle::swap(SoundHandle& o) noexcept
{
    std::swap(m_h, o.m_h);
}
