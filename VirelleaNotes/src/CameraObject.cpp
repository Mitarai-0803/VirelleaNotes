//============================================================================
//! @file   CameraObject.cpp
//! @brief  カメラオブジェクトクラスの実装
//============================================================================
#include "CameraObject.h"
#include "InputManager.h"
#include "ConstantsEngine.h"
#include "Functions.h"
#include <cmath>

namespace Object
{

CameraObject::CameraObject(VECTOR pos, VECTOR target)
    : m_camPos(pos)
    , m_target(target)
{
    SetName(OBJECT_NAME_CAMERA);

    // 初期状態から球面座標を計算する
    VECTOR diff = VSub(pos, target);
    m_distance = VSize(diff);
    m_yaw   = std::atan2(diff.x, diff.z);
    m_pitch = std::asin(diff.y / m_distance);

    // Application::Init() でエンジン起動時に一度だけ設定していたのと同じ画角（45度）を
    // デフォルト値として保持しておく（OnDraw() で毎フレーム再設定するため）。
    m_fovYRadian = MyLibrary::TORADIAN(45.0f);
}

void CameraObject::OnInit()
{
}

void CameraObject::OnUpdate()
{
    // マウス操作でカメラを回転（m_mouseRotationEnabled が false のシーンでは無効）
    if(m_mouseRotationEnabled) {
        UpdateCameraRotation();
    }

    //// WASD キーでカメラを移動
    //UpdateCameraMovement();
}

void CameraObject::OnDraw()
{
    // RenderPipeline（GeometryPass）がオフスクリーンのレンダーターゲットへ
    // SetDrawScreen() で切り替えるようになったことで、DxLib側の視点・投影設定が
    // キャンセルされてしまう。そのため視点位置だけでなく、画角・クリップ距離も
    // 毎フレームここで再設定し、どの描画先がアクティブでも正しく3D描画されるようにする。
    SetupCamera_Perspective(m_fovYRadian);
    SetCameraNearFar(m_nearZ, m_farZ);
    SetCameraPositionAndTarget_UpVecY(m_camPos, m_target);
    ChangeLightTypeDir(m_lightDirection);
}

void CameraObject::OnEnd()
{
}

//============================================================================
// プライベートメソッド
//============================================================================

void CameraObject::UpdateCameraRotation()
{
    //----------------------------------------------------------
    // マウスの移動量を取得し、視点を回転させる
    // - マウスX移動 → yaw（水平回転）
    // - マウスY移動 → pitch（垂直回転）
    //----------------------------------------------------------
    int deltaX = InputManager::GetMouseDeltaX();
    int deltaY = InputManager::GetMouseDeltaY();

    // マウスが動いているなら回転を適用する
    if (deltaX != 0 || deltaY != 0) {
        m_yaw   -= deltaX * m_mouseSensitivity;    // X移動 = yaw回転（左右）
        m_pitch -= deltaY * m_mouseSensitivity;    // Y移動 = pitch回転（上下）

        // pitch を [-π/2, π/2] に制限して、カメラが天地逆転しないようにする
        constexpr float PI_HALF = 3.14159265f / 2.0f;
        if (m_pitch > PI_HALF)  m_pitch = PI_HALF;
        if (m_pitch < -PI_HALF) m_pitch = -PI_HALF;

        // 球面座標からカメラ位置を再計算する
        float cosYaw   = std::cos(m_yaw);
        float sinYaw   = std::sin(m_yaw);
        float cosPitch = std::cos(m_pitch);
        float sinPitch = std::sin(m_pitch);

        m_camPos.x = m_target.x + m_distance * sinYaw * cosPitch;
        m_camPos.y = m_target.y + m_distance * sinPitch;
        m_camPos.z = m_target.z + m_distance * cosYaw * cosPitch;
    }
}

//============================================================================
// ゲッター / セッター
//============================================================================

void CameraObject::SetCamPos(VECTOR pos)
{
    m_camPos = pos;

    // 新しいカメラ位置から球面座標を再計算
    VECTOR diff = VSub(pos, m_target);
    m_distance = VSize(diff);
    if (m_distance > 0.0f) {
        m_yaw   = std::atan2(diff.x, diff.z);
        m_pitch = std::asin(diff.y / m_distance);
    }
}

void CameraObject::SetTarget(VECTOR target)
{
    // 注視点の変更時も球面座標を再計算
    VECTOR diff = VSub(m_camPos, target);
    m_distance = VSize(diff);
    if (m_distance > 0.0f) {
        m_yaw   = std::atan2(diff.x, diff.z);
        m_pitch = std::asin(diff.y / m_distance);
    }
    m_target = target;
}

}    // namespace Object
