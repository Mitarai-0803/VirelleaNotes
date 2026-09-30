#pragma once
//============================================================================
//! @file   ShaderManager.h
//! @brief  Shader の読み込み・名前付き管理を行うマネージャークラスの宣言
//! @details PhysicsManager と同様にシングルトンで一元管理する。
//! @author レオ
//============================================================================
#include "Shader.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace Graphics {

//-----------------------------------------------------------
//! @class ShaderManager
//! @brief Shader を名前で管理するマネージャー（シングルトン）
//-----------------------------------------------------------
class ShaderManager
{
public:
    //! @return シングルトンインスタンス
    static ShaderManager& GetInstance();

    //-----------------------------------------------------------
    //! @brief シェーダーを読み込み、名前を付けて管理する
    //! @details 既に同名で読み込み済みの場合はそれを返す（重複読み込みしない）
    //! @param name 管理名
    //! @param vertexShaderPath 頂点シェーダーファイルパス
    //! @param pixelShaderPath ピクセルシェーダーファイルパス
    //! @return 読み込んだ（または既存の）Shader。失敗時は nullptr
    //-----------------------------------------------------------
    std::shared_ptr<Shader> Load(const std::string& name, const std::string& vertexShaderPath, const std::string& pixelShaderPath);

    //-----------------------------------------------------------
    //! @param name 管理名
    //! @return 見つかれば Shader、なければ nullptr
    //-----------------------------------------------------------
    std::shared_ptr<Shader> Get(const std::string& name) const;

    //! @brief 管理している全 Shader を解放する
    void ReleaseAll();

private:
    ShaderManager()  = default;
    ~ShaderManager() = default;

    ShaderManager(const ShaderManager&)            = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;    //!< 管理名 -> Shader
};

}    // namespace Graphics
