#pragma once
#include "SceneBase.h"
#include "ResourceHandles.h"
#include "ConstantsRhythm.h"
#include <vector>
#include <string>

namespace Scene {
//-----------------------------------------------------------
//! @class  SongSelectScene
//! @detail 曲選択画面の実装
//-----------------------------------------------------------
class SongSelectScene : public SceneBase
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
    //! @brief 選択した曲のインデックスを取得
    //-----------------------------------------------------------
    int GetSelectedIndex() const;

    // (遊び方のトグルと描画はファイル内の静的関数で実装します)

private:
    // 曲選択に必要なローカル状態（メンバ化して軽量化）
    std::vector<std::string> m_song_list;              //!< 曲一覧（表示名）
    std::vector<std::string> m_song_paths;             //!< 曲データファイルパス
    std::vector<std::string> m_song_titles;            //!< 曲タイトル（data.txt から読み取る）
    std::vector<int>         m_song_bpm;               //!< 曲 BPM（data.txt から読み取る）
    std::vector<ImageHandle> m_song_icons;             //!< 曲アイコンハンドル（RAII）
    std::vector<int>         m_song_best_scores;       //!< 各曲のベストスコア
    int                      m_selected_index = 0;     //!< 選択中インデックス
    ImageHandle              m_back_image;             //!< 背景（RAII）
    bool                     m_is_selected = false;    //!< 選択確定フラグ

    // 事前に読み込んだ音声ハンドル（曲プレビュー用）
    std::vector<SoundHandle> m_preloaded_sound_handles;                                //!< 事前読み込みした音声ハンドル
    int                      m_current_preview_index        = -1;                      //!< 現在再生中のプレビューインデックス
    int                      m_preview_volume               = DEFAULT_SOUND_VOLUME;    //!< プレビュー再生時の音量(0-255)
    int                      m_preview_volume_display_timer = 0;                       //!< プレビュー音量表示タイマー(フレーム)

    // フォントハンドルのキャッシュ（描画で頻繁に使うためメンバ化）
    int m_font50 = 0;    //!< ヘッダ用フォントハンドル
    int m_font30 = 0;    //!< 中位フォントハンドル
    int m_font25 = 0;    //!< 小さいフォントハンドル
};
}    // namespace Scene
