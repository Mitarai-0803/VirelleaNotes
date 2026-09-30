//----------------------------------------------------------------------------
//!	@file	ps_model.fx
//!	@brief	MV1モデルピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"
 
static const float PI = 3.141592f;
 
// 頂点シェーダーの出力
struct VS_OUTPUT_MODEL
{
    float4 position_ : SV_Position; //!< 座標       (スクリーン空間)
    float4 curr_position_ : CURR_POSITION; //!< 現在の座標 (スクリーン空間)
    float3 world_position_ : WORLD_POSITION; //!< ワールド座標
    float3 normal_ : NORMAL0; //!< 法線
    float4 diffuse_ : COLOR0; //!< Diffuseカラー
    float2 uv0_ : TEXCOORD0; //!< テクスチャ座標
    float4 prev_position_ : PREV_POSITION; //!< 1フレーム前の座標 (スクリーン空間) ※末尾に追加されているため注意
};
 
typedef VS_OUTPUT_MODEL PS_INPUT_MODEL;
 
//--------------------------------------------------------------
// 定数バッファ
// CPUから受け渡される値。Read-only
//--------------------------------------------------------------
cbuffer CameraInfo : register(b10)
{
    matrix mat_view_; //!< ビュー行列
    matrix mat_proj_; //!< 投影行列
    float3 eye_position_; //!< カメラの位置
    matrix mat_light_view_;
    matrix mat_light_proj_;
};
 
// シャドウマップ用のデプステクスチャ
Texture2D ShadowTexture : register(t16);
 
//  シャドウ補間比較サンプラー
SamplerComparisonState ShadowSampler : register(s15);
 
//--------------------------------------------------------------
// D項 GGX (Trowbridge-Retiz)
//!	@param	[in]	a	ラフネスroughnessの二乗
//--------------------------------------------------------------
float D_GGX(float NdotH, float a)
{
    float a2 = a * a;
    float numerator = a2;
    float denominator = NdotH * NdotH * (a2 - 1.0f) + 1.0f;
 
    denominator = PI * denominator * denominator;
 
    return numerator / denominator;
}
 
float D_GTR1(float NdotH, float a)
{
    if (a >= 1.0f - 0.0001f)
    {
        return 1.0f / PI;
    }
 
    float a2 = a * a;
    float t = 1.0f + (a2 - 1.0f) * NdotH * NdotH;
    return (a2 - 1.0f) / (PI * log(a2) * t);
}
 
 
 
//---------------------------------------------------------------------------
// G項 Smith
//---------------------------------------------------------------------------
float G_Smith_Schlick_GGX(float a, float NdotV, float NdotL)
{
    // Smith schlick-GGX.
    float k = a * 0.5f;
    float GV = NdotV / (NdotV * (1 - k) + k);
    float GL = NdotL / (NdotL * (1 - k) + k);
 
    return GV * GL;
}
 
//---------------------------------------------------------------------------
// Schlickのフレネル近似式
//---------------------------------------------------------------------------
float3 F_Schlick(float3 N, float3 V, float3 specularColor)
{
    float invNdotV = 1.0f - dot(N, V);
    float invNdotV2 = invNdotV * invNdotV;
 
    return specularColor + (1.0f - specularColor) * (invNdotV2 * invNdotV2 * invNdotV);
}
 
float2 calcVogelDiskSampling(int i, int sampleCountMax, float angleBias = 0.0f)
{
    const float GOLDEN_ANGLE = 2.4;//黄金角
    float theta = GOLDEN_ANGLE * float(i) + angleBias; //角度
    float r = sqrt(float(i) + 0.5) / sqrt(float(sampleCountMax));
    return float2(cos(theta), sin(theta)) * r;
}

//---------------------------------------------------------------------------
// 影を判定
//! @return 遮蔽量  0.0f(暗) - 1.0f(明)
//---------------------------------------------------------------------------
float GetShadow(float3 worldPosition, float NdotL)
{
    //----------------------------------------------------------
    // シャドウを描画した時のLightView / LightProj で座標変換
    // シャドウデプス描画時の状態を再現できる
    // lightSpace.xy = シャドウデプスに描画された時のスクリーン座標のXY
    // lightSpace.z  = ワールド座標位置の「光源からの距離」
    //----------------------------------------------------------
    float4 lightSpace = mul(float4(worldPosition, 1.0f), mat_light_view_);
    lightSpace = mul(lightSpace, mat_light_proj_);
 
 
    // スクリーン座標 を UV座標 へ変換
    float2 uv = lightSpace.xy * float2(0.5f, -0.5f) + 0.5f;
 
    if (any(uv < 0.0f) || any(1.0f < uv))
        return NdotL; // 範囲外は影ではない
#if 1
    float shadow = 0.0f;
    const int SAMPLECOUNT_MAX = 32;

    for (int i = 0; i <= SAMPLECOUNT_MAX; ++i)
    {
        float2 offset = calcVogelDiskSampling(i, SAMPLECOUNT_MAX) * (1.0 / 8192.0) * 4*2;
            
            //  GPUの補完と比較機能を利用した高品質シャドウ
        static const float SHADOW_BIAS = 0.0001f;
        shadow += ShadowTexture.SampleCmpLevelZero(ShadowSampler, uv + offset, lightSpace.z - SHADOW_BIAS).r;
    }
    
    shadow *= (1.0f / SAMPLECOUNT_MAX);
    
#else    
    // シャドウデプスを読み込む
    //float depth = ShadowTexture.Sample(DiffuseSampler, uv).r;
    float2 resolution = float2(2048, 2048) * 4;
    //float depth = ShadowTexture.Load(int3(uv * resolution, 0)).r;
    float depth = ShadowTexture.Load(int3(uv * resolution, 0)).r;
    // 影判定  
    float shadow = 1.0f;
    static const float SHADOW_BIAS = 0.0001f;
    if (depth + SHADOW_BIAS < lightSpace.z)
    {
        shadow = 0.0f;  // 影の時
    }
#endif
    // Lambertの結果と比較して暗いほうを採用    
    return min(shadow, NdotL);
}
 
 
//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_MODEL input)
{
    PS_OUTPUT output;
 
    float2 uv = input.uv0_;
    float3 N = normalize(input.normal_); // 法線
 
    float3 V = normalize(eye_position_ - input.world_position_); // カメラに向かうベクトル

	//------------------------------------------------------------
	// 法線マップ
	//------------------------------------------------------------
    N = Normalmap(N, input.world_position_, uv);
 
	//------------------------------------------------------------
	// テクスチャカラーを読み込み
	//------------------------------------------------------------
    float4 textureColor = DiffuseTexture.Sample(DiffuseSampler, uv);
 
	// テクスチャは sRGB → Linearに変換
    float3 albedo = pow(saturate(textureColor.rgb), 2.2); // 材質の反射率 ≒ 物体の色
    float roughness = 0.2f; // ラフさ(粗さ)	0:つるつる ～ 1:ざらざら
    float metallic = 0.0f; // 金属具合		0:非金属   ～ 1:金属
    roughness = max(0.01f, roughness); // ラフネス最小値

    const float reflectance = 0.04f; // デフォルトの反射率 4% (0%～8%)
    float3 specularColor = lerp(float3(reflectance, reflectance, reflectance), albedo, metallic); // スペキュラーの色 (金属の場合はalbedoになる)

    // アルファテスト
    if (textureColor.a < 0.5)
        discard;
 
	//output.color0_ = textureColor; // * input.diffuse_;
 
	
    float3 L = normalize(float3(1, 1, 1)); // 光源の方向
    float3 lightColor = float3(1, 1, 1) * 2 * 3 * 4;
    float3 H = normalize(L + V);
 
    float NdotH = saturate(dot(N, H));
 
	
	//----------------------------------------------------------
	// 拡散反射光 Diffuse
	// 正規化 Lambert model
	//----------------------------------------------------------
    const float kD = 1.0f / PI;
    float diffuse = saturate(dot(N, L)) * kD; // saturateを使うと0サイクル
 
    float3 diffuseTerm = lightColor * albedo * diffuse; // Diffuse項	
 
	//----------------------------------------------------------
	// 鏡面反射光 Specular
    float3 specularTerm = 0;
    if (0)
    {
        float NdotL = saturate(dot(N, L));
        float LdotH = saturate(dot(L, H)) + 0.00001;
 
        // Cook-Torranceモデル最適化版
        // SIGGRAPH2015 Optimizing PBR
        float roughness4 = roughness * roughness * roughness * roughness;
        float numerator = roughness4;
 
        float denominator = NdotH * NdotH * (roughness4 - 1.0f) + 1.0f;
        denominator = 4.0f * PI * denominator * denominator * LdotH * LdotH * (roughness + 0.5f);
        float3 brdf = (numerator / denominator) * specularColor;
 
        float3 specular = brdf * NdotL;
        specularTerm = lightColor * specular;
    }
    else if (1)
    {
        float NdotL = saturate(dot(N, L));
        float NdotV = saturate(dot(N, V));
        float NdotH = saturate(dot(N, H)) + 0.00001;
		//----------------------------------------------------------
		// Cook-Torrance モデル
		//----------------------------------------------------------
        //float  D = D_GGX (NdotH, roughness * roughness); // NDF(Normal Distribution Function / 法線分布項)
        float D = D_GTR1(NdotH, roughness * roughness) * NdotL;
        float G = G_Smith_Schlick_GGX(roughness * roughness, NdotV, NdotL);
        float3 F = F_Schlick(N, L, specularColor); // Fresnel反射
		// BRDF(Bidirectional Reflectance Distribution Function : 双方向反射率分布関数）
        float3 brdf = (D * G * F)
		              / //------------------------
                           (4.0 * NdotL * NdotV);
 
        float3 specular = brdf * NdotL;
        specularTerm = lightColor * specular;
    }
    else
    {
		//----------------------------------------------------------
		// 正規化 Blinn-Phong model
		//----------------------------------------------------------
        float shininess = 300;
		// Roughness → shininess への変換
		// https://computergraphics.stackexchange.com/questions/1515/what-is-the-accepted-method-of-converting-shininess-to-roughness-and-vice-versa
        shininess = 2.0 / (roughness * roughness * roughness * roughness) - 2;
        const float kS = (shininess + 2.0f) * (1.0f / (2.0f * PI));
        float specular = pow(NdotH, shininess) * kS;
        float3 fresnel = F_Schlick(N, V, specularColor);
        specularTerm = lightColor * specular * fresnel;
    }
	//----------------------------------------------------------
	// 環境光 Ambient
	//----------------------------------------------------------
    float3 ambient = float3(0.5, 0.5, 0.5);
    float3 ambientTerm = ambient * albedo;
 
    ambientTerm = 0;
	// metallicが1.0に近づくにつれてdiffuseは減少する
    diffuseTerm *= 1.0f - metallic;
    output.color0_ = float4(diffuseTerm + specularTerm + ambientTerm, 1);
    //----------------------------------------------------------
    // 影
    //----------------------------------------------------------
    float NdotL = saturate(dot(N, L));
    float shadow = GetShadow(input.world_position_, NdotL);
    // 疑似的に影を薄くする
    const float SHADOW_ALPHA = 0.25f;
    shadow = lerp(SHADOW_ALPHA, 1.0f, shadow);
    output.color0_.rgb *= shadow;
	// Linear → sRGBに変換
//  output.color0_.rgb = pow(saturate(output.color0_.rgb), 1.0/2.2);
 
    //output.color0_ = float4(N, 1);
    //output.color0_.rgb = F_Schlick(N, V, specularColor);
 
    
 
	// 出力パラメータを返す
    return output;
}