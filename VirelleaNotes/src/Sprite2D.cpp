//============================================================================
//! @file   Sprite2D.cpp
//! @brief  Sprite2D クラスの実装
//============================================================================
#include "Sprite2D.h"
#include <DxLib.h>

namespace Object {

Sprite2D::Sprite2D(float x, float y, float width, float height)
    : m_width(width)
    , m_height(height)
{
    m_x = x;
    m_y = y;
}

void Sprite2D::OnInit()
{
    SetName("Sprite2D");
}

void Sprite2D::OnUpdate()
{
}

void Sprite2D::OnDraw()
{
    // NOTE: 回転（GetRotation()）はこの最小実装では未対応。
    //       回転付き矩形が必要な場合はテクスチャを用意し、
    //       DrawRotaGraph() 等に差し替えること。
    const int x1 = static_cast<int>(m_x);
    const int y1 = static_cast<int>(m_y);
    const int x2 = static_cast<int>(m_x + m_width);
    const int y2 = static_cast<int>(m_y + m_height);

    DrawBox(x1, y1, x2, y2, GetColor(m_colorR, m_colorG, m_colorB), TRUE);
}

void Sprite2D::OnEnd()
{
}

}    // namespace Object
