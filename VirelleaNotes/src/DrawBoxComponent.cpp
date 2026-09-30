//----------------------------------------------------------------------------
//! @file   DrawBoxComponent.cpp
//! @brief  四角描画コンポーネントの実装
//! @details 四角形の塗りつぶし描画を行うコンポーネントの実装を提供します。
//----------------------------------------------------------------------------
#include "DrawBoxComponent.h"
#include "ConstantsRhythm.h"
#include "UIConstants.h"

//============================================================================
//! @class  DrawBoxComponent
//! @details 四角形描画を担当するコンポーネントの具体実装。
//============================================================================
namespace UI {
//--------------------------------------------------------
//! @brief コンストラクタ
//--------------------------------------------------------
DrawBoxComponent::DrawBoxComponent()
    : ComponentBase()
    , m_draw_box{0}    // 描画矩形初期化
    , m_color{0}       // 描画色初期化
{
}

//--------------------------------------------------------
//! @brief 初期化処理
//--------------------------------------------------------
void DrawBoxComponent::Init()
{
    m_draw_box.x = 0;
    m_draw_box.y = 0;
    m_draw_box.w = 0;
    m_draw_box.h = 0;

    // 描画色初期化
    SetColor(0);
}

//--------------------------------------------------------
//! @brief 描画処理
//--------------------------------------------------------
void DrawBoxComponent::Draw()
{
    // 塗りつぶし四角の描画
    DrawFillBox(m_draw_box.x, m_draw_box.y, m_draw_box.x + m_draw_box.w, m_draw_box.y + m_draw_box.h, m_color);
}

//--------------------------------------------------------
//! @brief 矩形サイズ設定
//--------------------------------------------------------
void DrawBoxComponent::SetBoxSize(float x, float y, float w, float h)
{
    m_draw_box.x = x;
    m_draw_box.y = y;
    m_draw_box.w = w;
    m_draw_box.h = h;
}

//--------------------------------------------------------
//! @brief 色設定
//--------------------------------------------------------
void DrawBoxComponent::SetColor(const unsigned int color)
{
    m_color = color;
}
}    // namespace UI
