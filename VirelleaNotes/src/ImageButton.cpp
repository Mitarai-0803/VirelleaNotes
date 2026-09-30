#include "ImageButton.h"
#include "UIConstants.h"
#include "DxLib.h"

namespace UI {
//-----------------------------------------------------------
//! @brief デストラクタ
//! @details 画像ハンドルが生成されていれば削除
//-----------------------------------------------------------
ImageButton::~ImageButton()
{
    End();
}

//-----------------------------------------------------------
//! @brief 初期化処理
//-----------------------------------------------------------
void ImageButton::Init()
{
    m_image_off = -1;    // 番兵値を -1 に統一（0 は有効なハンドル値になり得るため）
    m_image_on  = -1;

    m_pos = {0.0f, 0.0f};

    m_size = {0.0f, 0.0f};

    m_state       = ButtonState::OFF;
    m_was_pressed = false;
    m_scale       = 1.0f;
}

//-----------------------------------------------------------
//! @brief 更新処理
//! @details マウスの位置とクリックに応じてON/OFF切替
//-----------------------------------------------------------
void ImageButton::Update()
{
    int mx = 0, my = 0;
    GetMousePoint(&mx, &my);

    float left   = m_pos.x - m_size.w * 0.5f;
    float top    = m_pos.y - m_size.h * 0.5f;
    float right  = left + m_size.w;
    float bottom = top + m_size.h;

    bool is_hover = (mx >= left && mx <= right && my >= top && my <= bottom);

    m_scale = is_hover ? BUTTON_HOVER_SCALE : 1.0f;

    bool now_pressed = (is_hover && (GetMouseInput() & MOUSE_INPUT_LEFT));

    if(m_input_type == ButtonInputType::TOGGLE) {
        if(now_pressed && !m_was_pressed) {
            m_state = (m_state == ButtonState::ON ? ButtonState::OFF : ButtonState::ON);
        }
    }
    else    // ONE_SHOT
    {
        m_state = (now_pressed ? ButtonState::ON : ButtonState::OFF);
    }

    m_was_pressed = now_pressed;
}

//-----------------------------------------------------------
//! @brief 描画処理
//! @details ON/OFF画像を描画、ホバーで拡大
//-----------------------------------------------------------
void ImageButton::Draw()
{
    int handle = (m_state == ButtonState::ON ? m_image_on : m_image_off);
    DrawRotaGraphF(m_pos.x, m_pos.y, m_scale, 0.0f, handle, true);
}

//-----------------------------------------------------------
//! @brief 終了処理
//! @details 画像ハンドルの削除
//-----------------------------------------------------------
void ImageButton::End()
{
    if(m_image_off != -1) {
        DeleteGraph(m_image_off);
        m_image_off = -1;
    }
    if(m_image_on != -1) {
        DeleteGraph(m_image_on);
        m_image_on = -1;
    }
}

//-----------------------------------------------------------
//! @brief 入力タイプ設定
//! @param type ONE_SHOT or TOGGLE
//-----------------------------------------------------------
void ImageButton::SetInputType(ButtonInputType type)
{
    m_input_type = type;
}

//-----------------------------------------------------------
//! @brief ON状態かどうか取得
//! @return ONならtrue
//-----------------------------------------------------------
bool ImageButton::IsOn() const
{
    return m_state == ButtonState::ON;
}

//-----------------------------------------------------------
//! @brief ON状態に設定
//-----------------------------------------------------------
void ImageButton::SetOn()
{
    m_state = ButtonState::ON;
}

//-----------------------------------------------------------
//! @brief OFF状態に設定
//-----------------------------------------------------------
void ImageButton::SetOff()
{
    m_state = ButtonState::OFF;
}

//-----------------------------------------------------------
//! @brief ボタン画像の設定
//! @param img_on ON画像パス
//! @param img_off OFF画像パス
//! @param pos 描画位置（中央基準）
//-----------------------------------------------------------
void ImageButton::SetButtonImage(const char* img_on, const char* img_off, const FloatPos& pos)
{
    // 既存ハンドルがあれば先に解放してからロードし直す（リーク修正）
    if(m_image_off != -1) {
        DeleteGraph(m_image_off);
        m_image_off = -1;
    }
    if(m_image_on != -1) {
        DeleteGraph(m_image_on);
        m_image_on = -1;
    }
    m_image_off = LoadGraph(img_off);
    m_image_on  = LoadGraph(img_on);

    m_pos = pos;

    // OFF画像を基準にサイズ取得
    int w = 0, h = 0;
    GetGraphSize(m_image_off, &w, &h);
    m_size.w = static_cast<float>(w);
    m_size.h = static_cast<float>(h);
}

//-----------------------------------------------------------
//! @brief クリック状態リセット
//-----------------------------------------------------------
void ImageButton::ResetClick()
{
    m_was_pressed = false;
}
}    // namespace UI
