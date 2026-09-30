//============================================================================
//! @file   Shader.cpp
//! @brief  Shader クラスの実装
//============================================================================
#include "Shader.h"
#include <DxLib.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <fstream>
#include <format>
#include <vector>

// シェーダーコンパイラのリンク
#pragma comment(lib, "d3dcompiler.lib")

namespace Graphics {

namespace {

//-----------------------------------------------------------
//! @brief HLSL ソースファイルをバイナリとして読み込む
//! @param path 読み込むファイルパス
//! @param[out] out 読み込んだ内容
//! @return true 読み込み成功
//-----------------------------------------------------------
bool ReadFileBinary(const std::string& path, std::vector<char>& out)
{
    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
    if(!file.is_open()) {
        OutputDebugStringA(("[Shader] file not found: " + path + "  (exeの作業ディレクトリ直下に data/Shader があるか確認)\n").c_str());
        return false;
    }

    auto size = file.tellg();
    out.resize(static_cast<size_t>(size));

    file.seekg(0, std::ios::beg);
    file.read(out.data(), size);

    // UTF-8 BOM (EF BB BF) が付いていると D3DCompile が
    // "error X3000: Illegal character in shader file" で失敗するため取り除く
    if(out.size() >= 3 && static_cast<unsigned char>(out[0]) == 0xEF && static_cast<unsigned char>(out[1]) == 0xBB && static_cast<unsigned char>(out[2]) == 0xBF) {
        out.erase(out.begin(), out.begin() + 3);
    }

    return true;
}

//-----------------------------------------------------------
//! @brief HLSL ソースを D3DCompile でコンパイルする
//! @param path ソースファイルパス（.fx）
//! @param targetName シェーダーモデル名（"vs_5_0" 等）
//! @param[out] byteCode コンパイル結果のバイトコード
//! @return true コンパイル成功
//-----------------------------------------------------------
bool CompileShaderFromFile(const std::string& path, const char* targetName, Microsoft::WRL::ComPtr<ID3DBlob>& byteCode)
{
    std::vector<char> source;
    if(!ReadFileBinary(path, source)) {
        return false;
    }

    UINT compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(_DEBUG)
    // デバッグ時はシェーダーデバッグ情報を含める
    compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_DEBUG;
#endif
    compileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

    // dxlib_vs.h / dxlib_ps.h が参照する DXLIB_VERSION をシェーダー内マクロとして渡す
    auto dxlibVersionString = std::format("{:#x}", DXLIB_VERSION);

    const D3D_SHADER_MACRO defines[] = {
        {"DXLIB_VERSION", dxlibVersionString.c_str()},
        {          nullptr,                 nullptr},
    };

    Microsoft::WRL::ComPtr<ID3DBlob> errors;

    HRESULT hr = D3DCompile(
        source.data(),                     // ソースコードの先頭アドレス
        source.size(),                     // ソースコードサイズ
        path.c_str(),                      // ソースファイルパス（エラーメッセージ表示用）
        defines,                           // プリプロセッサマクロ定義（DXLIB_VERSION）
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // #include を有効にする
        "main",                            // エントリポイント関数名
        targetName,                        // シェーダーモデル名
        compileFlags,                      // コンパイラフラグ
        0,                                  // コンパイラフラグ2（未使用）
        &byteCode,                         // [out] コンパイル済みバイトコード
        &errors);                          // [out] エラーメッセージ

    if(errors != nullptr) {
        OutputDebugStringA("--------------------\n");
        OutputDebugStringA(static_cast<const char*>(errors->GetBufferPointer()));
        OutputDebugStringA("--------------------\n");
    }

    return SUCCEEDED(hr);
}

//-----------------------------------------------------------
//! @brief パスの拡張子を .fx に置き換える
//! @details data/Shader 以下の実データはすべて .fx 拡張子で
//!          格納されている前提のため、呼び出し側が拡張子なし
//!          （または別拡張子）を渡しても .fx を読みにいく。
//! @param path 元のファイルパス
//! @return 拡張子を .fx に置き換えたパス
//-----------------------------------------------------------
std::string ToFxPath(const std::string& path)
{
    auto dot = path.find_last_of('.');
    auto slash = path.find_last_of("/\\");

    // 拡張子区切りの '.' がファイル名部分にある場合のみ除去する
    // （ディレクトリ名にドットが含まれるケースを誤って切り詰めないため）
    if(dot != std::string::npos && (slash == std::string::npos || dot > slash)) {
        return path.substr(0, dot) + ".fx";
    }

    return path + ".fx";
}

}    // namespace

Shader::~Shader()
{
    Release();
}

bool Shader::Load(const std::string& vertexShaderPath, const std::string& pixelShaderPath)
{
    Release();

    Microsoft::WRL::ComPtr<ID3DBlob> vertexByteCode;
    Microsoft::WRL::ComPtr<ID3DBlob> pixelByteCode;

    const std::string vertexFxPath = ToFxPath(vertexShaderPath);
    const std::string pixelFxPath  = ToFxPath(pixelShaderPath);

    const bool vertexCompiled = CompileShaderFromFile(vertexFxPath, "vs_5_0", vertexByteCode);
    const bool pixelCompiled  = CompileShaderFromFile(pixelFxPath, "ps_5_0", pixelByteCode);

    if(!vertexCompiled || !pixelCompiled) {
        OutputDebugStringA(("[Shader] compile FAILED: " + vertexFxPath + " / " + pixelFxPath + "\n").c_str());
        return false;
    }

    m_vertexShaderHandle = LoadVertexShaderFromMem(vertexByteCode->GetBufferPointer(), static_cast<int>(vertexByteCode->GetBufferSize()));
    m_pixelShaderHandle  = LoadPixelShaderFromMem(pixelByteCode->GetBufferPointer(), static_cast<int>(pixelByteCode->GetBufferSize()));

    if(m_vertexShaderHandle == -1 || m_pixelShaderHandle == -1) {
        // 片方でも失敗した場合は両方解放して未読み込み状態に戻す
        OutputDebugStringA("[Shader] DxLib Load*ShaderFromMem FAILED (DX11モードか / 入力レイアウト不一致でないか)\n");
        Release();
        return false;
    }

    OutputDebugStringA(("[Shader] loaded OK: " + vertexFxPath + " / " + pixelFxPath + "\n").c_str());
    return true;
}

void Shader::Release()
{
    if(m_vertexShaderHandle != -1) {
        DeleteShader(m_vertexShaderHandle);
        m_vertexShaderHandle = -1;
    }
    if(m_pixelShaderHandle != -1) {
        DeleteShader(m_pixelShaderHandle);
        m_pixelShaderHandle = -1;
    }
}

void Shader::Bind() const
{
    if(!IsValid()) return;

    SetUseVertexShader(m_vertexShaderHandle);
    SetUsePixelShader(m_pixelShaderHandle);
}

void Shader::Unbind() const
{
    // -1 を指定すると固定機能パイプラインに戻る
    SetUseVertexShader(-1);
    SetUsePixelShader(-1);
}

}    // namespace Graphics
