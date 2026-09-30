#pragma once
//============================================================================
//! @file   CameraObject.h
//! @brief  カメラオブジェクトクラスの宣言
//! @details
//!  マウス移動でカメラ視点を回転させ、WASD キーでカメラを移動できる。
//!  注視点を中心に回転する球面座標ベースの実装。
//! @author レオ
//============================================================================
#include "ObjectBase3D.h"
#include "Layer.h"
#include <DxLib.h>

namespace Object
{

//-----------------------------------------------------------
//! @class CameraObject
//! @brief マウス操作で視点回転、WASD キーで移動できるカメラオブジェクト
//-----------------------------------------------------------
class CameraObject : public ObjectBase3D
{
public:
    //-----------------------------------------------------------
    //! @param pos 初期カメラ位置
    //! @param target 初期注視点
    //-----------------------------------------------------------
    CameraObject(VECTOR pos, VECTOR target);
    virtual ~CameraObject() = default;

    //=============================================================
    // ゲッター / セッター
    //=============================================================

    //! @param pos 新しいカメラ位置
    void SetCamPos(VECTOR pos);

    //! @return 現在のカメラ位置
    VECTOR GetCamPos() const { return m_camPos; }

    //! @param target 新しい注視点
    void SetTarget(VECTOR target);

    //! @return 現在の注視点
    VECTOR GetTarget() const { return m_target; }

    //! @param speed 移動速度（m/秒）
    void SetMoveSpeed(float speed) { m_moveSpeed = speed; }

    //! @param sensitivity マウス移動1ピクセルあたりの回転角度（ラジアン）
    void SetMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }

    //-----------------------------------------------------------
    //! @param enabled false にすると、マウス移動によるカメラ回転を
    //!             行わなくなる（固定視点にしたいシーン向け）。デフォルトは true。
    //-----------------------------------------------------------
    void SetMouseRotationEnabled(bool enabled) { m_mouseRotationEnabled = enabled; }

    //! @param fovYRadian 縦画角（ラジアン）
    void SetPerspective(float fovYRadian) { m_fovYRadian = fovYRadian; }

    //-----------------------------------------------------------
    //! @param nearZ 近クリップ距離
    //! @param farZ 遠クリップ距離
    //-----------------------------------------------------------
    void SetNearFar(float nearZ, float farZ) { m_nearZ = nearZ; m_farZ = farZ; }

    //! @param direction 平行光源の方向
    void SetLightDirection(VECTOR direction) { m_lightDirection = direction; }

    //=============================================================
    // LayerMask（このカメラがどのレイヤーを描画するか）
    //=============================================================

    //-----------------------------------------------------------
    //! @return このカメラの LayerMask
    //! @details デフォルトは全レイヤー有効（Layer::LayerMask::All()）。
    //!          既存シーンのカメラはこのデフォルトのまま使えば、
    //!          今まで通り全オブジェクトが描画される。
    //-----------------------------------------------------------
    const Layer::LayerMask& GetLayerMask() const { return m_layerMask; }

    //! @param mask 設定する LayerMask
    void SetLayerMask(const Layer::LayerMask& mask) { m_layerMask = mask; }

protected:
    void OnInit()   override;
    void OnUpdate() override;
    void OnDraw()   override;
    void OnEnd()    override;

private:
    //! マウス入力を読み取り、注視点を中心とした球面座標でカメラを回転させる
    void UpdateCameraRotation();

    //! WASD キー入力でカメラを移動させる
    void UpdateCameraMovement();

    VECTOR m_camPos;              //!< カメラ位置
    VECTOR m_target;              //!< 注視点

    // 球面座標用（回転計算用）
    float  m_distance = 10.0f;    //!< カメラと注視点の距離
    float  m_yaw   = 0.0f;        //!< 水平回転（ラジアン）
    float  m_pitch = 0.5f;        //!< 垂直回転（ラジアン）

    float  m_moveSpeed        = 5.0f;      //!< WASD 移動速度（m/秒）
    float  m_mouseSensitivity = 0.005f;    //!< マウス感度（ラジアン/ピクセル）
    bool   m_mouseRotationEnabled = true;  //!< true の間はマウス移動でカメラが回転する

    // 透視投影パラメータ（Application::Init() で一度だけ設定していたものと同じ値をデフォルトとする）。
    // RenderPipeline 導入により GeometryPass が SetDrawScreen() で描画先を毎フレーム
    // 切り替えるようになったが、DxLib は SetDrawScreen() で描画先を切り替えると
    // カメラ（視点・投影）の設定がキャンセルされてしまうため、OnDraw() で視点位置と
    // 合わせて毎フレーム再設定する。
    float  m_fovYRadian = 0.0f;      //!< 縦画角（ラジアン）。コンストラクタで45度相当を設定
    float  m_nearZ      = 0.1f;      //!< 近クリップ距離
    float  m_farZ       = 1000.0f;   //!< 遠クリップ距離
    VECTOR m_lightDirection = VGet(0.8f, -1.2f, 1.0f);    //!< 平行光源の方向（Application::Init() と同じデフォルト値）

    Layer::LayerMask m_layerMask;    //!< このカメラが描画するレイヤー（デフォルト: 全レイヤー）
};

}    // namespace Object
