#include "ImageObject.h"
#include <DxLib.h>

namespace Object {
//-----------------------------------------------------------
// 画像読み込み
//-----------------------------------------------------------
void ImageObject::LoadImage(const std::string& path)
{
    // 既にハンドルを保持している場合は先に解放する（リーク修正）
    if(m_image_handle != -1) {
        DeleteGraph(m_image_handle);
        m_image_handle = -1;
    }
    m_image_handle = LoadGraph(path.c_str());
}

//-----------------------------------------------------------
// ハンドル設定（ムーブ）
//-----------------------------------------------------------
void ImageObject::SetImageHandle(int handle)
{
    // 既存ハンドルがあれば解放
    if(m_image_handle != -1) {
        DeleteGraph(m_image_handle);
    }
    m_image_handle = handle;
}

//-----------------------------------------------------------
// 位置/サイズ設定
//-----------------------------------------------------------
void ImageObject::SetPosition(int x, int y)
{
    m_x = x;
    m_y = y;
}

void ImageObject::SetSize(int w, int h)
{
    m_w = w;
    m_h = h;
}

//-----------------------------------------------------------
// 初期化
//-----------------------------------------------------------
void ImageObject::OnInit()
{
    // 特別な初期処理は不要
}

//-----------------------------------------------------------
// 描画
//-----------------------------------------------------------
void ImageObject::OnDraw()
{
    if(m_image_handle == -1)
        return;

    int h = m_image_handle;
    if(m_w > 0 && m_h > 0) {
        DrawExtendGraph(m_x, m_y, m_x + m_w, m_y + m_h, h, TRUE);
    }
    else {
        DrawGraph(m_x, m_y, h, TRUE);
    }
}

//-----------------------------------------------------------
// 終了
//-----------------------------------------------------------
void ImageObject::OnEnd()
{
    // m_image_handle は単純な int で RAII ではないため、ここで明示的に解放する（リーク修正）
    if(m_image_handle != -1) {
        DeleteGraph(m_image_handle);
        m_image_handle = -1;
    }
}
}    // namespace Object
