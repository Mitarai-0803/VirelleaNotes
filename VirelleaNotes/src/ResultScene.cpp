//----------------------------------------------------------------------------
//! @file   ResultScene.cpp
//! @brief  リザルトシーンの実装
//! @detail ゲーム終了後のリザルト画面を管理し、スコアやライン毎の判定結果を表示します。
//----------------------------------------------------------------------------
#include "ResultScene.h"
#include "InputManager.h"
#include <DxLib.h>
#include "Colors.h"
#include "ConstantsRhythm.h"
#include "UIConstants.h"
#include "DrawFormatStringObject.h"
#include "Font.h"
#include "SceneManager.h"
#include "CSVLoadObject.h"
#include <filesystem>
#include "ResultSongIconObject.h"

namespace Scene {
//-----------------------------------------------------------
// ライン毎の判定結果の格納
//-----------------------------------------------------------
std::vector<LineJudgeResult> ResultScene::m_line_judge_results;    //!< ライン毎の判定結果

//-------------------------------------------------------
//! シーン初期化処理
//-------------------------------------------------------
void ResultScene::SceneInit()
{
    //--------------------------------------------------------
    // リザルト背景画像オブジェクトの追加
    //--------------------------------------------------------
    {
        auto bg = std::make_shared<Object::ImageObject>();
        bg->SetName("ResultBackground");
        bg->LoadImage(RESULT_BG_IMAGE_PATH);
        bg->SetPosition(0, 0);
        bg->SetSize(SCREEN_W, SCREEN_H);
        AddObject(bg, 0);    // 背景は最背面
    }

    //--------------------------------------------------------
    // リザルトタイトル文字列オブジェクトの追加
    //--------------------------------------------------------
    AddObjectSetName<UI::DrawFormatStringObject>("ResultTitleString");
    if(auto title_str = GetSceneObject<UI::DrawFormatStringObject>("ResultTitleString")) {
        title_str->SetColor(COLOR_WHITE);
        title_str->SetFontHandle(GetFont(Font::FONT_SIZE_50_INDEX));
        title_str->SetFormat(RESULT_TITLE_STR);
        title_str->ApplyFormat();

        // 文字列の幅を取得して中央揃え
        int str_w = GetDrawStringWidthToHandle(RESULT_TITLE_STR, -1, GetFont(Font::FONT_SIZE_50_INDEX));
        title_str->SetPos(RESULT_TITLE_X - (str_w * HALF), RESULT_TITLE_Y);
    }

    //--------------------------------------------------------
    // 全ラインの判定結果を合計
    //--------------------------------------------------------
    int totalPerfect = 0;
    int totalGood    = 0;
    int totalBad     = 0;
    int totalMiss    = 0;
    int totalScore   = 0;

    for(const auto& result : m_line_judge_results) {
        totalPerfect += result.perfectCount;
        totalGood    += result.goodCount;
        totalBad     += result.badCount;
        totalMiss    += result.missCount;
        totalScore   += result.totalScore;
    }

    //--------------------------------------------------------
    // 合計PERFECT表示オブジェクト
    //--------------------------------------------------------
    AddObjectSetName<UI::DrawFormatStringObject>("TotalPerfect");
    if(auto perfect_str = GetSceneObject<UI::DrawFormatStringObject>("TotalPerfect")) {
        perfect_str->SetColor(COLOR_PINK);
        perfect_str->SetFontHandle(GetFont(Font::FONT_SIZE_30_INDEX));
        perfect_str->SetFormat(RESULT_JUDG_STR_PERFECT);
        perfect_str->SetParam(PARAMETER_JUDGE_PERFECT, totalPerfect);
        perfect_str->ApplyFormat();
        perfect_str->SetPos(RESULT_JUDG_START_X, RESULT_JUDG_START_Y);
    }

    //--------------------------------------------------------
    // 合計GOOD表示オブジェクト
    //--------------------------------------------------------
    AddObjectSetName<UI::DrawFormatStringObject>("TotalGood");
    if(auto good_str = GetSceneObject<UI::DrawFormatStringObject>("TotalGood")) {
        good_str->SetColor(COLOR_YELLOW_GREEN);
        good_str->SetFontHandle(GetFont(Font::FONT_SIZE_30_INDEX));
        good_str->SetFormat(RESULT_JUDG_STR_GOOD);
        good_str->SetParam(PARAMETER_JUDGE_GOOD, totalGood);
        good_str->ApplyFormat();
        good_str->SetPos(RESULT_JUDG_START_X, RESULT_JUDG_START_Y + RESULT_JUDG_LINE_HEIGHT);
    }

    //--------------------------------------------------------
    // 合計BAD表示オブジェクト
    //--------------------------------------------------------
    AddObjectSetName<UI::DrawFormatStringObject>("TotalBad");
    if(auto bad_str = GetSceneObject<UI::DrawFormatStringObject>("TotalBad")) {
        bad_str->SetColor(COLOR_GRAY);
        bad_str->SetFontHandle(GetFont(Font::FONT_SIZE_30_INDEX));
        bad_str->SetFormat(RESULT_JUDG_STR_BAD);
        bad_str->SetParam(PARAMETER_JUDGE_BAD, totalBad);
        bad_str->ApplyFormat();
        bad_str->SetPos(RESULT_JUDG_START_X, RESULT_JUDG_START_Y + RESULT_JUDG_LINE_HEIGHT * 2);
    }

    //--------------------------------------------------------
    // 合計MISS表示オブジェクト
    //--------------------------------------------------------
    AddObjectSetName<UI::DrawFormatStringObject>("TotalMiss");
    if(auto miss_str = GetSceneObject<UI::DrawFormatStringObject>("TotalMiss")) {
        miss_str->SetColor(COLOR_RED);
        miss_str->SetFontHandle(GetFont(Font::FONT_SIZE_30_INDEX));
        miss_str->SetFormat(RESULT_JUDG_STR_MISS);
        miss_str->SetParam(PARAMETER_JUDGE_MISS, totalMiss);
        miss_str->ApplyFormat();
        miss_str->SetPos(RESULT_JUDG_START_X, RESULT_JUDG_START_Y + RESULT_JUDG_LINE_HEIGHT * 3);
    }

    //--------------------------------------------------------
    // 合計SCORE表示オブジェクト
    //--------------------------------------------------------
    AddObjectSetName<UI::DrawFormatStringObject>("TotalScore");
    if(auto score_str = GetSceneObject<UI::DrawFormatStringObject>("TotalScore")) {
        score_str->SetColor(COLOR_WHITE);
        score_str->SetFontHandle(GetFont(Font::FONT_SIZE_40_INDEX));
        score_str->SetFormat(RESULT_JUDG_STR_SCORE);
        score_str->SetParam(PARAMETER_SCORE, totalScore);
        score_str->ApplyFormat();
        score_str->SetPos(RESULT_JUDG_START_X, RESULT_JUDG_START_Y + RESULT_JUDG_LINE_HEIGHT * 4);
    }

    //--------------------------------------------------------
    // 操作ガイド文字列オブジェクトの追加
    //--------------------------------------------------------
    AddObjectSetName<UI::DrawFormatStringObject>("GuideString");
    if(auto guide_str = GetSceneObject<UI::DrawFormatStringObject>("GuideString")) {
        guide_str->SetColor(COLOR_WHITE);
        guide_str->SetFontHandle(GetFont(Font::FONT_SIZE_20_INDEX));
        guide_str->SetFormat(RESULT_GUIDE_STR);
        guide_str->ApplyFormat();

        // 文字列の幅を取得して中央揃え
        int str_w = GetDrawStringWidthToHandle(RESULT_GUIDE_STR, -1, GetFont(Font::FONT_SIZE_20_INDEX));
        guide_str->SetPos(RESULT_GUIDE_X - (str_w * HALF), RESULT_GUIDE_Y);
    }

    //--------------------------------------------------------
    // 曲アイコン表示オブジェクトの追加
    //--------------------------------------------------------
    {
        // 右側表示用のオブジェクトを追加
        auto icon_obj = std::make_shared<Object::ResultSongIconObject>();
        // 座標とサイズを設定
        icon_obj->SetPosition(RESULT_ICON_X, RESULT_ICON_Y);
        icon_obj->SetSize(RESULT_ICON_SIZE);
        icon_obj->SetName("ResultSongIcon");
        AddObject(icon_obj, 1);
    }
}

//-------------------------------------------------------
//! シーン更新処理
//-------------------------------------------------------
void ResultScene::SceneUpdate()
{
    //--------------------------------------------------------
    // スペースキーで曲選択画面へ遷移
    //--------------------------------------------------------
    if(InputManager::PushHitKey(KEY_INPUT_SPACE)) {
        ChangeScene(Rhythm::SceneName::SONG_SELECT);
    }
}

//-------------------------------------------------------
//! シーン描画処理
//-------------------------------------------------------
void ResultScene::SceneDraw()
{
}

//-------------------------------------------------------
//! シーン終了処理
//-------------------------------------------------------
void ResultScene::SceneEnd()
{
}

//-------------------------------------------------------
//! ライン判定結果を設定
//-------------------------------------------------------
void ResultScene::SetLineJudgeResults(const std::vector<LineJudgeResult>& lineResults)
{
    m_line_judge_results = lineResults;
}

//-------------------------------------------------------
//! ライン判定結果を取得
//-------------------------------------------------------
const std::vector<LineJudgeResult>& ResultScene::GetLineJudgeResults()
{
    return m_line_judge_results;
}
}    // namespace Scene
