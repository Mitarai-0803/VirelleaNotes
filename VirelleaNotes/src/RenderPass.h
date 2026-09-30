#pragma once
//============================================================================
//! @file   RenderPass.h
//! @brief  RenderPipeline を構成する RenderPass 基底クラス・標準パスの宣言
//! @details
//!  RenderPipeline
//!   ├─ ShadowMapPass    （実装済み。DxLibのシャドウマップへ影を落とすオブジェクトの深度を焼き込む）
//!   ├─ GeometryPass     （実装済み。オフスクリーンのレンダーターゲットへ描画する）
//!   ├─ TransparentPass  （雛形のみ。現状は何もしない）
//!   ├─ PostProcessPass  （実装済み。GeometryPassの描画結果を画面全体に貼り直す）
//!   └─ FinalPass        （雛形のみ。現状は何もしない）
//!  のように、後からRenderPassを追加できる構造を目指す。
//!  現段階では ShadowMapPass と GeometryPass と PostProcessPass を RenderPipeline のデフォルト構成に含める。
//! @author レオ
//============================================================================
#include "ObjectBase.h"
#include "CameraObject.h"
#include "RenderTargetManager.h"
#include "Material.h"
#include <memory>
#include <string>
#include <vector>

namespace Graphics {

//-----------------------------------------------------------
//! @class RenderPass
//! @brief 描画パスの基底クラス
//! @details 派生クラスは Execute() で「このパスが何を描画するか」を実装する
//-----------------------------------------------------------
class RenderPass
{
public:
    //! @param name パス名（デバッグ表示用）
    explicit RenderPass(const std::string& name) : m_name(name) {}
    virtual ~RenderPass() = default;

    //-----------------------------------------------------------
    //! @brief このパスの描画処理を実行する
    //! @param objects 描画候補オブジェクト一覧（Scene が保持するもの）
    //! @param camera 描画に使用するカメラ（LayerMask 判定に使用）
    //-----------------------------------------------------------
    virtual void Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera) = 0;

    //! @return パス名
    const std::string& GetName() const { return m_name; }

private:
    std::string m_name;    //!< パス名
};

//-----------------------------------------------------------
//! @class GeometryPass
//! @brief 通常のジオメトリ描画パス
//! @details
//!  カメラの LayerMask に含まれ、かつ IsVisible() が true のオブジェクトのみ
//!  Draw() を呼び出す。既存オブジェクトの OnDraw() 実装（DxLib への直接描画）
//!  はそのまま活用され、変更の必要はない。
//!  ポストエフェクトを画面全体に適用できるよう、バックバッファへ直接描かず
//!  指定されたオフスクリーンのレンダーターゲットへ描画する。
//-----------------------------------------------------------
class GeometryPass : public RenderPass
{
public:
    //-----------------------------------------------------------
    //! @param targetName 描画先レンダーターゲットの管理名
    //!                        （RenderTargetManager::GetInstance() に事前に CreateRenderTarget 済みであること）
    //-----------------------------------------------------------
    explicit GeometryPass(std::string targetName)
        : RenderPass("GeometryPass")
        , m_targetName(std::move(targetName))
    {
    }

    void Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera) override;

private:
    std::string m_targetName;    //!< 描画先レンダーターゲットの管理名（RenderTargetManager::GetInstance() 上の名前）
};

//-----------------------------------------------------------
//! @class ShadowMapPass
//! @brief シャドウマップへの深度描画パス
//! @details
//!  DxLibのシャドウマップ機能（MakeShadowMap 等）を使用する。
//!  ObjectBase3D::IsCastShadow() が true の3Dオブジェクトだけを対象に、
//!  Graphics::ShadowMapManager 経由でライト視点の深度をシャドウマップへ焼き込み、
//!  最後に「以降の3D描画でこのシャドウマップを使う」設定を有効にする。
//!  このパスの直後に実行される GeometryPass の通常描画（DrawCube3D 等）が、
//!  自動的にこのシャドウマップによる影を受け取るようになる。
//-----------------------------------------------------------
class ShadowMapPass : public RenderPass
{
public:
    ShadowMapPass() : RenderPass("ShadowMapPass") {}
    void Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera) override;
};

//-----------------------------------------------------------
//! @class TransparentPass
//! @brief 半透明オブジェクト描画パス（雛形）
//-----------------------------------------------------------
class TransparentPass : public RenderPass
{
public:
    TransparentPass() : RenderPass("TransparentPass") {}
    void Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera) override;
};

//-----------------------------------------------------------
//! @class PostProcessPass
//! @brief ポストプロセス描画パス
//! @details
//!  GeometryPass が描いたオフスクリーンレンダーターゲットを1枚のテクスチャとして
//!  読み込み、画面全体に貼り直す。ここに設定した Material（シェーダー）が
//!  オブジェクトの設定に関係なく画面全体に一括で適用される。
//!  Material が未設定、またはシェーダーの読み込みに失敗している場合は、
//!  シェーダーなし（そのままの色）で全画面に描画するだけになる。
//-----------------------------------------------------------
class PostProcessPass : public RenderPass
{
public:
    //-----------------------------------------------------------
    //! @param sourceTargetName 読み込み元レンダーターゲットの管理名
    //!                              （GeometryPassの描画先と同じ名前にすること）
    //-----------------------------------------------------------
    explicit PostProcessPass(std::string sourceTargetName)
        : RenderPass("PostProcessPass")
        , m_sourceTargetName(std::move(sourceTargetName))
    {
    }

    //-----------------------------------------------------------
    //! @brief 画面全体に適用するシェーダー（Material）を設定する
    //! @param material 設定する Material（nullptrなら効果なしの素通しになる）
    //-----------------------------------------------------------
    void SetMaterial(std::shared_ptr<Material> material) { m_material = std::move(material); }

    void Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera) override;

private:
    std::string               m_sourceTargetName;    //!< 読み込み元レンダーターゲットの管理名（RenderTargetManager::GetInstance() 上の名前）
    std::shared_ptr<Material> m_material;            //!< 画面全体に適用するシェーダー（未設定可）
};

//-----------------------------------------------------------
//! @class FinalPass
//! @brief 最終合成描画パス（雛形）
//-----------------------------------------------------------
class FinalPass : public RenderPass
{
public:
    FinalPass() : RenderPass("FinalPass") {}
    void Execute(const std::vector<std::shared_ptr<Object::ObjectBase>>& objects, const Object::CameraObject& camera) override;
};

}    // namespace Graphics
