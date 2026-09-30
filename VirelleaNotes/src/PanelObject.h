#pragma once
//============================================================================
//! @file   PanelObject.h
//! @brief  透過PNGを3D空間内の「板」に貼り付けて表示する汎用オブジェクトの宣言
//! @details
//!  1枚の透過PNG（アルファチャンネル対応）を3D空間内の板ポリゴンとして表示する。
//!  EnemyObject が行っている「ビルボードで透過PNGを表示する」処理をベースに、
//!  常にカメラを向くビルボードだけでなく、オブジェクトの回転(SetRotation)に
//!  固定した向きの板としても使えるようにした汎用パネル表示用オブジェクト。
//!  1枚絵を3D空間内に配置するだけで、奥行きのある「2.5D」的な見た目を作れる。
//!
//!  使い方（例）:
//!    auto panel = std::make_shared<Object::PanelObject>(
//!        JPH::RVec3(0.0f, 3.0f, 5.0f),   // 表示位置
//!        2.0f, 1.5f,                     // 半幅・半高さ
//!        "Data/Image/panel.png",         // 透過PNGのパス
//!        Object::PanelBillboardMode::Fixed);
//!    AddObject(panel);
//!
//!  向きを固定したい場合は SetRotation() で任意の角度を指定する。
//!  常にカメラの正面を向かせたい場合は PanelBillboardMode::Billboard を指定する。
//! @author レオ
//============================================================================
#include "ObjectBase3D.h"
#include <Jolt/Jolt.h>
#include <string>

namespace Object {

//! @brief パネルの向きの決め方
enum class PanelBillboardMode
{
    Fixed,        //!< オブジェクトの回転(SetRotation)に固定した向きで描画する（壁の看板・掲示物など）
    Billboard,    //!< 常にカメラの正面を向く（EnemyObjectと同じ方式。1枚絵に立体感を出したい場合向け）
};

//-----------------------------------------------------------
//! @class PanelObject
//! @brief 透過PNG1枚を3D空間内の板として貼り付け表示する汎用オブジェクト
//-----------------------------------------------------------
class PanelObject : public ObjectBase3D
{
public:
    //-----------------------------------------------------------
    //! @param pos パネル中心のワールド座標
    //! @param halfWidth パネルの半幅（X方向の表示サイズ）
    //! @param halfHeight パネルの半高さ（Y方向の表示サイズ）
    //! @param imagePath 透過PNG等の画像パス（アルファチャンネル対応。読込失敗時は単色パネルにフォールバック）
    //! @param mode 向きの決め方（Fixed=回転に固定 / Billboard=常にカメラ正面。デフォルトはFixed）
    //-----------------------------------------------------------
    PanelObject(const JPH::RVec3& pos, float halfWidth, float halfHeight, const std::string& imagePath,
        PanelBillboardMode mode = PanelBillboardMode::Fixed);
    virtual ~PanelObject() = default;

    //! @param mode 向きの決め方を変更する
    void SetBillboardMode(PanelBillboardMode mode) { m_mode = mode; }

    //! @return 現在の向きの決め方
    PanelBillboardMode GetBillboardMode() const { return m_mode; }

    //-----------------------------------------------------------
    //! @param halfWidth パネルの半幅
    //! @param halfHeight パネルの半高さ
    //-----------------------------------------------------------
    void SetSize(float halfWidth, float halfHeight)
    {
        m_halfWidth  = halfWidth;
        m_halfHeight = halfHeight;
    }

protected:
    void OnInit()   override;
    void OnUpdate() override;
    void OnDraw()   override;
    void OnEnd()    override;

private:
    //-----------------------------------------------------------
    //! @brief Fixed モード：オブジェクトの回転(m_rot)に固定した板ポリゴンを描画する
    //! @param center パネル中心のワールド座標
    //-----------------------------------------------------------
    void DrawFixedPanel(const VECTOR& center) const;

    //-----------------------------------------------------------
    //! @brief Billboard モード：常にカメラ正面を向く板として描画する（EnemyObjectと同じ方式）
    //! @param center パネル中心のワールド座標
    //-----------------------------------------------------------
    void DrawBillboardPanel(const VECTOR& center) const;

    //-----------------------------------------------------------
    //! @brief 画像読込に失敗した場合のフォールバック（単色の板）を描画する
    //! @param center パネル中心のワールド座標
    //-----------------------------------------------------------
    void DrawFallbackPanel(const VECTOR& center) const;

    float              m_halfWidth;     //!< パネルの半幅
    float              m_halfHeight;    //!< パネルの半高さ
    std::string        m_imagePath;     //!< 画像ファイルパス
    PanelBillboardMode m_mode;          //!< 向きの決め方

    int m_imageHandle = -1;    //!< LoadGraph() のハンドル（読込失敗時は -1）
};

}    // namespace Object
