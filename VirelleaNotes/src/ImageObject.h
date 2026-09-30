//----------------------------------------------------------------------------
//! @file   ImageObject.h
//! @brief  画像描画用のシンプルなオブジェクト定義
//! @detail 画像ファイルを読み込み、シーン内で描画するための単純な ObjectBase 派生クラスを提供します。
//!         で画像ハンドルを保持し、位置・サイズ・表示フラグを管理します。
//----------------------------------------------------------------------------
#pragma once

#include "ObjectBase.h"
#include <DxLib.h>
#include <string>

namespace Object {
//============================================================================
//! @class ImageObject
//! 画像を読み込み描画するためのオブジェクト
//! @detail 単一の画像ハンドルを保持し、位置とサイズを指定して描画します。
//============================================================================
class ImageObject : public ObjectBase
{
public:
    ImageObject()  = default;
    ~ImageObject() = default;

    // 画像ハンドルを直接取得（既存描画関数との互換用）
    int GetHandle() const { return m_image_handle; }

    // 画像をファイルパスから読み込み
    void LoadImage(const std::string& path);

    // 既存のハンドルを設定
    void SetImageHandle(int handle);

    // 位置とサイズの設定
    void SetPosition(int x, int y);
    void SetSize(int w, int h);

    // 表示/非表示は基底クラス（ObjectBase::SetVisible / IsVisible）を使用する

protected:
    void OnInit() override;
    void OnDraw() override;
    void OnEnd() override;

private:
    int m_image_handle = -1;    //!< 画像ハンドル
    int m_x            = 0;     //!< 描画 X 座標
    int m_y            = 0;     //!< 描画 Y 座標
    int m_w            = 0;     //!< 描画幅（0 の場合は画像幅を使用）
    int m_h            = 0;     //!< 描画高さ（0 の場合は画像高さを使用）
};
}    // namespace Object
