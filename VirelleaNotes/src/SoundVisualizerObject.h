//----------------------------------------------------------------------------
//! @file   SoundVisualizerObject.h
//! @brief  サウンドビジュアライザーのインターフェース
//----------------------------------------------------------------------------
#pragma once
#include "ObjectBase.h"
#include "UIConstants.h"
#include "Structs.h"
#include <vector>

namespace Object {
//============================================================================
//! @class  SoundVisualizerObject
//! 音声のスペクトラムを描画するためのオブジェクトです。
//============================================================================
class SoundVisualizerObject : public ObjectBase
{
public:
    //! コンストラクタを定義します。
    SoundVisualizerObject() = default;

    //! デストラクタを定義します。
    ~SoundVisualizerObject() = default;

    //! 初期化処理を行います。
    void OnInit() override;

    //! 更新処理を行います。
    void OnUpdate() override;

    //! 描画処理を行います。
    void OnDraw() override;

    //! 終了処理を行います。
    void OnEnd() override;

    //! 解析音源ハンドルを設定します。
    //! @param [in] handle 音声解析に使用するソフトサウンドのハンドル
    void SetSoftHandle(int handle);

    //! 再生位置を設定します。
    //! @param [in] sample_time 再生位置（サンプル単位）
    void SetSampleTime(int sample_time);

    //! ビジュアライザーの描画位置を設定します。
    //! @param [in] x 描画基準X
    //! @param [in] y 描画基準Y
    void SetPosition(float x, float y);

private:
    // サウンドビジュアライザー関係の変数
    FloatPos m_sound_visualizer_pos{0, 0};     //!< ビジュアライザーの描画基準座標
    int      m_sound_handle{0};                //!< サウンドハンドル（再生用、参照）
    int      m_soft_sound_handle{0};           //!< ソフトサウンドハンドル（解析用、参照）
    int      m_sample_pos{0};                  //!< 現在の再生位置（サンプル）
    float    m_param_list[BUFFER_LENGTH]{};    //!< 表示用のパラメータ配列
    bool     m_one_load_flag{false};           //!< 一度だけ読み込んだかどうかのフラグ
};
}    // namespace Object
