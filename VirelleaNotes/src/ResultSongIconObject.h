#pragma once
#include "ObjectBase.h"
#include "ResourceHandles.h"
#include "CSVLoadObject.h"
#include "UIConstants.h"
#include <filesystem>
#include <DxLib.h>

namespace Object {

//----------------------------------------------------------------------------
//! @class  ResultSongIconObject
//! @detail リザルト画面の右側に選択した曲のアイコンを表示するオブジェクト
//----------------------------------------------------------------------------
class ResultSongIconObject : public ObjectBase
{
public:
    ResultSongIconObject()  = default;
    ~ResultSongIconObject() = default;

    void OnInit() override;
    void OnDraw() override;
    void OnEnd() override;

    // 表示位置を外部から設定可能にする
    void SetPosition(int x, int y);

    // 表示サイズを外部から設定可能にする
    void SetSize(int size);

private:
    ImageHandle m_icon_handle;
    int         m_x    = -1;    //!< 描画 X 座標（-1 の場合はデフォルト配置を使用）
    int         m_y    = -1;    //!< 描画 Y 座標（-1 の場合はデフォルト配置を使用）
    int         m_size = -1;    //!< 描画サイズ（-1 の場合はデフォルトサイズを使用）

    // 描画時のローカル変数をメンバ化して毎フレームの再生成を回避
    int   m_icon_img_w  = 0;        //!< 読み込んだ画像の幅
    int   m_icon_img_h  = 0;        //!< 読み込んだ画像の高さ
    float m_icon_scale  = 1.0f;     //!< 描画時のスケール
    float m_icon_cx     = 0.0f;     //!< 描画時の中心 X
    float m_icon_cy     = 0.0f;     //!< 描画時の中心 Y
    bool  m_icon_loaded = false;    //!< アイコンが読み込まれているかのフラグ
};

}    // namespace Object
