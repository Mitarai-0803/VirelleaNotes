#pragma once
#include "ObjectBase.h"
#include "ConstantsRhythm.h"
#include "NotesObject.h"
#include "UIConstants.h"
#include "Structs.h"
#include <vector>
#include <memory>
#include <array>
#include <string>
#include "JudgeEffect.h"

namespace Object {
//-----------------------------------------------------------
//! @class LineObject
//! @brief 線分オブジェクトクラス
//-----------------------------------------------------------
class LineObject : public ObjectBase
{
public:
    //-----------------------------------------------------------
    //! @brief 初期化処理
    //-----------------------------------------------------------
    void OnInit() override;

    //-----------------------------------------------------------
    //! @brief 更新処理
    //-----------------------------------------------------------
    void OnUpdate() override;

    //-----------------------------------------------------------
    //! @brief 描画処理
    //-----------------------------------------------------------
    void OnDraw() override;

    //-----------------------------------------------------------
    //! @brief  終了処理
    //-----------------------------------------------------------
    void OnEnd() override;

    //-----------------------------------------------------------
    //! @brief  ノーツの生成
    //! @param notes_type ノーツの種類（通常、長押し開始、長押し終了など）
    //-----------------------------------------------------------
    void AddNotes(int notes_type);

    //-----------------------------------------------------------
    //! @brief ノーツを流すラインの座標の設定
    //! @param x ラインのX座標
    //! @param y ラインのY座標
    //! @param w ラインの幅(右下ｘ座標)
    //! @param h ラインの高さ(右下y座標)
    //-----------------------------------------------------------
    void SetLineBoxPos(float x, float y, float w, float h);

    //-----------------------------------------------------------
    //! @brief 判定ラインのY座標の設定
    //! @param x ラインのX座標
    //! @param y ラインのY座標
    //! @param w ラインの幅(右下ｘ座標)
    //! @param h ラインの高さ(右下y座標)
    //-----------------------------------------------------------
    void SetDrawLinePos(float x, float y, float w, float h);

    //-----------------------------------------------------------
    //! @brief キーの設定
    //! @param key 入力キー
    //-----------------------------------------------------------
    void SetKey(int key);

    //-----------------------------------------------------------
    //! @brief  キーと表示用ラベルの設定
    //! @param key  入力キー
    //! @param label ライン上に表示する文字列
    //-----------------------------------------------------------
    void SetKey(int key, const std::string& label);

    //-----------------------------------------------------------
    //! @brief  ラベルのみの設定
    //! @param label ライン上に表示する文字列
    //-----------------------------------------------------------
    void SetKeyLabel(const std::string& label);

    //-----------------------------------------------------------
    //! @brief デバッグ描画の有効化/無効化
    //! @param enabled 有効にする場合は true
    //-----------------------------------------------------------
    void SetDebugDraw(bool enabled);

    //-----------------------------------------------------------
    //! @brief  合計スコアの取得
    //! @return 合計スコア
    //-----------------------------------------------------------
    int GetTotalScore() const;

    //-----------------------------------------------------------
    //! @brief  合計スコアのリセット
    //-----------------------------------------------------------
    void ResetTotalScore();

    //-----------------------------------------------------------
    //! @brief  MISS回数の取得
    //! @return MISS回数
    //-----------------------------------------------------------
    int GetMissCount() const;

    //-----------------------------------------------------------
    //! @brief  GOOD回数の取得
    //! @return GOOD回数
    //-----------------------------------------------------------
    int GetGoodCount() const;

    //-----------------------------------------------------------
    //! @brief  PERFECT回数の取得
    //! @return PERFECT回数
    //-----------------------------------------------------------
    int GetPerfectCount() const;

    //-----------------------------------------------------------
    //! @brief  BAD回数の取得
    //! @return BAD回数
    //-----------------------------------------------------------
    int GetBadCount() const;

private:
    //-----------------------------------------------------------
    //! @brief ノーツの判定処理
    //! @param note_pos ノーツの座標
    //! @return 判定結果（PERFECT, GOOD, BAD, MISS, または NONE）
    //-----------------------------------------------------------
    int CheckJudgement(const FloatPos& note_pos);

    //-----------------------------------------------------------
    //! @brief 判定結果に応じた処理の適用
    //! @param result 判定結果
    //-----------------------------------------------------------
    void ApplyJudgement(int result);

    //-----------------------------------------------------------
    // 構造体
    //-----------------------------------------------------------
    struct HoldNotePair    // 長押しノーツの構造体
    {
        std::shared_ptr<NotesObject> start;
        std::shared_ptr<NotesObject> end;
        bool                         isHolding = false;
        bool                         isFailed  = false;
        bool                         isSuccess = false;
        bool                         isDraw    = true;
        // ホールド開始時の固定座標（押した瞬間の位置を保持）
        FloatPos startPos          = {0.0f};
        bool     isStartFixed      = false;
        int      hold_effect_timer = 0;    //!< ホールド中のエフェクト発生タイマ
    };

    //-----------------------------------------------------------
    // メンバー変数
    //-----------------------------------------------------------
    BoxF         m_line_box;                               //!< ノーツを流すラインの四角情報
    BoxF         m_judgement_line[JUDGEMENT_MAX];          //!< 既存互換の判定配列（上側領域を保持）
    BoxF         m_judgement_line_above[JUDGEMENT_MAX];    //!< 判定領域（ライン上側）
    BoxF         m_judgement_line_below[JUDGEMENT_MAX];    //!< 判定領域（ライン下側）
    FloatLinePos m_draw_line;                              //!< 判定ライン(描画用)
    FloatPos     m_draw_judge_str_pos = {0.0f};            //!< 判定文字列の描画座標

    // 軽量な描画用キャッシュ変数（毎フレーム再作成するよりメンバで保持する）
    // 背景色は RGB 成分を保持して逐次補間で復帰する
    float        m_bg_r             = 64.0f;         //!< 背景色 赤成分のキャッシュ
    float        m_bg_g             = 64.0f;         //!< 背景色 緑成分のキャッシュ
    float        m_bg_b             = 64.0f;         //!< 背景色 青成分のキャッシュ
    unsigned int m_hold_rect_color  = COLOR_BLUE;    //!< ホールド矩形色のキャッシュ
    int          m_draw_judge_str_w = 0;             //!< 判定文字列幅のキャッシュ
    int          m_draw_judge_str_h = 0;             //!< 判定文字列高さのキャッシュ

    std::vector<std::shared_ptr<NotesObject>> m_notes;         //!< 流すノーツ
    std::vector<HoldNotePair>                 m_hold_notes;    //!< 長押しノーツ

    // 判定エフェクトの管理
    std::vector<JudgeEffect> m_judge_effects;    //!< 判定時に表示するエフェクトの配列

    int         m_judgement_mode = NORMAL;                               //!< 判定モード
    int         m_miss_count     = 0;                                    //!< ミスカウント
    int         m_good_count     = 0;                                    //!< グッドカウント
    int         m_perfect_count  = 0;                                    //!< パーフェクトカウント
    int         m_bad_count      = 0;                                    //!< バッドカウント
    int         m_sync_count     = 0;                                    //!< シンクカウント
    int         m_key            = 0;                                    //!< 判定キー
    std::string m_key_label;                                             //!< ライン上に表示する入力キーのラベル
    int         m_frame_count                         = 0;               //!< フレーム数のカウント
    int         m_total_score                         = 0;               //!< 合計スコア（外部参照用）
    int         m_notes_image_handle[NOTES_IMAGE_MAX] = {-1, -1, -1};    //!< ノーツ画像（未ロードは -1）
    bool        m_debug_draw_enabled                  = false;           //!< デバッグ描画フラグ
};
}    // namespace Object
