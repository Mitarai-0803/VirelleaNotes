#pragma once
//============================================================================
//! @file   PostProcessParams.h
//! @brief  ポストエフェクトの調整パラメータ保持とGPU転送の宣言
//! @details
//!  PostProcess_PS.fx の cbuffer cbUserPostProcess (register b3) へ
//!  影の閾値・減色レベルを転送する。値は ImGui（DebugManager）のスライダーから
//!  実行中に書き換えられ、PostProcessPass が毎フレーム Bind() 直後に転送する。
//!  MaterialToon と同じ方式（動的コンスタントバッファ＋PSSetConstantBuffers）で、
//!  DxLibが使用する b0/b1 と、MaterialToon の b2 を避けて b4 を使う。
//!
//!  使い方:
//!   1. DxLib_Init() の後に一度だけ CreateConstantBuffer() を呼ぶ
//!   2. 毎フレーム、Shader::Bind() の直後に UpdateAndBind() を呼ぶ（PostProcessPass が実施）
//!   3. DxLib_End() の前に Release() を呼ぶ
//! @author レオ
//============================================================================
#include "ConstantsEngine.h"
#include <cstddef>

namespace Graphics {

//-----------------------------------------------------------
//! @struct PostProcessConstants
//! @brief PostProcess_PS.fx の cbUserPostProcess と1:1でメモリレイアウトを合わせること
//! @details float4 = 16byte 境界。並び順を変える場合はシェーダー側も合わせて変更する。
//-----------------------------------------------------------
struct PostProcessConstants
{
    float ShadowThreshold = POSTPROCESS_DEFAULT_SHADOW_THRESHOLD;    //!< 影が始まる明るさの閾値
    float ColorLevels     = POSTPROCESS_DEFAULT_COLOR_LEVELS;        //!< 減色レベル（1チャンネルあたりの階調数）
    float Padding[2]      = {0.0f, 0.0f};                            //!< 16byte境界に合わせるための詰め物
};

constexpr std::size_t CONSTANT_BUFFER_ALIGNMENT = 16;    //!< 定数バッファのサイズ境界（byte）
static_assert(sizeof(PostProcessConstants) % CONSTANT_BUFFER_ALIGNMENT == 0, "PostProcessConstants must be a multiple of 16 bytes");

//-----------------------------------------------------------
//! @class PostProcessParams
//! @brief ポストエフェクトの調整パラメータを保持しGPUへ転送するシングルトン
//-----------------------------------------------------------
class PostProcessParams
{
public:
    //! @return 唯一のインスタンス
    static PostProcessParams& GetInstance();

    PostProcessParams(const PostProcessParams&)            = delete;
    PostProcessParams& operator=(const PostProcessParams&) = delete;

    //-----------------------------------------------------------
    //! @brief 定数バッファをGPU上に一度だけ作成する
    //! @details DxLib_Init() の後（DirectX11デバイス生成後）に呼ぶこと。
    //! @return true 成功（作成済みの場合も true）
    //-----------------------------------------------------------
    bool CreateConstantBuffer();

    //-----------------------------------------------------------
    //! @brief 現在の値をGPUへ転送し、PS の register(b4) にバインドする
    //! @details Shader::Bind() の直後、Draw呼び出しの直前に呼ぶこと。
    //!          バッファ未作成時は何もしない。
    //-----------------------------------------------------------
    void UpdateAndBind() const;

    //! @brief 定数バッファを解放する（DxLib_End() より前に呼ぶこと）
    void Release();

    //! @brief 全パラメータを初期値に戻す
    void ResetToDefault() { m_constants = PostProcessConstants(); }

    //! @return 現在のパラメータ（ImGuiのスライダーから直接書き換える用途）
    PostProcessConstants& GetConstants() { return m_constants; }

    //! @return true なら定数バッファ作成済み（GPUへ転送できる状態）
    bool IsValid() const { return m_constantBufferHandle != -1; }

private:
    PostProcessParams()  = default;
    ~PostProcessParams() = default;

    PostProcessConstants                 m_constants;          //!< 現在のパラメータ値
    int                                  m_constantBufferHandle = -1;    //!< DxLib の定数バッファハンドル（未作成時は -1）
};

}    // namespace Graphics
