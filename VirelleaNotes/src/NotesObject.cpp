#include "NotesObject.h"
#include "Colors.h"
#include "ConstantsRhythm.h"

namespace Object {
//-----------------------------------------------------------
//! @brief 初期化処理
//-----------------------------------------------------------
void NotesObject::OnInit()
{
    m_pos.x        = 0.0f;
    m_pos.y        = 0.0f;
    m_speed        = NOTES_DEFAULT_SPEED;
    m_image_handle = 0;
}

//-----------------------------------------------------------
//! @brief 更新処理
//-----------------------------------------------------------
void NotesObject::OnUpdate()
{
    if(m_pos.y <= SCREEN_H) {
        m_pos.y += m_speed;
    }
}

//-----------------------------------------------------------
//! @brief 描画処理
//-----------------------------------------------------------
void NotesObject::OnDraw()
{
    //	ノーツの画像
    float image_w;
    float image_h;

    //ノーツの画像の幅の取得
    GetGraphSizeF(m_image_handle, &image_w, &image_h);

    // ノーツの描画
    DrawRotaGraph(m_pos.x + (image_w * HALF), m_pos.y, 1.0f, 0.0f, m_image_handle, true);
}

//-----------------------------------------------------------
//! @brief 終了処理
//-----------------------------------------------------------
void NotesObject::OnEnd()
{
    //	読み込んだ画像の消去
    DeleteGraph(m_image_handle);
}

//-----------------------------------------------------------
//! @brief 座標を設定
//-----------------------------------------------------------
void NotesObject::SetPos(float x, float y)
{
    m_pos = {x, y};
}

//-----------------------------------------------------------
//! @brief ノーツタイプを設定
//-----------------------------------------------------------
void NotesObject::SetNotesType(int type)
{
    m_notes_type = type;
}

//-----------------------------------------------------------
//! @brief 座標を取得
//-----------------------------------------------------------
FloatPos NotesObject::GetPos()
{
    return m_pos;
}

//-----------------------------------------------------------
//! @brief ノーツタイプを取得
//-----------------------------------------------------------
int NotesObject::GetNotesType()
{
    return m_notes_type;
}

//-----------------------------------------------------------
//! @brief ノーツ画像を設定
//-----------------------------------------------------------
void NotesObject::SetNotesImage(int& notes_image_handle)
{
    m_image_handle = notes_image_handle;
}
}    // namespace Object
