#include "ResultSongIconObject.h"
#include <filesystem>

namespace Object {
//-------------------------------------------------------
//! @brief 初期化処理
//-------------------------------------------------------
void ResultSongIconObject::OnInit()
{
    // デフォルト位置とサイズを UIConstants から設定（外部から設定がある場合は上書きしない）
    if(m_x == -1) {
        m_x = RESULT_ICON_X;
    }

    if(m_y == -1) {
        m_y = RESULT_ICON_Y;
    }

    if(m_size == -1) {
        m_size = RESULT_ICON_SIZE;
    }

    // CSVLoadObject から選択されたデータセットを取得して icon.png を読み込む
    auto datasets = CSVLoadObject::GetAvailableDataSets();
    int  sel      = CSVLoadObject::GetSelectedIndex();
    if(sel >= 0 && sel < static_cast<int>(datasets.size())) {
        std::filesystem::path csvpath(datasets[sel].csv_path);
        std::filesystem::path dataset_dir = csvpath.parent_path().parent_path();
        std::filesystem::path icon_path   = dataset_dir / "icon.png";
        if(std::filesystem::exists(icon_path)) {
            int h = LoadGraph(icon_path.string().c_str());
            if(h != -1) {
                m_icon_handle = ImageHandle(h);
                // 画像サイズとスケール等をキャッシュ
                GetGraphSize(m_icon_handle.Get(), &m_icon_img_w, &m_icon_img_h);
                if(m_icon_img_w > 0 && m_icon_img_h > 0) {
                    float scale_x = static_cast<float>(m_size) / static_cast<float>(m_icon_img_w);
                    float scale_y = static_cast<float>(m_size) / static_cast<float>(m_icon_img_h);
                    m_icon_scale  = (scale_x + scale_y) * 0.5f;
                    m_icon_cx     = static_cast<float>(m_x) + m_size * 0.5f;
                    m_icon_cy     = static_cast<float>(m_y) + m_size * 0.5f;
                    m_icon_loaded = true;
                }
            }
        }
    }
}

//-------------------------------------------------------
//! @brief 描画処理
//-------------------------------------------------------
void ResultSongIconObject::OnDraw()
{
    if(!m_icon_loaded) {
        // 画像がない場合は灰色の箱を描画
        DrawBox(m_x, m_y, m_x + m_size, m_y + m_size, COLOR_DARKGRAY, TRUE);
        return;
    }

    // キャッシュした情報を用いて描画（毎フレームのローカル生成なし）
    DrawRotaGraphF(m_icon_cx, m_icon_cy, m_icon_scale, 0.0f, m_icon_handle.Get(), TRUE);
}

//-------------------------------------------------------
//! @brief 終了処理
//-------------------------------------------------------
void ResultSongIconObject::OnEnd()
{
    // m_icon_handle は ImageHandle(RAII) なので、ここで手動 DeleteGraph すると
    // デストラクタが再度解放してしまい二重解放になる（バグ修正: 明示的な解放を削除）
}

//-------------------------------------------------------
//! @brief 表示位置を外部から設定可能にする
//! @param x X座標
//! @param y Y座標
//-------------------------------------------------------
void ResultSongIconObject::SetPosition(int x, int y)
{
    m_x = x;
    m_y = y;
}

//-------------------------------------------------------
//! @brief 表示サイズを外部から設定可能にする
//! @param size サイズ
//-------------------------------------------------------
void ResultSongIconObject::SetSize(int size)
{
    m_size = size;
}

}    // namespace Object
