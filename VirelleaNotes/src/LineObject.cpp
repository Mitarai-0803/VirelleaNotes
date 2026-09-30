//----------------------------------------------------------------------------
//! @file   LineObject.cpp
//! @brief  ラインと判定ロジックの実装
//! @detail 判定ラインの配置、ノーツの更新・判定、ホールドノーツの管理、
//!         および描画を行う実装を提供します。判定結果の表示やスコア管理も担当します。
//----------------------------------------------------------------------------
#include "LineObject.h"
#include "InputManager.h"
#include "GameScene.h"
#include "Functions.h"
#include "JudgeEffect.h"
#include "Structs.h"
#include "Font.h"

//============================================================================
//! @class  LineObject
//! @detail ライン (判定ライン) に関する描画とノーツの更新・判定処理を担う実装。
//!         判定領域の計算、ホールドノーツのペア管理、判定スコアの集計などを行います。
//============================================================================
namespace Object {
//-----------------------------------------------------------
//! 初期化処理
//----------------------------------------------------------------------------
void LineObject::OnInit()
{
    // ライン描画用の矩形を初期化
    m_line_box = {0.0f, 0.0f, 0.0f, 0.0f};
    // 判定ラインの始点/終点を初期化
    m_draw_line = {0.0f, 0.0f, 0.0f, 0.0f};

    // 各判定領域を初期化（上側/下側それぞれ）
    for(int i = 0; i < JUDGEMENT_MAX; ++i) {
        m_judgement_line_above[i] = {0.0f, 0.0f, 0.0f, 0.0f};
        m_judgement_line_below[i] = {0.0f, 0.0f, 0.0f, 0.0f};
    }

    // ラインに対応する入力ラベルを描画
    if(!m_key_label.empty()) {
        int   font = Font::GetFont(Font::FONT_SIZE_20_INDEX);
        int   w    = GetDrawStringWidthToHandle(m_key_label.c_str(), -1, font);
        int   h    = GetFontSizeToHandle(font);
        float x    = m_line_box.x + (m_line_box.w * 0.5f) - (w * 0.5f);
        float y    = m_draw_line.pos_1.y - h - HOWTO_LINE_GAP_SMALL;    // ライン上に小さく表示
        DrawStringToHandle(static_cast<int>(x), static_cast<int>(y), m_key_label.c_str(), COLOR_WHITE, font);
    }

    // 判定状態・表示フレーム数を初期化
    m_judgement_mode = NORMAL;
    m_frame_count    = 0;

    // ノーツ表示用画像を読み込み
    m_notes_image_handle[0] = LoadGraph("data/Image/notes.png");
    m_notes_image_handle[1] = LoadGraph("data/Image/notes1.png");
    m_notes_image_handle[2] = LoadGraph("data/Image/notes2.png");

    // ノーツ配列を空にする
    m_notes.clear();

    // 背景色の初期値を既定色に設定
    m_bg_r = DEFAULT_BG_R;
    m_bg_g = DEFAULT_BG_G;
    m_bg_b = DEFAULT_BG_B;

    // デバッグ描画を有効化
    m_debug_draw_enabled = false;

    // ラインの入力ラベル初期化
    m_key_label.clear();
}

//-----------------------------------------------------------
//! 更新処理
//----------------------------------------------------------------------------
void LineObject::OnUpdate()
{
    // シーンがポーズ中なら更新をスキップ
    if(Scene::GameScene::IsPaused())
        return;
    // 判定エフェクト更新
    for(auto& ef : m_judge_effects) {
        ef.OnUpdate();
    }

    // 終了したエフェクトを削除して領域を再利用可能にする
    m_judge_effects.erase(std::remove_if(m_judge_effects.begin(), m_judge_effects.end(), [](const JudgeEffect& e) { return !e.IsAlive(); }),
                          m_judge_effects.end());

    //--------------------------------
    // 判定領域の配置
    //--------------------------------
    // m_draw_line の始点・終点から各判定領域（PERFECT/GOOD/BAD/MISS）の矩形を作成
    const float judgeWidth = m_draw_line.pos_2.x - m_draw_line.pos_1.x;
    for(int i = 0; i < JUDGEMENT_MAX; ++i) {
        // 上側領域（判定ラインを基準に上方向に積む）
        m_judgement_line_above[i].x = m_draw_line.pos_1.x;
        m_judgement_line_above[i].y = m_draw_line.pos_1.y - (JUDGEMENT_H * i);
        m_judgement_line_above[i].w = judgeWidth;
        m_judgement_line_above[i].h = JUDGEMENT_H;

        // 既存コードとの互換性のため、m_judgement_line に上側データをコピー
        m_judgement_line[i] = m_judgement_line_above[i];

        // 下側領域（判定ラインを基準に下方向に積む）
        // 下側は PERFECT が m_draw_line の位置、その後下へ積む
        // 配列インデックスは enum JUDGEMENT に準拠しているため、描画上のオフセットをマッピングする
        static const int belowOffset[JUDGEMENT_MAX] = {3, 0, 1, 2};    // MISS, PERFECT, GOOD, BAD
        m_judgement_line_below[i].x                 = m_draw_line.pos_1.x;
        m_judgement_line_below[i].y                 = m_draw_line.pos_1.y + (JUDGEMENT_H * belowOffset[i]);
        m_judgement_line_below[i].w                 = judgeWidth;
        m_judgement_line_below[i].h                 = JUDGEMENT_H;
    }

    //--------------------------------
    // MISS 判定領域をライン下端まで拡張
    //--------------------------------
    // ライン背景の下端まで MISS 判定領域を伸ばして、到達後の削除判定を行いやすくする
    const float lineBottom         = m_line_box.y + m_line_box.h;
    const float missTop            = m_judgement_line_below[MISS].y;
    float       missHeight         = lineBottom - missTop;
    m_judgement_line_below[MISS].h = (missHeight > 0.0f) ? missHeight : 0.0f;

    //--------------------------------
    // ノーツ更新と判定処理
    //--------------------------------
    const bool isCountingDown = Scene::GameScene::IsCountingDown();
    // m_notes に格納された全ノーツを更新して、キーヒット時に判定を行う
    if(!isCountingDown) {
        for(auto& note : m_notes) {
            if(!note)
                continue;

            // ノーツ個別の更新（移動など）を行う
            note->OnUpdate();
            const auto noteType = note->GetNotesType();
            const auto pos      = note->GetPos();

            // 通常・同時押しノーツの即時判定処理
            if((noteType == NOTE_TYPE::NOTE_TYPE_TAP && InputManager::PushHitKey(m_key)) ||
               (noteType == NOTE_TYPE::NOTE_TYPE_SYNC && InputManager::PushHitKey(m_key))) {
                int result = CheckJudgement(pos);
                if(result != NORMAL) {
                    // 判定適用とノーツ削除
                    ApplyJudgement(result);
                    note.reset();
                }
            }

            //--------------------------------
            // ホールドノーツのペア処理
            //--------------------------------
            for(auto& pair : m_hold_notes) {
                // 開始ノーツが存在しないペアは無視
                if(!pair.start)
                    continue;

                const auto& sp = pair.start->GetPos();

                // 開始ノーツが判定ラインを越えてしまった場合は開始が押されていない扱いで失敗にする
                if(!pair.isHolding && !pair.isFailed && !pair.isStartFixed) {
                    const float startCenterY = sp.y + (NOTES_H * HALF);
                    if(startCenterY > m_draw_line.pos_1.y) {
                        // 開始ノーツ未押下でライン通過 -> 失敗表示にして色を灰色にする
                        pair.isFailed = true;
                        pair.isDraw   = true;
                        // 描画が伸び続けるのを防ぐため開始位置を固定する
                        pair.startPos     = sp;
                        pair.isStartFixed = true;
                        // 判定表示を出す
                        ApplyJudgement(MISS);
                    }
                }

                // ホールド開始判定: 開始ノーツが PERFECT 判定領域に被っていてキーが押されたら開始
                if(!pair.isHolding && InputManager::PushHitKey(m_key) &&
                   MyLibrary::CheckBoxHit(m_judgement_line_above[PERFECT].x,
                                          m_judgement_line_above[PERFECT].y,
                                          m_judgement_line_above[PERFECT].w,
                                          m_judgement_line_above[PERFECT].h,
                                          sp.x,
                                          sp.y,
                                          NOTES_W,
                                          NOTES_H)) {
                    pair.isHolding = true;
                    pair.isFailed  = false;
                    pair.isDraw    = true;    // 四角を描画する
                    // 判定を適用して表示フレームをリセット
                    ApplyJudgement(PERFECT);

                    // m_notes 側から開始ノーツを削除（ペアは start を保持して描画に利用）
                    bool removed = false;
                    // まずはポインタ一致で削除
                    for(auto& n : m_notes) {
                        if(n && pair.start && n.get() == pair.start.get()) {
                            n.reset();
                            removed = true;
                            break;
                        }
                    }

                    // フォールバック: 近傍の HOLD_START を削除（何らかの理由でポインタ不整合が起きた場合の保険）
                    if(!removed) {
                        for(auto& n : m_notes) {
                            if(!n)
                                continue;
                            if(n->GetNotesType() == NOTE_TYPE::NOTE_TYPE_HOLD_START) {
                                auto np = n->GetPos();
                                // 位置が近ければ削除
                                if(std::abs(np.x - sp.x) < HOLD_REMOVE_NEAR_THRESHOLD && std::abs(np.y - sp.y) < HOLD_REMOVE_NEAR_THRESHOLD) {
                                    n.reset();
                                    break;
                                }
                            }
                        }
                    }

                    // 押した瞬間の座標を固定して四角の伸びを止める
                    pair.startPos     = sp;
                    pair.isStartFixed = true;
                    // m_notes から削除したが、ペアの開始情報は保持しておく（描画・判定用）
                }

                // ホールド終了判定: 終了ノーツが存在し、ホールド中でかつ失敗していない場合に判定
                if(pair.end && pair.isHolding && !pair.isFailed) {
                    const auto& ep = pair.end->GetPos();
                    if(InputManager::PushHitKey(m_key) && MyLibrary::CheckBoxHit(m_judgement_line_above[PERFECT].x,
                                                                                 m_judgement_line_above[PERFECT].y,
                                                                                 m_judgement_line_above[PERFECT].w,
                                                                                 m_judgement_line_above[PERFECT].h,
                                                                                 ep.x,
                                                                                 ep.y,
                                                                                 NOTES_W,
                                                                                 NOTES_H)) {
                        // 正常終了処理: 成功フラグを立てて描画を停止
                        pair.isHolding = false;
                        pair.isSuccess = true;
                        // 正常終了では四角は消える
                        pair.isDraw   = false;
                        pair.isFailed = false;
                        // 成功扱いだがペア情報はクリアせず、後続の処理で適切に削除する
                        // 判定を適用して表示フレームをリセット
                        ApplyJudgement(PERFECT);

                        // m_notes 側から終了ノーツを削除
                        for(auto& n : m_notes) {
                            if(n == pair.end) {
                                n.reset();
                                break;
                            }
                        }

                        // 終了ノーツは到着時に自動で削除
                        pair.end.reset();
                        // 開始ノーツ情報は保持しておく（描画用に startPos を使う）
                        // pair.start.reset();  // コメントアウトして保持
                    }
                }

                // ホールド中にキーが離れた場合は失敗扱いとする
                if(pair.isHolding && !CheckHitKey(m_key)) {
                    // 押している途中で離した -> 失敗扱い
                    pair.isHolding = false;
                    pair.isFailed  = true;
                    // 失敗時は矩形を描画して色を灰色にする
                    pair.isDraw = true;
                    // 判定を適用して表示フレームをリセット
                    ApplyJudgement(MISS);

                    // 終了ノーツは到着時に矩形を消すために残す
                    pair.hold_effect_timer = 0;
                }

                // ホールド中は定期的にエフェクトを発生させる
                if(pair.isHolding) {
                    ++pair.hold_effect_timer;
                    if(pair.hold_effect_timer >= HOLD_EFFECT_INTERVAL) {
                        pair.hold_effect_timer = 0;
                        // エフェクト上限をチェックして追加
                        if(static_cast<int>(m_judge_effects.size()) < JUDGE_EFFECTS_DRAW_LIMIT) {
                            JudgeEffect ef;
                            ef.OnInit();
                            // 発生位置: 押した瞬間の固定座標を優先
                            float ex = (pair.isStartFixed) ? pair.startPos.x : pair.start->GetPos().x;
                            float ey = (pair.isStartFixed) ? pair.startPos.y : pair.start->GetPos().y;
                            // ノーツの左上座標ではなく中心で出すためにノーツ幅/高さの半分を加算
                            float ex_center = ex + (NOTES_W * HALF);
                            float ey_center = ey + (NOTES_H * HALF);
                            // 色と短めの継続時間、小さめのパーティクル数を指定
                            ef.Start(ex_center, ey_center, COLOR_YELLOW, JUDGEMENT_DRAW_TIME_MAX / 2, 6);
                            m_judge_effects.push_back(std::move(ef));
                            // ホールド継続中のスコア加点
                            m_total_score += HOLD_SCORE_PER_TICK;
                        }
                    }
                }

                // 終了ノーツ到着後に失敗している場合: MISS 領域に到達したらペアをクリア
                if(pair.end && pair.isFailed) {
                    const auto& ep = pair.end->GetPos();
                    if(MyLibrary::CheckBoxHit(m_judgement_line_below[MISS].x,
                                              m_judgement_line_below[MISS].y,
                                              m_judgement_line_below[MISS].w,
                                              m_judgement_line_below[MISS].h,
                                              ep.x,
                                              ep.y,
                                              NOTES_W,
                                              NOTES_H)) {
                        // m_notes から終了ノーツを削除
                        for(auto& n : m_notes) {
                            if(n && pair.end && n.get() == pair.end.get()) {
                                n.reset();
                                break;
                            }
                        }

                        // ペアをクリアして描画を止める
                        pair.end.reset();
                        pair.start.reset();
                        pair.isDraw    = false;
                        pair.isFailed  = false;
                        pair.isHolding = false;
                    }
                }

                // 終了ノーツが PERFECT 判定領域に被ったら開始・終了ノーツとも削除してペアをクリア
                if(pair.end && pair.isDraw) {
                    const auto& ep = pair.end->GetPos();
                    if(MyLibrary::CheckBoxHit(m_judgement_line_below[PERFECT].x,
                                              m_judgement_line_below[PERFECT].y,
                                              m_judgement_line_below[PERFECT].w,
                                              m_judgement_line_below[PERFECT].h,
                                              ep.x,
                                              ep.y,
                                              NOTES_W,
                                              NOTES_H)) {
                        // m_notes から終了ノーツを削除（ポインタ一致で優先）
                        for(auto& n : m_notes) {
                            if(n && pair.end && n.get() == pair.end.get()) {
                                n.reset();
                            }
                        }

                        // 開始ノーツはペア内に情報が残っていればそれを優先して削除
                        if(pair.start) {
                            for(auto& n : m_notes) {
                                if(n && n.get() == pair.start.get()) {
                                    n.reset();
                                }
                            }
                        }
                        else if(pair.isStartFixed) {
                            // フォールバック: 固定された開始座標付近の HOLD_START を削除
                            for(auto& n : m_notes) {
                                if(!n)
                                    continue;
                                if(n->GetNotesType() == NOTE_TYPE::NOTE_TYPE_HOLD_START) {
                                    auto np = n->GetPos();
                                    // 位置が近ければ削除
                                    if(std::abs(np.x - pair.startPos.x) < 8.0f && std::abs(np.y - pair.startPos.y) < 8.0f) {
                                        n.reset();
                                        break;
                                    }
                                }
                            }
                        }

                        // ペアをクリアして矩形描画を停止
                        pair.end.reset();
                        pair.start.reset();
                        pair.isDraw       = false;
                        pair.isFailed     = false;
                        pair.isHolding    = false;
                        pair.isStartFixed = false;
                    }
                }
            }
        }
    }

    //-----------------------------------------------------------
    // ライン背景の四角の描画
    //-----------------------------------------------------------
    // 背景色のスムーズな遷移
    // キー押下時はハイライト色に即時設定、離したら徐々に既定の色へ戻す
    if(CheckHitKey(m_key)) {
        // ハイライト色に即時セット
        m_bg_r = HIGHLIGHT_BG_R;
        m_bg_g = HIGHLIGHT_BG_G;
        m_bg_b = HIGHLIGHT_BG_B;
    }
    else {
        // 毎フレーム既定色へ線形補間で戻す
        m_bg_r += (DEFAULT_BG_R - m_bg_r) * BG_LERP_SPEED;
        m_bg_g += (DEFAULT_BG_G - m_bg_g) * BG_LERP_SPEED;
        m_bg_b += (DEFAULT_BG_B - m_bg_b) * BG_LERP_SPEED;
    }

    // 終了した/無効なホールドペアを削除して矩形が残る不具合を回避
    m_hold_notes.erase(std::remove_if(m_hold_notes.begin(),
                                      m_hold_notes.end(),
                                      [](const HoldNotePair& p) {
                                          // start と end が両方 null のペアは不要
                                          return (!p.start && !p.end);
                                      }),
                       m_hold_notes.end());
}

//-----------------------------------------------------------
//! 描画処理
//----------------------------------------------------------------------------
void LineObject::OnDraw()
{
    // DxLib 用のカラー値に変換して描画
    const unsigned int bg_color_draw = GetColor(static_cast<int>(m_bg_r), static_cast<int>(m_bg_g), static_cast<int>(m_bg_b));
    DrawFillBox(m_line_box.x, m_line_box.y, m_line_box.x + m_line_box.w, m_line_box.y + m_line_box.h, bg_color_draw);

    //--------------------------------------------------------
    // デバッグ描画: 判定領域の矩形とノーツのバウンディングおよび判定状態
    //--------------------------------------------------------
    if(m_debug_draw_enabled) {
        // 判定領域を色付きで描画（上側と下側を両方表示）
        // NOTE: 配列インデックス順は enum JUDGEMENT に合わせる (0:MISS,1:PERFECT,2:GOOD,3:BAD)
        unsigned int dbg_colors_above[JUDGEMENT_MAX] = {COLOR_PINK, COLOR_PINK, COLOR_YELLOW_GREEN, COLOR_GRAY};
        // 上側と同じ判定ごとの色を下側でも使用する
        unsigned int dbg_colors_below[JUDGEMENT_MAX] = {COLOR_RED, COLOR_PINK, COLOR_YELLOW_GREEN, COLOR_GRAY};

        // 上側判定領域を描画
        for(int i = 0; i < JUDGEMENT_MAX; ++i) {
            const float  x         = m_judgement_line_above[i].x;
            const float  y         = m_judgement_line_above[i].y;
            const float  w         = m_judgement_line_above[i].w;
            const float  h         = m_judgement_line_above[i].h;
            unsigned int fillColor = dbg_colors_above[i];
            DrawFillBox(static_cast<int>(x), static_cast<int>(y), static_cast<int>(x + w), static_cast<int>(y + h), fillColor);
        }

        // 下側判定領域を描画
        for(int i = 0; i < JUDGEMENT_MAX; ++i) {
            const float x = m_judgement_line_below[i].x;
            const float y = m_judgement_line_below[i].y;
            const float w = m_judgement_line_below[i].w;
            const float h = m_judgement_line_below[i].h;
            DrawFillBox(static_cast<int>(x), static_cast<int>(y), static_cast<int>(x + w), static_cast<int>(y + h), dbg_colors_below[i]);
        }

        // 判定ライン位置を描画
        DrawLineAA(static_cast<int>(m_draw_line.pos_1.x),
                   static_cast<int>(m_draw_line.pos_1.y),
                   static_cast<int>(m_draw_line.pos_2.x),
                   static_cast<int>(m_draw_line.pos_2.y),
                   COLOR_WHITE,
                   DEBUG_LINE_THICKNESS);

        // 各ノーツのバウンディングと判定候補を描画
        for(const auto& note : m_notes) {
            if(!note)
                continue;
            auto pos = note->GetPos();
            int  lx  = static_cast<int>(pos.x);
            int  ly  = static_cast<int>(pos.y);
            int  rx  = static_cast<int>(pos.x + NOTES_W);
            int  ry  = static_cast<int>(pos.y + NOTES_H);
            DrawLineBox(lx, ly, rx, ry, COLOR_CYAN);

            // 判定結果候補を評価して文字列表示
            int res = CheckJudgement(pos);
            // デバッグ表示は実際の表示ルールに合わせる（上側の配列MISSはPERFECT扱い）
            const float judgeLineY  = m_draw_line.pos_1.y;
            const float noteCenterY = pos.y + (NOTES_H * HALF);
            if(noteCenterY < judgeLineY && res == MISS) {
                res = PERFECT;
            }
            const char* label = "NONE";
            switch(res) {
            case MISS:
                label = "MISS";
                break;
            case PERFECT:
                label = "PERFECT";
                break;
            case GOOD:
                label = "GOOD";
                break;
            case BAD:
                label = "BAD";
                break;
            default:
                break;
            }

            DrawString(lx, ly - NOTE_DEBUG_LABEL_Y_OFFSET, label, COLOR_WHITE);
        }
    }

    //-----------------------------------------------------------
    //	縁のライン
    //-----------------------------------------------------------
    DrawLineBox(m_line_box.x, m_line_box.y, m_line_box.x + m_line_box.w, m_line_box.y + m_line_box.h, COLOR_WHITE);

    //-----------------------------------------------------------
    //	判定ライン
    //-----------------------------------------------------------
    DrawLineAA(m_draw_line.pos_1.x, m_draw_line.pos_1.y, m_draw_line.pos_2.x, m_draw_line.pos_2.y, COLOR_WHITE, JUDGEMENT_W);

    //-----------------------------------------------------------
    //	ラインの設定された文字を描画
    //-----------------------------------------------------------
    DrawStringToHandle(static_cast<int>(m_line_box.x + (m_line_box.w * HALF) - KEY_LABEL_X_ADJUST),
                       static_cast<int>(m_draw_line.pos_1.y + KEY_LABEL_Y_OFFSET),
                       m_key_label.c_str(),
                       COLOR_WHITE,
                       Font::GetFont(Font::FONT_SIZE_20_INDEX));

    //-----------------------------------------------------------
    // 判定結果の表示
    //-----------------------------------------------------------
    // 判定が出ている場合のみ、一定フレーム数だけ文字列を描画する
    if(m_judgement_mode != NORMAL && m_judgement_mode < JUDGEMENT_MAX) {
        // フレーム数が最大値に達していない場合は表示を続ける
        if(m_frame_count < JUDGEMENT_DRAW_TIME_MAX) {
            // インデックス順 (0: MISS, 1: PERFECT, 2: GOOD, 3: BAD)
            const char*  JUDGE_STRING[JUDGEMENT_MAX] = {"MISS", "PERFECT", "GOOD", "BAD"};
            unsigned int JUDGE_COLOR[JUDGEMENT_MAX]  = {COLOR_RED, COLOR_PINK, COLOR_YELLOW_GREEN, COLOR_GRAY};

            // 判定文字列の幅・高さをキャッシュに保存
            m_draw_judge_str_w = GetDrawStringWidthToHandle(JUDGE_STRING[m_judgement_mode], -1, Font::GetFont(Font::FONT_SIZE_20_INDEX));
            m_draw_judge_str_h = GetFontSizeToHandle(Font::GetFont(Font::FONT_SIZE_20_INDEX));

            // ライン中央上部に判定文字列を描画（キャッシュ値を使用）
            DrawStringToHandle(m_line_box.x + (m_line_box.w * HALF) - (m_draw_judge_str_w * HALF),
                               m_draw_line.pos_1.y + m_draw_judge_str_h,
                               JUDGE_STRING[m_judgement_mode],
                               JUDGE_COLOR[m_judgement_mode],
                               Font::GetFont(Font::FONT_SIZE_20_INDEX));

            ++m_frame_count;
        }
        else {
            // 所定フレーム表示後は通常状態へ戻す
            m_judgement_mode = NORMAL;
        }
    }

    // ホールドノーツ用の矩形描画
    for(const auto& pair : m_hold_notes) {
        if(!pair.isDraw)
            continue;
        if(!pair.start && !pair.isStartFixed)
            continue;

        // ホールド矩形の色をメンバキャッシュに設定して再利用
        m_hold_rect_color = COLOR_BLUE;
        if(pair.isFailed)
            m_hold_rect_color = COLOR_DARKGRAY;
        else if(!pair.isHolding)
            m_hold_rect_color = COLOR_YELLOW;

        FloatPos spPos = {0.0f};
        if(pair.isStartFixed) {
            spPos = pair.startPos;
        }
        else {
            spPos = pair.start->GetPos();
        }

        if(pair.end) {
            const auto  ep    = pair.end->GetPos();
            const float left  = ((spPos.x < ep.x) ? spPos.x : ep.x) + HOLD_NOTES_W;
            const float right = ((spPos.x < ep.x) ? ep.x : spPos.x) + NOTES_W - HOLD_NOTES_W;
            // 上端は通常 start と end の小さい方だが、失敗時は終了ノーツに合わせる
            float top = 0.0f;
            if(pair.isFailed) {
                top = ep.y;    // 失敗時は終了ノーツの上端に合わせる
            }
            else {
                top = (spPos.y < ep.y) ? spPos.y : ep.y;
            }
            // 終了ノーツのYはMISS下端を超えないようにクランプ
            const float missBottom = m_judgement_line[MISS].y + m_judgement_line[MISS].h;
            float       ep_y       = ep.y;
            if(ep_y + NOTES_H > missBottom) {
                ep_y = missBottom - NOTES_H;
            }
            // ホールド中は判定ラインのYに合わせて下端を固定
            const float judgeY = m_draw_line.pos_1.y;
            float       bottom = 0.0f;
            if(pair.isHolding) {
                bottom = judgeY;
            }
            else {
                // 下端は start と end の大きい方 + ノーツ高さ、かつ missBottom を超えない
                bottom = ((spPos.y > ep_y) ? spPos.y : ep_y);
                if(bottom > missBottom)
                    bottom = missBottom;
            }

            const float draw_left   = (left < right) ? left : right;
            const float draw_right  = (left < right) ? right : left;
            const float draw_top    = (top < bottom) ? top : bottom;
            const float draw_bottom = (top < bottom) ? bottom : top;

            // ホールド用の長方形を描画
            DrawFillBox(
                static_cast<int>(draw_left), static_cast<int>(draw_top), static_cast<int>(draw_right), static_cast<int>(draw_bottom), m_hold_rect_color);
        }
        else {
            const float center_x = m_line_box.x + (NOTES_W * HALF);
            const float left     = center_x + HOLD_NOTES_W;
            const float right    = spPos.x + NOTES_W - HOLD_NOTES_W;
            const float top      = m_line_box.y;
            float       bottom   = 0.0f;
            if(pair.isHolding) {
                // ホールド中は判定ラインに合わせる
                bottom = m_draw_line.pos_1.y;
            }
            else {
                bottom = spPos.y;
            }

            const float draw_l = (left < right) ? left : right;
            const float draw_r = (left < right) ? right : left;
            const float draw_t = (top < bottom) ? top : bottom;
            const float draw_b = (top < bottom) ? bottom : top;

            // 終了ノーツがまだない場合の伸びた矩形を描画
            DrawFillBox(static_cast<int>(draw_l), static_cast<int>(draw_t), static_cast<int>(draw_r), static_cast<int>(draw_b), m_hold_rect_color);
        }
    }

    //-----------------------------------------------------------
    // ノーツ描画
    //-----------------------------------------------------------
    for(auto& note : m_notes) {
        if(note)
            note->OnDraw();
    }

    // 判定エフェクト描画
    for(const auto& ef : m_judge_effects) {
        ef.OnDraw();
    }
}

//-----------------------------------------------------------
//! @brief  終了処理
//-----------------------------------------------------------
void LineObject::OnEnd()
{
    // ノーツ用画像ハンドルの削除
    for(int i = 0; i < NOTES_IMAGE_MAX; ++i) {
        if(m_notes_image_handle[i] != -1)    // 番兵値を -1 に統一（0 は有効なハンドル値になり得るため）
        {
            DeleteGraph(m_notes_image_handle[i]);
            m_notes_image_handle[i] = -1;
        }
    }

    // ノーツ配列・エフェクトをクリア
    m_notes.clear();
    m_judge_effects.clear();
}

//-----------------------------------------------------------
// ノーツの生成
//-----------------------------------------------------------
// 指定されたタイプのノーツを生成してライン上に配置し、
// ホールドノーツであればペア管理用の配列に登録する
//-----------------------------------------------------------
void LineObject::AddNotes(int notes_type)
{
    // 新規ノーツ生成と初期化
    auto notes = std::make_shared<NotesObject>();
    notes->OnInit();

    // ライン上部の中央にノーツをセット
    float centerX = m_line_box.x + (NOTES_W * HALF);
    float centerY = m_line_box.y;
    notes->SetPos(centerX, centerY);
    notes->SetNotesType(notes_type);

    // ノーツの種類に応じて表示画像を切り替え
    switch(notes_type) {
    case NOTE_TYPE_TAP:    // タップノーツ
        notes->SetNotesImage(m_notes_image_handle[2]);
        break;

    case NOTE_TYPE_HOLD_START:    // ホールド開始
        notes->SetNotesImage(m_notes_image_handle[0]);
        break;

    case NOTE_TYPE_HOLD_END:    // ホールド終了
        notes->SetNotesImage(m_notes_image_handle[0]);
        break;

    case NOTE_TYPE_SYNC:    // 同時押しノーツ
        notes->SetNotesImage(m_notes_image_handle[1]);
        break;
    }

    // ノーツ配列へ追加
    m_notes.push_back(notes);

    // ホールドノーツのペア管理
    if(notes_type == NOTE_TYPE_HOLD_START) {
        // 開始ノーツとして新しいペアを作成
        HoldNotePair pair;
        pair.start = notes;
        m_hold_notes.push_back(pair);
    }
    else if(notes_type == NOTE_TYPE_HOLD_END) {
        // 終了ノーツは直近の未完了ペア（最新の開始に対応）に割り当て
        // 逆順で検索して最新の開始ノーツとペアにする
        for(auto it = m_hold_notes.rbegin(); it != m_hold_notes.rend(); ++it) {
            // 終了ノーツは未割当ての最新ペアに割り当てる
            // start が MISS になっていても end を繋げて矩形を閉じたいので isFailed 条件は外す
            if(!it->end) {
                it->end = notes;
                break;
            }
        }
    }
}

//-----------------------------------------------------------
//! @brief ノーツを流すラインの座標の設定
//-----------------------------------------------------------
void LineObject::SetLineBoxPos(float x, float y, float w, float h)
{
    m_line_box = {x, y, w, h};
}

//-----------------------------------------------------------
//! @brief 判定ラインの座標の設定
//-----------------------------------------------------------
void LineObject::SetDrawLinePos(float x, float y, float w, float h)
{
    m_draw_line.pos_1 = {x, y};
    m_draw_line.pos_2 = {w, h};
}

//-----------------------------------------------------------
//! @brief キーの設定
//! @param key 入力キー
//-----------------------------------------------------------
void LineObject::SetKey(int key)
{
    m_key = key;
}

//-----------------------------------------------------------
//! @brief  キーと表示用ラベルの設定
//! @param key  入力キー
//! @param label ライン上に表示する文字列
//-----------------------------------------------------------
void LineObject::SetKey(int key, const std::string& label)
{
    m_key       = key;
    m_key_label = label;
}

//-----------------------------------------------------------
//! @brief  ラベルのみの設定
//! @param label ライン上に表示する文字列
//-----------------------------------------------------------
void LineObject::SetKeyLabel(const std::string& label)
{
    m_key_label = label;
}

//-----------------------------------------------------------
// デバッグ描画の有効化
//-----------------------------------------------------------
void LineObject::SetDebugDraw(bool enabled)
{
    m_debug_draw_enabled = enabled;
}

//-----------------------------------------------------------
//! @brief  合計スコアを取得
//! @return 合計スコア
//-----------------------------------------------------------
int LineObject::GetTotalScore() const
{
    return m_total_score;
}

//-----------------------------------------------------------
//! @brief ノーツの判定処理
//! @param note_pos ノーツの座標
//! @return 判定結果（PERFECT, GOOD, BAD, MISS, または NORMAL）
//-----------------------------------------------------------
int LineObject::CheckJudgement(const FloatPos& note_pos)
{
    // 判定ラインの上下で判定順序を切り替える
    // 上側から到着したノーツは上から順に BAD -> GOOD -> PERFECT
    // 下側から到達したノーツは下から順に PERFECT -> GOOD -> BAD -> MISS
    const float judgeLineY = m_draw_line.pos_1.y;
    // ノーツ中心の Y を基準に上下を判定（ノーツの左上ではなく中心で判断）
    const float noteCenterY = note_pos.y + (NOTES_H * HALF);
    if(noteCenterY < judgeLineY) {
        // 上側から到着: 上側領域を上から順に評価 (BAD -> GOOD -> PERFECT)
        if(MyLibrary::CheckBoxHit(m_judgement_line_above[BAD].x,
                                  m_judgement_line_above[BAD].y,
                                  m_judgement_line_above[BAD].w,
                                  m_judgement_line_above[BAD].h,
                                  note_pos.x,
                                  note_pos.y,
                                  NOTES_W,
                                  NOTES_H))
            return BAD;

        if(MyLibrary::CheckBoxHit(m_judgement_line_above[GOOD].x,
                                  m_judgement_line_above[GOOD].y,
                                  m_judgement_line_above[GOOD].w,
                                  m_judgement_line_above[GOOD].h,
                                  note_pos.x,
                                  note_pos.y,
                                  NOTES_W,
                                  NOTES_H))
            return GOOD;

        if(MyLibrary::CheckBoxHit(m_judgement_line_above[PERFECT].x,
                                  m_judgement_line_above[PERFECT].y,
                                  m_judgement_line_above[PERFECT].w,
                                  m_judgement_line_above[PERFECT].h,
                                  note_pos.x,
                                  note_pos.y,
                                  NOTES_W,
                                  NOTES_H))
            return PERFECT;

        // 判定ライン直上 (配列上の MISS 領域) を PERFE﻿CT 扱いにする
        if(MyLibrary::CheckBoxHit(m_judgement_line_above[MISS].x,
                                  m_judgement_line_above[MISS].y,
                                  m_judgement_line_above[MISS].w,
                                  m_judgement_line_above[MISS].h,
                                  note_pos.x,
                                  note_pos.y,
                                  NOTES_W,
                                  NOTES_H))
            return PERFECT;
    }
    else {
        // 下側から到着: 下側領域を順に評価 (PERFECT -> GOOD -> BAD -> MISS)
        if(MyLibrary::CheckBoxHit(m_judgement_line_below[PERFECT].x,
                                  m_judgement_line_below[PERFECT].y,
                                  m_judgement_line_below[PERFECT].w,
                                  m_judgement_line_below[PERFECT].h,
                                  note_pos.x,
                                  note_pos.y,
                                  NOTES_W,
                                  NOTES_H))
            return PERFECT;

        if(MyLibrary::CheckBoxHit(m_judgement_line_below[GOOD].x,
                                  m_judgement_line_below[GOOD].y,
                                  m_judgement_line_below[GOOD].w,
                                  m_judgement_line_below[GOOD].h,
                                  note_pos.x,
                                  note_pos.y,
                                  NOTES_W,
                                  NOTES_H))
            return GOOD;

        if(MyLibrary::CheckBoxHit(m_judgement_line_below[BAD].x,
                                  m_judgement_line_below[BAD].y,
                                  m_judgement_line_below[BAD].w,
                                  m_judgement_line_below[BAD].h,
                                  note_pos.x,
                                  note_pos.y,
                                  NOTES_W,
                                  NOTES_H))
            return BAD;

        if(MyLibrary::CheckBoxHit(m_judgement_line_below[MISS].x,
                                  m_judgement_line_below[MISS].y,
                                  m_judgement_line_below[MISS].w,
                                  m_judgement_line_below[MISS].h,
                                  note_pos.x,
                                  note_pos.y,
                                  NOTES_W,
                                  NOTES_H))
            return MISS;
    }
    return NORMAL;
}

//-----------------------------------------------------------
//! @brief 判定結果に応じた処理の適用
//! @param result 判定結果
//-----------------------------------------------------------
void LineObject::ApplyJudgement(int result)
{
    m_judgement_mode = result;

    switch(result) {
    case PERFECT:
        m_perfect_count++;
        // パーフェクト時のスコア加算
        m_total_score += 100;
        break;
    case GOOD:
        m_good_count++;
        // グッド時のスコア加算
        m_total_score += 50;
        break;
    case BAD:
        m_bad_count++;
        // バッド時のスコア加算
        m_total_score += 10;
        break;
    case MISS:
        m_miss_count++;
        // ミス時のスコア加算
        m_total_score += 0;
        break;
    }

    // 判定表示用のフレーム数をリセット
    m_frame_count = 0;

    // 判定エフェクト生成
    unsigned int ef_color = COLOR_WHITE;
    switch(result) {
    case PERFECT:
        ef_color = COLOR_PINK;
        break;
    case GOOD:
        ef_color = COLOR_YELLOW_GREEN;
        break;
    case BAD:
        ef_color = COLOR_GRAY;
        break;
    case MISS:
        ef_color = COLOR_RED;
        break;
    }

    // エフェクトを追加（最大 16 まで）
    if(static_cast<int>(m_judge_effects.size()) < JUDGE_EFFECTS_DRAW_LIMIT) {
        JudgeEffect ef;
        ef.OnInit();
        // 判定ライン中央に出す
        float cx = m_line_box.x + (m_line_box.w * 0.5f);
        float cy = m_draw_line.pos_1.y;
        ef.Start(cx, cy, ef_color, JUDGEMENT_DRAW_TIME_MAX);
        m_judge_effects.push_back(std::move(ef));
    }
}

//-----------------------------------------------------------
//! @brief  合計スコアをリセット
//-----------------------------------------------------------
void LineObject::ResetTotalScore()
{
    m_total_score = 0;
}

//-----------------------------------------------------------
//! @brief  MISS回数の取得
//! @return MISS回数
//-----------------------------------------------------------
int LineObject::GetMissCount() const
{
    return m_miss_count;
}

//-----------------------------------------------------------
//! @brief  GOOD回数の取得
//! @return GOOD回数
//-----------------------------------------------------------
int LineObject::GetGoodCount() const
{
    return m_good_count;
}

//-----------------------------------------------------------
//! @brief  PERFECT回数の取得
//! @return PERFECT回数
//-----------------------------------------------------------
int LineObject::GetPerfectCount() const
{
    return m_perfect_count;
}

//-----------------------------------------------------------
//! @brief  BAD回数の取得
//! @return BAD回数
//-----------------------------------------------------------
int LineObject::GetBadCount() const
{
    return m_bad_count;
}
}    // namespace Object
