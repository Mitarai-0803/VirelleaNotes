//============================================================================
//! @file   Functions.cpp
//! @brief  ユーティリティ関数群の実装
//! @author レオ
//============================================================================
#include "Functions.h"
#include "ConstantsGame.h"
#include "VectorMath.h"

namespace MyLibrary
{
	float ConvertMilliToSec(int milli)
	{
		return static_cast<float>(milli) / static_cast<float>(MS_PER_SEC);
	}

	float ConvertMilliToMinute(int milli)
	{
		return static_cast<float>(milli) / static_cast<float>(MS_PER_MIN); // 1分 = 60秒 * 1000ミリ秒
	}

	std::string GetFileExtension(const char* filepath)
	{
		if (filepath == nullptr) return "";

		std::string path(filepath);
		size_t pos = path.rfind('.');
		if (pos == std::string::npos || pos == path.length() - 1) return "";
		return path.substr(pos + 1);
	}

	float ObjectPointToDirection(float2 now_pos, float2 goal_pos, float dir, float rot_speed)
	{
		float now_dir = TODEGREE(dir); // ラジアン → 度
		float dx = goal_pos.x - now_pos.x;
		float dy = goal_pos.y - now_pos.y;
		float goal_dir = TODEGREE(atan2f(dy, dx)) + 90.0f; // 目標角度

		float diff = goal_dir - now_dir;
		while (diff >= 180.0f) diff -= 360.0f;
		while (diff < -180.0f) diff += 360.0f;

		if (diff > rot_speed) now_dir += rot_speed;
		else if (diff < -rot_speed) now_dir -= rot_speed;
		else now_dir = goal_dir;

		return TORADIAN(now_dir); // 度 → ラジアン
	}

	void CameraStop(float pos_x, float pos_y, float w, float h, float& cam_x, float& cam_y)
	{
		if (pos_x > HALF_SCREEN_W && pos_x < w - HALF_SCREEN_W)
			cam_x = HALF_SCREEN_W - pos_x;

		if (pos_y > HALF_SCREEN_H && pos_y < h - HALF_SCREEN_H)
			cam_y = HALF_SCREEN_H - pos_y;
	}

	void ClampPositionToArea(float2& target_pos, int target_w, int target_h, float area_w, float area_h)
	{
		target_pos.x = max(static_cast<float>(target_w) * HALF, min(target_pos.x, area_w - target_w * HALF));
		target_pos.y = max(static_cast<float>(target_h) * HALF, min(target_pos.y, area_h - target_h * HALF));
	}

	bool CheckPointBoxHitF(const float2& point, const float2& pos, const float2& size)
	{
		return (point.x >= pos.x && point.x <= pos.x + size.x) &&
			(point.y >= pos.y && point.y <= pos.y + size.y);
	}

	bool CheckPointBoxHit(float point_x, float point_y, float box_x, float box_y, float box_w, float box_h)
	{
		return (point_x >= box_x && point_x <= box_x + box_w) &&
			(point_y >= box_y && point_y <= box_y + box_h);
	}

	bool CheckBoxHit(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
	{
		return (x1 + w1 >= x2 && x1 <= x2 + w2) &&
			(y1 + h1 >= y2 && y1 <= y2 + h2);
	}

	bool CheckCircleHit(float x1, float y1, float r1, float x2, float y2, float r2)
	{
		float dist = static_cast<float>(sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
		return dist <= (r1 + r2);
	}

	bool CheckPointCircleHit(float point_x, float point_y, float circle_x, float circle_y, float circle_r)
	{
		float dist = static_cast<float>(sqrt((point_x - circle_x) * (point_x - circle_x) + (point_y - circle_y) * (point_y - circle_y)));
		return dist <= circle_r;
	}

	void DrawCircle3D_XZ(float3 center, float radius, int color, bool fill)
	{
		VECTOR pos1;
		VECTOR pos2;
		for (int i = 0; i < 36; i++) {
			pos1.x = center.x + radius * sinf(TORADIAN(i * 10.0f));
			pos1.y = center.y;
			pos1.z = center.z + radius * cosf(TORADIAN(i * 10.0f));

			pos2.x = center.x + radius * sinf(TORADIAN((i + 1) * 10.0f));
			pos2.y = center.y;
			pos2.z = center.z + radius * cosf(TORADIAN((i + 1) * 10.0f));

			if (fill) {
				DrawTriangle3D(cast(center), pos1, pos2, color, TRUE);
			}
			else {
				DrawLine3D(pos1, pos2, color);
			}
		}
	}

	// ---------------------------------------------------------------
	//! @brief 矩形を描画（XZ方向）
	//! @param  [in]     center     中心位置
	//! @param  [in]     half_w     半分の幅
	//! @param  [in]     half_h     半分の高さ
	//! @param  [in]     color      色
	//! @param  [in]     fill       塗りつぶしフラグ
	//----------------------------------------------------------------
	void DrawBox3D_XZ(float3 center, float half_w, float half_h, int color, bool fill)
	{
		VECTOR pos1, pos2, pos3, pos4;

		pos1.x = center.x + half_w;
		pos1.y = center.y;
		pos1.z = center.z + half_h;

		pos2.x = center.x + half_w;
		pos2.y = center.y;
		pos2.z = center.z - half_h;

		pos3.x = center.x - half_w;
		pos3.y = center.y;
		pos3.z = center.z - half_h;

		pos4.x = center.x - half_w;
		pos4.y = center.y;
		pos4.z = center.z + half_h;

		if (fill) {
			DrawTriangle3D(cast(center), pos1, pos2, color, TRUE);
			DrawTriangle3D(cast(center), pos3, pos4, color, TRUE);
		}
		else {
			DrawLine3D(pos1, pos2, color);
			DrawLine3D(pos2, pos3, color);
			DrawLine3D(pos3, pos4, color);
			DrawLine3D(pos4, pos1, color);
		}
	}

	bool CheckCircleXZHit(float3& center1, float radius1, float3& center2, float radius2)
	{
		float3 pos1 = center1;
		float3 pos2 = center2;

		pos1.y = 0.0f;
		pos2.y = 0.0f;

		float distance = GetFloat3Distance(pos1, pos2);
		float radius = radius1 + radius2;
		if (distance <= radius) {
			return true;
		}
		return false;
	}

	bool CheckBallHit(float3& center1, float radius1, float3& center2, float radius2)
	{
		float distance = GetFloat3Distance(center1, center2);
		float radius = radius1 + radius2;
		if (distance <= radius) {
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------
	//! @brief 3D の AB 同士の当たり判定を行なう
	//! @param box_pos1 ボックス 1 の中心位置
	//! @param box_size1 ボックス 1 の半サイズ
	//! @param box_pos2 ボックス 2 の中心位置
	//! @param box_size2 ボックス 2 の半サイズ
	//! @return 当たっていれば true
	//-----------------------------------------------------------
	bool CheckBoxHit3D(float3& box_pos1, float3& box_size1, float3& box_pos2, float3& box_size2)
	{
		float right1 = box_pos1.x + box_size1.x;
		float left1 = box_pos1.x - box_size1.x;
		float up1 = box_pos1.y + box_size1.y;
		float down1 = box_pos1.y - box_size1.y;
		float front1 = box_pos1.z + box_size1.z;
		float back1 = box_pos1.z - box_size1.z;

		float right2 = box_pos2.x + box_size2.x;
		float left2 = box_pos2.x - box_size2.x;
		float up2 = box_pos2.y + box_size2.y;
		float down2 = box_pos2.y - box_size2.y;
		float front2 = box_pos2.z + box_size2.z;
		float back2 = box_pos2.z - box_size2.z;

		if (right1 >= left2 && left1 <= right2) {
			if (up1 >= down2 && down1 <= up2) {
				if (front1 >= back2 && back1 <= front2) {
					return true;
				}
			}
		}
		return false;
	}

	bool CheckPointBoxHit3D(float3& point, float3& box_pos, float3& box_size)
	{
		float left = box_pos.x - box_size.x;
		float right = box_pos.x + box_size.x;
		float up = box_pos.y - box_size.y;
		float down = box_pos.y + box_size.y;
		float front = box_pos.z - box_size.z;
		float back = box_pos.z + box_size.z;

		if (point.x >= left && point.x <= right &&
			point.y >= up && point.y <= down &&
			point.z >= front && point.z <= back)
		{
			return true;
		}

		return false;
	}

	bool CheckBallBoxHit(float3& ball_pos, float ball_radius, float3& box_pos, float3& box_size)
	{
		float3 near_pos;
		near_pos.x = ball_pos.x;
		if (near_pos.x < box_pos.x - box_size.x) 	near_pos.x = box_pos.x - box_size.x;
		if (near_pos.x > box_pos.x + box_size.x) 	near_pos.x = box_pos.x + box_size.x;

		near_pos.y = ball_pos.y;
		if (near_pos.y < box_pos.y - box_size.y) 	near_pos.y = box_pos.y - box_size.y;
		if (near_pos.y > box_pos.y + box_size.y) 	near_pos.y = box_pos.y + box_size.y;

		near_pos.z = ball_pos.z;
		if (near_pos.z < box_pos.z - box_size.z) 	near_pos.z = box_pos.z - box_size.z;
		if (near_pos.z > box_pos.z + box_size.z) 	near_pos.z = box_pos.z + box_size.z;

		float distance = GetFloat3Distance(ball_pos, near_pos);
		if (distance <= ball_radius) {
			return true;
		}

		return false;
	}

	float3 GetFloat3LinePointNearPosition(float3& line_start, float3& line_goal, float3& point)
	{
		float3 line_dir = line_goal - line_start;
		Normalize(line_dir);

		float3 point_dir = point - line_start;

		float line_dot = GetFloat3Dot(line_dir, point_dir);

		float3 near_dir = line_dir * line_dot;

		float3 near_pos = line_start + near_dir;

		float3 dir1 = line_start - near_pos;

		if (GetFloat3Dot(line_dir, dir1) > 0.0f) {
			near_pos = line_start;
		}

		float3 dir2 = line_goal - near_pos;

		if (GetFloat3Dot(line_dir, dir2) < 0.0f) {
			near_pos = line_goal;
		}

		return near_pos;
	}

	float GetFloat3LinePointNearDistance(float3& line_start, float3& line_goal, float3& point)
	{
		float3 near_pos = GetFloat3LinePointNearPosition(line_start, line_goal, point);

		float near_distance = GetFloat3Distance(point, near_pos);

		return near_distance;
	}

	bool CheckLineBallHit(float3& line_start, float3& line_goal, float3& ball_pos, float ball_radius)
	{
		float near_distance = GetFloat3LinePointNearDistance(line_start, line_goal, ball_pos);

		if (near_distance <= ball_radius) {
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------
	//! @brief ２つの float2 の距離を求める
	//! @param pos1 1つ目の座標
	//! @param pos2 2つ目の座標
	//! @return 2点間の距離
	//! @note              戻り値は float 型
	//-----------------------------------------------------------
	float GetFloat2Distance(float2& pos1, float2& pos2)
	{
		float x = pos1.x - pos2.x;
		float y = pos1.y - pos2.y;
		float distance = sqrtf(x * x + y * y);
		return distance;
	}

	float GetFloat2Dot(float2& v1, float2& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	float GetFloat2Cross(float2& v1, float2& v2)
	{
		return v1.x * v2.y - v1.y * v2.x;
	}

	float GetFloat3Distance(float3& pos1, float3& pos2)
	{
		float x = pos1.x - pos2.x;
		float y = pos1.y - pos2.y;
		float z = pos1.z - pos2.z;
		float distance = sqrtf(x * x + y * y + z * z);
		return distance;
	}

	float GetFloat3Dot(float3& v1, float3& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	float3 GetFloat3Cross(float3& v1, float3& v2)
	{
		float3 v;
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
		return v;
	}

	float3 GetFloat3VTransform(float3& v, MATRIX& mat)
	{
		VECTOR base = cast(v);
		VECTOR change = VTransform(base, mat);

		float3 ret = cast(change);
		return ret;
	}

	float TORADIAN(float degree) { return degree * PI / 180.0f; }

	//---------------------------------------------------------------
	//! ラジアンを度に変換
	//! @param radian ラジアン
	//! @return 度
	//---------------------------------------------------------------
	float TODEGREE(float radian) { return radian * 180.0f / PI; }

	//---------------------------------------------------------------
	//! @brief 正規化
	//! @param vec 正規化するベクトル
	//! @return 正規化後のベクトル
	//---------------------------------------------------------------
	float2 Normalize(const float2& vec)
	{
		float length = sqrtf(vec.x * vec.x + vec.y * vec.y);
		if (length == 0.0f) return float2(0.0f, 0.0f);
		return float2(vec.x / length, vec.y / length);
	}

	//---------------------------------------------------------------
	//! @brief 正規化
	//! @param vec 正規化するベクトル
	//! @return 正規化後のベクトル
	//---------------------------------------------------------------
	float3 Normalize(const float3& vec)
	{
		float length = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
		if (length == 0.0f) return float3(0.0f, 0.0f, 0.0f);
		return float3(vec.x / length, vec.y / length, vec.z / length);
	}

	//---------------------------------------------------------------
	//! 回転に沿った長さ（cos）を取得
	//! @param rot 回転（ラジアン）
	//! @return cos(rot)
	//---------------------------------------------------------------
	float GetLengthAlongRotation(float rot) { return cosf(rot); } // 回転に沿った長さ取得

	//---------------------------------------------------------------
	//! 回転に垂直な長さ（sin）を取得
	//! @param rot 回転（ラジアン）
	//! @return sin(rot)
	//---------------------------------------------------------------
	float GetLengthOppositeRotation(float rot) { return sinf(rot); } // 回転に対する直交成分取得
}
