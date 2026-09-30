#pragma once
#include "SceneBase.h"
#include "ImageObject.h"
#include "ResourceHandles.h"
#include "Structs.h"

namespace Scene {
//-----------------------------------------------------------
//! @class  ResultScene
//! @detail リザルト画面の処理。スコア表示などを行う
//-----------------------------------------------------------
class ResultScene : public SceneBase
{
public:
    //-----------------------------------------------------------
    //! @brief シーン初期化処理
    //-----------------------------------------------------------
    void SceneInit() override;

    //-----------------------------------------------------------
    //! @brief シーン更新処理
    //-----------------------------------------------------------
    void SceneUpdate() override;

    //-----------------------------------------------------------
    //! @brief シーン描画処理
    //-----------------------------------------------------------
    void SceneDraw() override;

    //-----------------------------------------------------------
    //! @brief シーン終了処理
    //-----------------------------------------------------------
    void SceneEnd() override;

    //-----------------------------------------------------------
    //! @brief  ライン判定結果を設定
    //! @param [in] lineResults ライン毎の判定結果の配列
    //-----------------------------------------------------------
    static void SetLineJudgeResults(const std::vector<LineJudgeResult>& lineResults);

    //-----------------------------------------------------------
    //! @brief  ライン判定結果を取得
    //! @return ライン毎の判定結果の配列
    //-----------------------------------------------------------
    static const std::vector<LineJudgeResult>& GetLineJudgeResults();

private:
    static std::vector<LineJudgeResult> m_line_judge_results;    //!< ライン毎の判定結果
    Object::ImageObject                 m_back_image;            //!< 背景画像ハンドル（RAII）
};
}    // namespace Scene
