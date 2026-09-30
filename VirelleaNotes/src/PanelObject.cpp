//============================================================================
//! @file   PanelObject.cpp
//! @brief  PanelObject クラスの実装
//============================================================================
#include "PanelObject.h"
#include <DxLib.h>

namespace Object {

namespace {
//! @brief フォールバック描画（画像なし時）の板の半奥行き
constexpr float PANEL_FALLBACK_HALF_DEPTH = 0.05f;
}    // namespace

PanelObject::PanelObject(
    const JPH::RVec3& pos, float halfWidth, float halfHeight, const std::string& imagePath, PanelBillboardMode mode)
    : m_halfWidth(halfWidth)
    , m_halfHeight(halfHeight)
    , m_imagePath(imagePath)
    , m_mode(mode)
{
    // コライダーを持たないため、座標は基底クラスの手動座標（m_pos）にそのまま保持する
    m_pos = pos;

    // NOTE: EnemyObjectと同じ理由で、OnInit()はAddObject()されたタイミングでしか
    //       自動的に呼ばれないため（ゲーム中に生成する場合に備えて）、
    //       画像読込はコンストラクタで確実に行う。
    m_imageHandle = LoadGraph(m_imagePath.c_str());
}

void PanelObject::OnInit()
{
}

void PanelObject::OnUpdate()
{
}

void PanelObject::DrawFixedPanel(const VECTOR& center) const
{
    // オブジェクトの回転(m_rot / SetRotationで設定)をローカル座標の四隅に適用し、
    // 任意の向きの板を組み立てる。ローカル空間では板はXY平面上にあり、
    // 回転なしの初期状態ではZ+方向を向く。
    const JPH::Quat rot = GetRotation();

    const JPH::Vec3 localTopLeft     = JPH::Vec3(-m_halfWidth, m_halfHeight, 0.0f);
    const JPH::Vec3 localTopRight    = JPH::Vec3(m_halfWidth, m_halfHeight, 0.0f);
    const JPH::Vec3 localBottomLeft  = JPH::Vec3(-m_halfWidth, -m_halfHeight, 0.0f);
    const JPH::Vec3 localBottomRight = JPH::Vec3(m_halfWidth, -m_halfHeight, 0.0f);
    const JPH::Vec3 localNormal      = JPH::Vec3(0.0f, 0.0f, 1.0f);

    const JPH::Vec3 worldTopLeft     = rot * localTopLeft;
    const JPH::Vec3 worldTopRight    = rot * localTopRight;
    const JPH::Vec3 worldBottomLeft  = rot * localBottomLeft;
    const JPH::Vec3 worldBottomRight = rot * localBottomRight;
    const JPH::Vec3 worldNormal      = rot * localNormal;

    const VECTOR posTopLeft = VGet(center.x + worldTopLeft.GetX(), center.y + worldTopLeft.GetY(), center.z + worldTopLeft.GetZ());
    const VECTOR posTopRight =
        VGet(center.x + worldTopRight.GetX(), center.y + worldTopRight.GetY(), center.z + worldTopRight.GetZ());
    const VECTOR posBottomLeft =
        VGet(center.x + worldBottomLeft.GetX(), center.y + worldBottomLeft.GetY(), center.z + worldBottomLeft.GetZ());
    const VECTOR posBottomRight =
        VGet(center.x + worldBottomRight.GetX(), center.y + worldBottomRight.GetY(), center.z + worldBottomRight.GetZ());
    const VECTOR normal = VGet(worldNormal.GetX(), worldNormal.GetY(), worldNormal.GetZ());

    const COLOR_U8 white{255, 255, 255, 255};
    const COLOR_U8 noSpecular{0, 0, 0, 0};

    // 板1枚 = 2三角形だが、回転のさせ方次第で頂点の巻き順（表裏の向き）が
    // 入れ替わってしまう可能性があるため、正逆両方の巻き順で計4三角形を積み、
    // カリング設定に関係なくどちらの面からでも表示されるようにする。
    VERTEX3D vertices[12]{};

    auto setVertex = [&](int index, const VECTOR& pos, float u, float v) {
        vertices[index].pos  = pos;
        vertices[index].norm = normal;
        vertices[index].dif  = white;
        vertices[index].spc  = noSpecular;
        vertices[index].u    = u;
        vertices[index].v    = v;
    };

    // 正面向き: 左下 → 左上 → 右上、左下 → 右上 → 右下
    setVertex(0, posBottomLeft, 0.0f, 1.0f);
    setVertex(1, posTopLeft, 0.0f, 0.0f);
    setVertex(2, posTopRight, 1.0f, 0.0f);
    setVertex(3, posBottomLeft, 0.0f, 1.0f);
    setVertex(4, posTopRight, 1.0f, 0.0f);
    setVertex(5, posBottomRight, 1.0f, 1.0f);

    // 逆向き（上記と同じ3頂点を逆順にしただけ）: 裏側からも見えるようにするため
    setVertex(6, posBottomLeft, 0.0f, 1.0f);
    setVertex(7, posTopRight, 1.0f, 0.0f);
    setVertex(8, posTopLeft, 0.0f, 0.0f);
    setVertex(9, posBottomLeft, 0.0f, 1.0f);
    setVertex(10, posBottomRight, 1.0f, 1.0f);
    setVertex(11, posTopRight, 1.0f, 0.0f);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawPolygon3D(vertices, 4, m_imageHandle, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void PanelObject::DrawBillboardPanel(const VECTOR& center) const
{
    // EnemyObjectと同じ考え方：常にカメラ正面を向くビルボードとして描画する。
    int texW = 1;
    int texH = 1;
    GetGraphSize(m_imageHandle, &texW, &texH);
    if(texW <= 0)
        texW = 1;
    if(texH <= 0)
        texH = 1;

    // 画像のピクセルサイズを基準に、指定した半幅・半高さぴったりの
    // ワールドサイズになるよう拡大率を逆算する。
    const float exRateX = (m_halfWidth * 2.0f) / static_cast<float>(texW);
    const float exRateY = (m_halfHeight * 2.0f) / static_cast<float>(texH);

    // PNGのアルファチャンネルを有効にした状態で描画する。
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawBillboard3D(center, 0.5f, 0.5f, exRateX, exRateY, 0.0f, m_imageHandle, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void PanelObject::DrawFallbackPanel(const VECTOR& center) const
{
    // 画像未配置・読込失敗時のフォールバック：単色の板として表示する
    const VECTOR minPos = VGet(center.x - m_halfWidth, center.y - m_halfHeight, center.z - PANEL_FALLBACK_HALF_DEPTH);
    const VECTOR maxPos = VGet(center.x + m_halfWidth, center.y + m_halfHeight, center.z + PANEL_FALLBACK_HALF_DEPTH);
    DrawCube3D(minPos, maxPos, GetColor(200, 200, 200), GetColor(80, 80, 80), true);
}

void PanelObject::OnDraw()
{
    const JPH::RVec3 pos = GetPosition();
    const VECTOR     center =
        VGet(static_cast<float>(pos.GetX()), static_cast<float>(pos.GetY()), static_cast<float>(pos.GetZ()));

    if(m_imageHandle == -1) {
        DrawFallbackPanel(center);
        return;
    }

    if(m_mode == PanelBillboardMode::Billboard) {
        DrawBillboardPanel(center);
    }
    else {
        DrawFixedPanel(center);
    }
}

void PanelObject::OnEnd()
{
    if(m_imageHandle != -1) {
        DeleteGraph(m_imageHandle);
        m_imageHandle = -1;
    }
}

}    // namespace Object
