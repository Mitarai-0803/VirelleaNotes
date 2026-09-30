#pragma once
//============================================================================
//! @file   Shader.h
//! @brief  HLSL 頂点/ピクセルシェーダーを扱う Shader クラスの宣言
//! @details
//!  HLSL ソース（.fx）を D3DCompile で実行時にコンパイルし、
//!  DXライブラリの LoadVertexShaderFromMem / LoadPixelShaderFromMem で
//!  バイトコードから直接シェーダーハンドルを作成する。
//!  シェーダーデータは data/Shader 以下に .fx 形式で格納されている前提で、
//!  Load() に渡すパスの拡張子は無視され、内部で常に .fx に置き換えて読み込む。
//!
//!  責務分離: Shader → Material → Renderer → RenderPipeline
//!  Shader は「頂点/ピクセルシェーダーの読み込み・バインドのみ」を担当し、
//!  色などのパラメータは Material が保持する。
//!
//!  @note エントリポイント関数名は "main" 固定、シェーダーモデルは
//!        頂点シェーダーが vs_5_0、ピクセルシェーダーが ps_5_0 固定とする。
//!        コンパイルエラー時は DxLib のログ（OutputDebugStringA 経由）に
//!        エラー内容を出力する。
//! @author レオ
//============================================================================
#include <string>

namespace Graphics {

//-----------------------------------------------------------
//! @class Shader
//! @brief 頂点シェーダー・ピクセルシェーダーのハンドルを保持するクラス
//! @details 読み込み・バインド・解放のみを責務とし、
//!          パラメータの設定は Material 側が行う。
//-----------------------------------------------------------
class Shader
{
public:
    Shader() = default;
    ~Shader();

    //-----------------------------------------------------------
    //! @brief HLSL ソースファイルを読み込み、実行時コンパイルする
    //! @param vertexShaderPath 頂点シェーダーソースファイルパス
    //!                              （拡張子は無視され、内部で .fx に置き換えて読み込む）
    //! @param pixelShaderPath ピクセルシェーダーソースファイルパス
    //!                              （拡張子は無視され、内部で .fx に置き換えて読み込む）
    //! @return true 読み込み成功（両方成功した場合のみ true）
    //-----------------------------------------------------------
    bool Load(const std::string& vertexShaderPath, const std::string& pixelShaderPath);

    //! @brief 読み込み済みハンドルを解放する
    void Release();

    //-----------------------------------------------------------
    //! @brief このシェーダーを描画に使用する状態にする
    //! @details DxLib の SetUseVertexShader / SetUsePixelShader を呼び出す
    //-----------------------------------------------------------
    void Bind() const;

    //! @brief シェーダー使用状態を解除し、固定機能パイプラインに戻す
    void Unbind() const;

    //! @return true なら頂点・ピクセル両シェーダーが読み込み済み
    bool IsValid() const { return m_vertexShaderHandle != -1 && m_pixelShaderHandle != -1; }

    //! @return 頂点シェーダーハンドル（未読み込みなら -1）
    int GetVertexShaderHandle() const { return m_vertexShaderHandle; }

    //! @return ピクセルシェーダーハンドル（未読み込みなら -1）
    int GetPixelShaderHandle() const { return m_pixelShaderHandle; }

private:
    int m_vertexShaderHandle = -1;    //!< 頂点シェーダーハンドル
    int m_pixelShaderHandle  = -1;    //!< ピクセルシェーダーハンドル
};

}    // namespace Graphics
