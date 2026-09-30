//============================================================================
//! @file   RenderPass.cpp
//! @brief  RenderPass 標準パスの実装
//============================================================================
#include "RenderPass.h"
#include "ObjectBase3D.h"
#include "ShadowMapManager.h"
#include "PostProcessParams.h"
#include "ConstantsEngine.h"
#include "ConstantsGame.h"
#include <DxLib.h>
#include <algorithm>
#include <cmath>
#include <vector>

namespace Graphics {

namespace {

//-----------------------------------------------------------
//! @brief 画像を全面に貼る（DxLib既定の2D頂点シェーダー + 現在設定中のピクセルシェーダー）
//! @details DrawGraph() は SetUsePixelShader() で設定した独自シェーダーを使わないため、
//!          VERTEX2DSHADER + DrawPolygonIndexed2DToShader() で描画する必要がある。
//-----------------------------------------------------------
void DrawGraphWithShader(int graphHandle)
{
    int w = 0;
    int h = 0;
    GetGraphSize(graphHandle, &w, &h);

    VERTEX2DSHADER v[4] = {};
    for(auto& vert : v) {
        vert.rhw = 1.0f;
        vert.dif = GetColorU8(255, 255, 255, 255);
        vert.spc = GetColorU8(0, 0, 0, 0);
    }

    const float x0 = 0.0f;
    const float y0 = 0.0f;
    const float x1 = static_cast<float>(w);
    const float y1 = static_cast<float>(h);

    v[0].pos = VGet(x0, y0, 0.0f);  v[0].u = v[0].su = 0.0f;  v[0].v = v[0].sv = 0.0f;
    v[1].pos = VGet(x1, y0, 0.0f);  v[1].u = v[1].su = 1.0f;  v[1].v = v[1].sv = 0.0f;
    v[2].pos = VGet(x0, y1, 0.0f);  v[2].u = v[2].su = 0.0f;  v[2].v = v[2].sv = 1.0f;
    v[3].pos = VGet(x1, y1, 0.0f);  v[3].u = v[3].su = 1.0f;  v[3].v = v[3].sv = 1.0f;

    unsigned short index[6] = {0, 1, 2, 2, 1, 3};
    DrawPolygonIndexed2DToShader(v, 4, index, 2);
}

}    // namespace

void GeometryPass::Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera)
{
    // オフスクリーンのレンダーターゲットへ切り替える。
    // 失敗時（未作成・DX11未初期化等）は何もしないため、現在アクティブな
    // 描画先（通常はバックバッファ）へそのまま直接描画するフォールバックになる。
    bool useOffscreen = RenderTargetManager::GetInstance().SetActive(m_targetName);
    if(useOffscreen) {
        ClearDrawScreen();    //!< 切り替えた先（このレンダーターゲット）をクリア
    }

    for(const auto& obj : objects) {
        if(!obj)
            continue;
        if(!obj->IsVisible())
            continue;    // オブジェクト自身が非表示なら除外
        if(!camera.GetLayerMask().Contains(obj->GetLayer()))
            continue;    // カメラのLayerMask対象外なら除外

        obj->Draw();    // 実際の描画は各オブジェクトの OnDraw() に委ねる
    }

    // 後続のパス（PostProcessPass等）がバックバッファを前提にできるよう、必ず戻しておく
    if(useOffscreen) {
        RenderTargetManager::GetInstance().ResetToScreen();
    }
}

// --- ShadowMapPass は実装済み。TransparentPass / FinalPass は雛形のまま（将来の拡張ポイント）。 ---

void ShadowMapPass::Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera)
{
    (void)camera;    // シャドウマップはライト視点で描画するため、カメラのLayerMask判定は使用しない

    auto& shadowMap = ShadowMapManager::GetInstance();
    if(!shadowMap.IsValid())
        return;    // ShadowMapManager::Init() が未実行、または失敗している場合は何もしない

    // 前フレームの「シャドウマップを使う」設定が残っていると深度記録中の描画に
    // 影響するため、記録を始める前に一旦解除しておく。
    shadowMap.EndUse();

    shadowMap.BeginRecord();

    for(const auto& obj : objects) {
        if(!obj)
            continue;
        auto obj3d = std::dynamic_pointer_cast<Object::ObjectBase3D>(obj);
        if(!obj3d || !obj3d->IsVisible() || !obj3d->IsCastShadow())
            continue;    // 3Dオブジェクトかつ影ONかつ表示中のものだけシャドウマップへ描画する

        obj3d->Draw();    // 既存のOnDraw()をそのまま使う。ここでの描画先はシャドウマップの深度バッファになる
    }

    shadowMap.EndRecord();

    // 以降（GeometryPass）の通常描画がこのシャドウマップによる影を受け取れるようにする
    shadowMap.BeginUse();
}

void TransparentPass::Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera)
{
    (void)objects;
    (void)camera;
    // TODO: 半透明オブジェクトのソート・アルファブレンド描画をここに実装する
}

void PostProcessPass::Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera)
{
    (void)objects;
    (void)camera;

    int sceneColorHandle = RenderTargetManager::GetInstance().GetHandle(m_sourceTargetName);
    if(sceneColorHandle == -1)
        return;    // GeometryPass側が未作成・失敗している場合は何もしない

    // Material未設定 or シェーダー読み込み失敗時は Bind() が false を返すだけなので、
    // その場合はシェーダーなし（そのままの色）で全画面に描画するだけになる。
    bool useShader = m_material && m_material->Bind();
    if(useShader) {
        // 2D描画(ToShader系)は DxLib 既定の頂点シェーダーを使うため、独自VSは解除する。
        // （独自VSの入力は VS_INPUT_3D で、VERTEX2DSHADER とは頂点レイアウトが違う）
        SetUseVertexShader(-1);

        PostProcessParams::GetInstance().UpdateAndBind();    // 影の閾値・減色レベル（ImGuiで調整可）を b3 へ転送
        SetUseTextureToShader(0, sceneColorHandle);          // ピクセルシェーダー側の DiffuseTexture(t0) にSceneColorを渡す

        DrawGraphWithShader(sceneColorHandle);

        SetUseTextureToShader(0, -1);    // テクスチャのバインドを解除
        m_material->Unbind();
    }
    else {
        // SceneColorは画面と同解像度で作成しているため、(0,0)へ等倍描画すれば画面全体を覆う
        DrawGraph(0, 0, sceneColorHandle, FALSE);
    }
}

void FinalPass::Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera)
{
    (void)objects;
    (void)camera;
    // TODO: 最終合成・バックバッファへの転送処理をここに実装する
}

}    // namespace Graphics
