#pragma once
//============================================================================
//! @file   Functions.h
//! @brief  ユーティリティ関数群の宣言
//! @author レオ
//============================================================================
#include "DxMain.h"
#include <string>
#include <numbers>
#include <math.h>
#include <algorithm>
#include "hlsl++.h"

// ※ using namespace はヘッダに書くと全インクルード先に汚染されるため .cpp 側で宣言する

namespace MyLibrary
{
    //---------------------------------------------------------------
    //! @brief ミリ秒を秒に変換
    //! @param milli ミリ秒
    //! @return 秒表記の浮動小数点値
    //---------------------------------------------------------------
    float ConvertMilliToSec(int milli);

    //---------------------------------------------------------------
    //! @brief ミリ秒を分に変換
    //! @param milli ミリ秒
    //! @return 分表記の浮動小数点値
    //---------------------------------------------------------------
    float ConvertMilliToMinute(int milli);

    //---------------------------------------------------------------
    //! @brief ファイルパスから拡張子を取得
    //! @param filepath ファイルパス（nullptr可）
    //! @return 拡張子文字列（見つからない場合は空文字）
    //---------------------------------------------------------------
    std::string GetFileExtension(const char* filepath);

    //---------------------------------------------------------------
    //! @brief オブジェクトの向き（ラジアン）を求める（回転補間あり）
    //! @param now_pos 現在位置
    //! @param goal_pos 目標位置
    //! @param dir 現在の角度（ラジアン）
    //! @param rot_speed 回転速度（度/フレーム）
    //! @return 移動後の向き（ラジアン）
    //---------------------------------------------------------------
    float ObjectPointToDirection(hlslpp::float2 now_pos, hlslpp::float2 goal_pos, float dir, float rot_speed);

    //---------------------------------------------------------------
    //! @brief カメラの停止制御（指定範囲外ではスクロールしない）
    //! @param pos_x 対象 X 座標
    //! @param pos_y 対象 Y 座標
    //! @param w 対象幅
    //! @param h 対象高さ
    //! @param[in,out] cam_x カメラ X（参照で更新）
    //! @param[in,out] cam_y カメラ Y（参照で更新）
    //---------------------------------------------------------------
    void CameraStop(float pos_x, float pos_y, float w, float h, float& cam_x, float& cam_y);

    //---------------------------------------------------------------
    //! @brief 位置を指定エリア内に制限します
    //! @param[in,out] target_pos 対象位置（参照で更新）
    //! @param target_w 対象幅
    //! @param target_h 対象高さ
    //! @param area_w エリア幅
    //! @param area_h エリア高さ
    //---------------------------------------------------------------
    void ClampPositionToArea(hlslpp::float2& target_pos, int target_w, int target_h, float area_w, float area_h);

    //---------------------------------------------------------------
    //! @brief 点と矩形の当たり判定（浮動小数）
    //! @param point 判定点
    //! @param pos 矩形左上位置
    //! @param size 矩形サイズ
    //! @return 当たっていれば true
    //---------------------------------------------------------------
    bool CheckPointBoxHitF(const hlslpp::float2& point, const hlslpp::float2& pos, const hlslpp::float2& size);

    //---------------------------------------------------------------
    //! @brief 点と矩形の当たり判定（座標指定）
    //! @param point_x 点の X
    //! @param point_y 点の Y
    //! @param box_x 矩形左上 X
    //! @param box_y 矩形左上 Y
    //! @param box_w 矩形幅
    //! @param box_h 矩形高
    //! @return 当たっていれば true
    //---------------------------------------------------------------
    bool CheckPointBoxHit(float point_x, float point_y, float box_x, float box_y, float box_w, float box_h);

    //---------------------------------------------------------------
    //! @brief 矩形同士の当たり判定
    //! @param x1 矩形1の左上 X
    //! @param y1 矩形1の左上 Y
    //! @param w1 矩形1の幅
    //! @param h1 矩形1の高さ
    //! @param x2 矩形2の左上 X
    //! @param y2 矩形2の左上 Y
    //! @param w2 矩形2の幅
    //! @param h2 矩形2の高さ
    //! @return 当たっていれば true
    //---------------------------------------------------------------
    bool CheckBoxHit(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

    //---------------------------------------------------------------
    //! @brief 円同士の当たり判定
    //! @param x1 円1の中心 X
    //! @param y1 円1の中心 Y
    //! @param r1 円1の半径
    //! @param x2 円2の中心 X
    //! @param y2 円2の中心 Y
    //! @param r2 円2の半径
    //! @return 当たっていれば true
    //---------------------------------------------------------------
    bool CheckCircleHit(float x1, float y1, float r1, float x2, float y2, float r2);

    //---------------------------------------------------------------
    //! @brief 点と円の当たり判定
    //! @param point_x 点の X
    //! @param point_y 点の Y
    //! @param circle_x 円の中心 X
    //! @param circle_y 円の中心 Y
    //! @param circle_r 円の半径
    //! @return 当たっていれば true
    //---------------------------------------------------------------
    bool CheckPointCircleHit(float point_x, float point_y, float circle_x, float circle_y, float circle_r);

    //---------------------------------------------------------------
    //! @brief 円を描画（XZ平面）
    //! @param center 中心位置
    //! @param radius 半径
    //! @param color 色
    //! @param fill 塗りつぶしフラグ
    //---------------------------------------------------------------
    void DrawCircle3D_XZ(hlslpp::float3 center, float radius, int color, bool fill = false);

    //---------------------------------------------------------------
    //! @brief 矩形を描画（XZ方向）
    //! @param center 中心位置
    //! @param half_w 半分の幅
    //! @param half_h 半分の高さ
    //! @param color 色
    //! @param fill 塗りつぶしフラグ
    //---------------------------------------------------------------
    void DrawBox3D_XZ(hlslpp::float3 center, float half_w, float half_h, int color, bool fill = false);

    //-----------------------------------------------------------
    //! @brief XZ 平面上の円同士の当たり判定を行なう
    //! @param center1 円 1 の中心位置
    //! @param radius1 円 1 の半径
    //! @param center2 円 2 の中心位置
    //! @param radius2 円 2 の半径
    //! @return 当たっていれば true
    //-----------------------------------------------------------
    bool CheckCircleXZHit(hlslpp::float3& center1, float radius1, hlslpp::float3& center2, float radius2);

    //-----------------------------------------------------------
    //! @brief 球同士の当たり判定を行なう
    //! @param center1 球 1 の中心位置
    //! @param radius1 球 1 の半径
    //! @param center2 球 2 の中心位置
    //! @param radius2 球 2 の半径
    //! @return 当たっていれば true
    //-----------------------------------------------------------
    bool CheckBallHit(hlslpp::float3& center1, float radius1, hlslpp::float3& center2, float radius2);

    //-----------------------------------------------------------
    //! @brief 3D の AABB 同士の当たり判定を行なう
    //! @param box_pos1 ボックス 1 の中心位置
    //! @param box_size1 ボックス 1 の半サイズ
    //! @param box_pos2 ボックス 2 の中心位置
    //! @param box_size2 ボックス 2 の半サイズ
    //! @return 当たっていれば true
    //-----------------------------------------------------------
    bool CheckBoxHit3D(hlslpp::float3& box_pos1, hlslpp::float3& box_size1, hlslpp::float3& box_pos2, hlslpp::float3& box_size2);

    //-----------------------------------------------------------
    //! @brief 点と 3D ボックスの当たり判定を行なう
    //! @param point 判定対象の点
    //! @param box_pos ボックスの中心位置
    //! @param box_size ボックスの半サイズ
    //! @return 当たっていれば true
    //-----------------------------------------------------------
    bool CheckPointBoxHit3D(hlslpp::float3& point, hlslpp::float3& box_pos, hlslpp::float3& box_size);

    //-----------------------------------------------------------
    //! @brief 球と AABB（ボックス）の当たり判定を行なう
    //! @param ball_pos 球の中心位置
    //! @param ball_radius 球の半径
    //! @param box_pos ボックスの中心位置
    //! @param box_size ボックスの半サイズ
    //! @return 当たっていれば true
    //-----------------------------------------------------------
    bool CheckBallBoxHit(hlslpp::float3& ball_pos, float ball_radius, hlslpp::float3& box_pos, hlslpp::float3& box_size);

    //-----------------------------------------------------------
    //! @brief 線分と点の最近接位置を取得します（3D）
    //! @param line_start 線分の開始位置
    //! @param line_goal 線分の終了位置
    //! @param point 判定対象の点
    //! @return 線分上の最近接位置（float3）
    //-----------------------------------------------------------
    hlslpp::float3 GetFloat3LinePointNearPosition(hlslpp::float3& line_start, hlslpp::float3& line_goal, hlslpp::float3& point);

    //-----------------------------------------------------------
    //! @brief 線分と点の最近接距離を返す（3D）
    //! @param line_start 線分の開始位置
    //! @param line_goal 線分の終了位置
    //! @param point 判定対象の点
    //! @return 最近接距離
    //-----------------------------------------------------------
    float GetFloat3LinePointNearDistance(hlslpp::float3& line_start, hlslpp::float3& line_goal, hlslpp::float3& point);

    //-----------------------------------------------------------
    //! @brief 線分と球の当たり判定を行なう（3D）
    //! @param line_start 線分の開始位置
    //! @param line_goal 線分の終了位置
    //! @param ball_pos 球の中心位置
    //! @param ball_radius 球の半径
    //! @return 当たっていれば true
    //-----------------------------------------------------------
    bool CheckLineBallHit(hlslpp::float3& line_start, hlslpp::float3& line_goal, hlslpp::float3& ball_pos, float ball_radius);

    //-----------------------------------------------------------
    //! @brief ２つの float2 の距離を求める
    //! @param pos1 1つ目の座標
    //! @param pos2 2つ目の座標
    //! @return 2点間の距離
    //-----------------------------------------------------------
    float GetFloat2Distance(hlslpp::float2& pos1, hlslpp::float2& pos2);

    //-----------------------------------------------------------
    //! @brief ２つの float2 の内積を求める
    //! @param v1 1つ目のベクトル
    //! @param v2 2つ目のベクトル
    //! @return 内積（float）
    //-----------------------------------------------------------
    float GetFloat2Dot(hlslpp::float2& v1, hlslpp::float2& v2);

    //-----------------------------------------------------------
    //! @brief ２つの float2 の外積を求める
    //! @param v1 1つ目のベクトル
    //! @param v2 2つ目のベクトル
    //! @return 外積（スカラー）
    //-----------------------------------------------------------
    float GetFloat2Cross(hlslpp::float2& v1, hlslpp::float2& v2);

    //-----------------------------------------------------------
    //! @brief ２つの float3 の距離を求める
    //! @param pos1 1つ目の座標
    //! @param pos2 2つ目の座標
    //! @return 2点間の距離（float）
    //-----------------------------------------------------------
    float GetFloat3Distance(hlslpp::float3& pos1, hlslpp::float3& pos2);

    //-----------------------------------------------------------
    //! @brief ２つの float3 の内積を求める
    //! @param v1 1つ目のベクトル
    //! @param v2 2つ目のベクトル
    //! @return 内積（float）
    //-----------------------------------------------------------
    float GetFloat3Dot(hlslpp::float3& v1, hlslpp::float3& v2);

    //-----------------------------------------------------------
    //! @brief ２つの float3 の外積を求める
    //! @param v1 1つ目のベクトル
    //! @param v2 2つ目のベクトル
    //! @return 外積（float3）
    //-----------------------------------------------------------
    hlslpp::float3 GetFloat3Cross(hlslpp::float3& v1, hlslpp::float3& v2);

    //-----------------------------------------------------------
    //! @brief float3 を MATRIX で変換した float3 を返す
    //! @param v 変換するベクトル
    //! @param mat 変換行列
    //! @return 変換後の float3
    //-----------------------------------------------------------
    hlslpp::float3 GetFloat3VTransform(hlslpp::float3& v, MATRIX& mat);

    //! @brief 平面上の円同士の当たり判定を行なう（float2 版）
    bool CheckCircleHit(hlslpp::float2& center1, float radius1, hlslpp::float2& center2, float radius2);

    //! @brief 点と円の当たり判定を行なう（float2 版）
    bool CheckPointCircleHit(hlslpp::float2& point, hlslpp::float2& center, float radius);

    //! @brief 四角同士の当たり判定を行なう（float2 版）
    bool CheckBoxHit(hlslpp::float2& box_pos1, hlslpp::float2& box_size1, hlslpp::float2& box_pos2, hlslpp::float2& box_size2);

    //! @brief 点と四角の当たり判定を行なう（float2 版）
    bool CheckPointBoxHit(hlslpp::float2& point, hlslpp::float2& box_pos, hlslpp::float2& box_size);

    //! @brief 円と四角の当たり判定を行なう（float2 版）
    bool CheckCircleBoxHit(hlslpp::float2& circle, float radius, hlslpp::float2& box_pos, hlslpp::float2& box_size);

    //! @brief 点と三角形の当たり判定を行なう（2D）
    bool CheckPointTriangleHit(hlslpp::float2& point, hlslpp::float2& triangle_pos1, hlslpp::float2& triangle_pos2, hlslpp::float2& triangle_pos3);

    //! @brief 線分と点の最近接位置を取得します（2D）
    hlslpp::float2 GetFloat2LinePointNearPosition(hlslpp::float2& line_start, hlslpp::float2& line_goal, hlslpp::float2& point);

    //! @brief 線分と点の最近接距離を返す（2D）
    float GetFloat2LinePointNearDistance(hlslpp::float2& line_start, hlslpp::float2& line_goal, hlslpp::float2& point);

    //! @brief 線分と円の当たり判定を行なう（2D）
    bool CheckLineCircleHit(hlslpp::float2& line_start, hlslpp::float2& line_goal, hlslpp::float2& circle_pos, float circle_radius);

    //-----------------------------------------------------------
    //! @brief 2 点間の距離を返す
    //! @param x1 点1 の X 座標
    //! @param y1 点1 の Y 座標
    //! @param x2 点2 の X 座標
    //! @param y2 点2 の Y 座標
    //! @return 2 点間の距離（float）
    //-----------------------------------------------------------
    float GetDistance(float x1, float y1, float x2, float y2);

    //! @brief 四角同士の当たり判定を行なう（float 版）
    bool CheckBoxHit(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

    //! @brief 点と四角の当たり判定を行なう（float 版）
    bool CheckPointBoxHit(float point_x, float point_y, float box_x, float box_y, float box_w, float box_h);

    //! @brief 円と四角の当たり判定を行う（float 版）
    bool CheckCircleBoxHit(float circle_x, float circle_y, float circle_r, float box_x, float box_y, float box_w, float box_h);

    //---------------------------------------------------------------
    //! @brief 度をラジアンに変換
    //! @param degree 度
    //! @return ラジアン
    //---------------------------------------------------------------
    float TORADIAN(float degree);

    //---------------------------------------------------------------
    //! @brief ラジアンを度に変換
    //! @param radian ラジアン
    //! @return 度
    //---------------------------------------------------------------
    float TODEGREE(float radian);

    //---------------------------------------------------------------
    //! @brief 正規化（float2）
    //! @param vec 正規化するベクトル
    //! @return 正規化後のベクトル
    //---------------------------------------------------------------
    hlslpp::float2 Normalize(const hlslpp::float2& vec);

    //---------------------------------------------------------------
    //! @brief 正規化（float3）
    //! @param vec 正規化するベクトル
    //! @return 正規化後のベクトル
    //---------------------------------------------------------------
    hlslpp::float3 Normalize(const hlslpp::float3& vec);

    //---------------------------------------------------------------
    //! @brief 回転に沿った長さ（cos）を取得
    //! @param rot 回転（ラジアン）
    //! @return cos(rot)
    //---------------------------------------------------------------
    float GetLengthAlongRotation(float rot);

    //---------------------------------------------------------------
    //! @brief 回転に垂直な長さ（sin）を取得
    //! @param rot 回転（ラジアン）
    //! @return sin(rot)
    //---------------------------------------------------------------
    float GetLengthOppositeRotation(float rot);

} // namespace MyLibrary
