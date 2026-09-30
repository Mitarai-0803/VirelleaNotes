//----------------------------------------------------------------------------
//! @file   SoftSoundHandle.cpp
//! @brief  ソフトサウンドハンドルの 実装
//! @detail SoftSoundHandle の実装を提供します。
//----------------------------------------------------------------------------
#include "SoftSoundHandle.h"

SoftSoundHandle::SoftSoundHandle() noexcept
    : m_h(-1)
{
}
SoftSoundHandle::SoftSoundHandle(int h) noexcept
    : m_h(h)
{
}
SoftSoundHandle SoftSoundHandle::Load(const std::string& path)
{
    int h = LoadSoftSound(path.c_str());
    return SoftSoundHandle(h);
}
SoftSoundHandle::~SoftSoundHandle()
{
    if(m_h != -1)
        DeleteSoftSound(m_h);
}
SoftSoundHandle::SoftSoundHandle(SoftSoundHandle&& o) noexcept
    : m_h(o.m_h)
{
    o.m_h = -1;
}
SoftSoundHandle& SoftSoundHandle::operator=(SoftSoundHandle&& o) noexcept
{
    if(this != &o) {
        if(m_h != -1)
            DeleteSoftSound(m_h);
        m_h   = o.m_h;
        o.m_h = -1;
    }
    return *this;
}
bool SoftSoundHandle::IsValid() const noexcept
{
    return m_h != -1;
}
int SoftSoundHandle::Get() const noexcept
{
    return m_h;
}
SoftSoundHandle::operator bool() const noexcept
{
    return IsValid();
}
int SoftSoundHandle::Release() noexcept
{
    int t = m_h;
    m_h   = -1;
    return t;
}
void SoftSoundHandle::swap(SoftSoundHandle& o) noexcept
{
    std::swap(m_h, o.m_h);
}
