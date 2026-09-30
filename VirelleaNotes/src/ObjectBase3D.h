#pragma once
//============================================================================
//! @file   ObjectBase3D.h
//! @brief  3Dオブジェクト基底クラス
//! @author レオ
//============================================================================
#include "ObjectBase.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <memory>

// CollisionComponent はグローバル名前空間の前方宣言
class CollisionComponent;

// Material はグローバル名前空間ではなく Graphics 名前空間の前方宣言
// （ヘッダの依存を減らすため、実体は Material.h を参照）
namespace Graphics {
class Material;
}

namespace Object {

class ObjectBase3D : public ObjectBase
{
public:
    //! @brief コンストラクタ
    ObjectBase3D() = default;

    //! @brief デストラクタ
    virtual ~ObjectBase3D() = default;

    //-----------------------------------------------------------
    //! @brief 座標の取得
    //! @details コライダーがあれば物理座標を返す。なければ手動設定の座標を返す。
    //! @return ワールド座標
    //-----------------------------------------------------------
    JPH::RVec3 GetPosition() const;

    //-----------------------------------------------------------
    //! @brief 座標の設定
    //! @details コライダーがあれば物理ボディも同期して移動する。
    //! @param pos 設定するワールド座標
    //-----------------------------------------------------------
    void SetPosition(const JPH::RVec3& pos);

    //-----------------------------------------------------------
    //! @brief 回転の取得
    //! @details コライダーがあれば物理回転を返す。なければ手動設定の回転を返す。
    //! @return 回転（クォータニオン）
    //-----------------------------------------------------------
    JPH::Quat GetRotation() const;

    //-----------------------------------------------------------
    //! @brief 回転の設定
    //! @details コライダーがあれば物理ボディも同期して回転する。
    //! @param rot 設定する回転（クォータニオン）
    //-----------------------------------------------------------
    void SetRotation(const JPH::Quat& rot);

    //! @return 拡大縮小率
    JPH::Vec3 GetScale() const { return m_scale; }

    //! @param scale 設定する拡大縮小率
    void SetScale(const JPH::Vec3& scale) { m_scale = scale; }

    //-----------------------------------------------------------
    //! @return 設定されている Material（未設定なら nullptr）
    //! @details Shader → Material → Renderer → RenderPipeline という
    //!          責務分離のうち、Object 側が保持するのはこの Material 参照のみ。
    //!          実際の描画（Draw()）で Material を使うかどうかは派生クラスの
    //!          OnDraw() 実装に委ねる（既存の Box/Floor/Sphere は未使用のまま）。
    //-----------------------------------------------------------
    std::shared_ptr<Graphics::Material> GetMaterial() const { return m_material; }

    //! @param material 設定する Material
    void SetMaterial(std::shared_ptr<Graphics::Material> material) { m_material = material; }

    //=============================================================
    // シャドウマップ（DxLibのMakeShadowMap系機能によるリアルタイムの影）
    //=============================================================

    //-----------------------------------------------------------
    //! @brief このオブジェクトをシャドウマップに描画する（影を落とす）対象にする
    //! @details ShadowMapPass が毎フレーム、このフラグが true の3Dオブジェクトだけを
    //!          シャドウマップへ描画する（＝光源から見て影を落とす側）。
    //!          影を「受ける」側（床など）はこのフラグを立てる必要はなく、
    //!          SetUseShadowMap() が有効な間に通常描画されたオブジェクトは
    //!          自動的に影を受ける。
    //! @param cast true なら影を落とす対象にする
    //-----------------------------------------------------------
    void SetCastShadow(bool cast = true) { m_castShadow = cast; }

    //! @brief シャドウマップへの描画（影を落とす対象）から外す
    void ClearCastShadow() { m_castShadow = false; }

    //! @return true なら ShadowMapPass がシャドウマップへ描画する（影を落とす）対象
    bool IsCastShadow() const { return m_castShadow; }

protected:
    JPH::RVec3 m_pos   = JPH::RVec3(0.0f, 0.0f, 0.0f);    //!< 手動設定の座標
    JPH::Quat  m_rot   = JPH::Quat::sIdentity();          //!< 手動設定の回転
    JPH::Vec3  m_scale = JPH::Vec3(1.0f, 1.0f, 1.0f);     //!< 拡大縮小率

    std::shared_ptr<Graphics::Material> m_material;    //!< 任意設定の Material（未設定可）

    bool m_castShadow = false;    //!< true なら ShadowMapPass がシャドウマップへ描画する（影を落とす）
};

}    // namespace Object
