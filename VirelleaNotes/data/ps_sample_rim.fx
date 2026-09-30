//----------------------------------------------------------------------------
//!	@file	ps_sample_rim.fx
//!	@brief	【サンプル】リムライト（縁取り発光）ピクセルシェーダー
//! @details
//!  法線とカメラ方向の角度から輪郭部分を検出し、指定色で縁取りを加算する。
//!  vs_3d.fx（頂点シェーダー）とセットで使用する。
//!  CameraInfo (b10) は DxLib が自動で埋めてくれる定数バッファで、
//!  eye_position_ （カメラのワールド座標）を利用してリム角度を計算する。
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

// 頂点シェーダー(vs_3d.fx)の出力と一致させる
struct VS_OUTPUT_3D
{
    float4 position_      : SV_Position;
    float3 worldPosition_ : WORLD_POSITION;
    float3 normal_        : NORMAL0;
    float4 diffuse_       : COLOR0;
    float2 uv0_           : TEXCOORD0;
};

typedef VS_OUTPUT_3D PS_INPUT_SAMPLE;

//--------------------------------------------------------------
// カメラ情報（DxLibが自動設定する定数バッファ。ps_model.fxと同じレジスタ）
//--------------------------------------------------------------
cbuffer CameraInfo : register(b10)
{
    matrix mat_view_;
    matrix mat_proj_;
    float3 eye_position_;
    matrix mat_light_view_;
    matrix mat_light_proj_;
};

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_SAMPLE input)
{
    PS_OUTPUT output;

    float3 N = normalize(input.normal_);
    float3 V = normalize(eye_position_ - input.worldPosition_);

    // ベース色（テクスチャがあれば乗算、無ければ頂点カラーのみ）
    float4 baseColor = DiffuseTexture.Sample(DiffuseSampler, input.uv0_) * input.diffuse_;

    //------------------------------------------------------------
    // リムライト計算
    // NdotV が小さい（法線とカメラ方向が直角に近い＝輪郭）ほど強く発光させる
    //------------------------------------------------------------
    float NdotV = saturate(dot(N, V));
    float rim = pow(1.0f - NdotV, 3.0f);    // 指数を大きくすると縁が細くなる

    // サンプルとして固定のシアン系リムカラーを使用。
    // 時間で変化させたい／色をCPUから渡したい場合は、
    // ShaderManager 経由で独自の定数バッファ（SetPSConstBuffer）を
    // 追加するとよい（ShaderRegistry.cpp のコメント参照）。
    const float3 RIM_COLOR = float3(0.3f, 0.8f, 1.0f);

    output.color0_.rgb = baseColor.rgb + RIM_COLOR * rim;
    output.color0_.a   = baseColor.a;

    return output;
}
