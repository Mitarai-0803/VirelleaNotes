//----------------------------------------------------------------------------
//! @file   GameScene.cpp
//! @brief  ゲームシーンの実装
//! @detail シーンの初期化、更新、描画、終了処理を提供し、各UIオブジェクトやコンポーネント間の連携を管理します。
//----------------------------------------------------------------------------
#include "GameScene.h"
#include "LineObject.h"
#include "ConstantsRhythm.h"
#include "ObjectBase.h"
#include "CSVLoadObject.h"
#include "Functions.h"
#include "AudioManagerComponent.h"
#include "Colors.h"
#include "SoundVisualizerObject.h"
#include "DrawBoxComponent.h"
#include "DrawFormatStringObject.h"
#include "SceneManager.h"
#include "ResultScene.h"
#include "PauseObject.h"
#include <filesystem>
#include <fstream>
#include "InputManager.h"

namespace Scene {
//-----------------------------------------------------------
// メンバ: 選択された曲インデックスの格納
//-----------------------------------------------------------
static int  GameSelectedIndex        = -1;                      // 外部から渡される選択インデックス
static bool SceneTransitionRequested = false;                   // シーン遷移リクエスト済みフラグ
static bool SceneIsPaused            = false;                   // ポーズ状態フラグ
static bool SceneIsCountingDown      = false;                   // カウントダウン実行中フラグ
static int  CountdownTimer           = 0;                       // カウントダウン残フレーム
static int  PrevCountdownSecond      = -1;                      // 前回表示した秒
static int  CountdownSoundHandle     = -1;                      // カウントダウンSEハンドル
static int  GameVolume               = DEFAULT_SOUND_VOLUME;    // ゲーム再生時の音量(0-255)
static int  GameVolumeDisplayTimer   = 0;                       // ゲーム音量表示タイマー(フレーム)
static bool ShowHowto                = false;                   // 遊び方表示フラグ
static int  HowtoImageHandle         = -1;                      // 遊び方画像ハンドル

//-----------------------------------------------------------
//! @brief シーンの初期化処理
//! @details 必要なオブジェクトを生成し、コンポーネントを登録する
//-----------------------------------------------------------
void GameScene::SceneInit()
{
    // シーン遷移フラグを初期化
    SceneTransitionRequested = false;

    // シーン開始時はポーズ/カウントダウン状態をリセットする
    SceneIsPaused       = false;
    SceneIsCountingDown = false;
    ShowHowto           = false;

    // 遊び方画像を初期化時に読み込む（シーン終了で解放する）
    if(HowtoImageHandle == -1) {
        HowtoImageHandle = LoadGraph(HOWTO_IMAGE_PATH);
    }

    // フォントハンドルのキャッシュ
    m_font50 = GetFont(Font::FONT_SIZE_50_INDEX);
    m_font40 = GetFont(Font::FONT_SIZE_40_INDEX);
    m_font25 = GetFont(Font::FONT_SIZE_25_INDEX);

    // 四角描画オブジェクトの追加
    {
        auto draw_box_obj = std::make_shared<Object::ObjectBase>();    // オブジェクトの作成
        if(draw_box_obj) {
            draw_box_obj->SetName("DrawBox");                      // 名前の設定
            draw_box_obj->Init();                                  // オブジェクトの初期化処理
            draw_box_obj->AddComponent<UI::DrawBoxComponent>();    // コンポーネントの追加
            AddObject(draw_box_obj);                               // シーンにオブジェクトを追加
        }

        // (カウントダウン状態取得は GameScene::IsCountingDown で提供)

        // ゲーム音量表示タイマー更新
        if(GameVolumeDisplayTimer > 0)
            --GameVolumeDisplayTimer;
    }

    // PauseObject を追加してポーズ描画をオブジェクト化
    {
        auto pause_obj = std::make_shared<Object::PauseObject>();
        if(pause_obj) {
            pause_obj->SetName("PauseObject");
            pause_obj->Init();
            AddObject(pause_obj, 10000);    // 描画順を最前面に
        }
    }

    // ゲーム音量調整 (左右キーで変更)
    if(InputManager::PushHitKey(KEY_INPUT_LEFT)) {
        GameVolume -= VOLUME_STEP;
        if(GameVolume < 0)
            GameVolume = 0;
        // オーディオマネージャーに反映
        if(auto audio_manager_obj = GetSceneObject<Object::ObjectBase>("AudioManagerObject")) {
            if(auto audio_manager_cmp = audio_manager_obj->GetComponent<Audio::AudioManagerComponent>()) {
                audio_manager_cmp->SetSoundVolume(GameVolume);
            }
        }
        GameVolumeDisplayTimer = FRAME_RATE * VOLUME_DISPLAY_SECONDS;    // 表示秒数
    }
    if(InputManager::PushHitKey(KEY_INPUT_RIGHT)) {
        GameVolume += VOLUME_STEP;
        if(GameVolume > 255)
            GameVolume = 255;
        // オーディオマネージャーに反映
        if(auto audio_manager_obj = GetSceneObject<Object::ObjectBase>("AudioManagerObject")) {
            if(auto audio_manager_cmp = audio_manager_obj->GetComponent<Audio::AudioManagerComponent>()) {
                audio_manager_cmp->SetSoundVolume(GameVolume);
            }
        }
        GameVolumeDisplayTimer = FRAME_RATE * VOLUME_DISPLAY_SECONDS;    // 表示秒数
    }

    // 四角描画オブジェクトの取得
    if(auto draw_box_obj = GetSceneObject<Object::ObjectBase>("DrawBox")) {
        // 四角描画コンポーネントの取得
        if(auto draw_box_com = draw_box_obj->GetComponent<UI::DrawBoxComponent>()) {
            // 四角描画コンポーネントの設定
            draw_box_com->SetBoxSize(DRAW_BOX_X, DRAW_BOX_Y, DRAW_BOX_W, DRAW_BOX_H);

            // 描画する四角の色の設定
            draw_box_com->SetColor(COLOR_NAVY_BLUE);
        }
    }

    // 書式付き文字オブジェクトの追加
    // 現在時間の表示用文字列オブジェクト
    AddObjectSetName<UI::DrawFormatStringObject>("TimeStringObject");
    if(auto time_str_obj = GetSceneObject<UI::DrawFormatStringObject>("TimeStringObject")) {
        // 文字の色、座標、フォントの設定
        time_str_obj->SetColor(COLOR_WHITE);
        time_str_obj->SetFontHandle(GetFont(Font::FONT_SIZE_50_INDEX));
        time_str_obj->SetFormat(TIME_STR);
    }

    // スコアの表示用文字列オブジェクト
    AddObjectSetName<UI::DrawFormatStringObject>("ScoreStringObject");
    if(auto score_str_obj = GetSceneObject<UI::DrawFormatStringObject>("ScoreStringObject")) {
        // 文字の色、座標、フォントの設定
        score_str_obj->SetColor(COLOR_WHITE);
        score_str_obj->SetFontHandle(GetFont(Font::FONT_SIZE_40_INDEX));
        score_str_obj->SetFormat(SCORE_STR);
    }

    // CSVと音源のペアを先に走査
    Object::CSVLoadObject::RefreshAvailableDataSets("data");

    // 外部から渡された選択インデックスがある場合はそれを使用
    // すでに SongSelectScene 等で CSVLoadObject::SetSelectedIndex が呼ばれていれば
    // それを優先し、未設定の場合のみデフォルト選択を行う
    const auto& datasets = Object::CSVLoadObject::GetAvailableDataSets();
    if(GameSelectedIndex >= 0 && GameSelectedIndex < static_cast<int>(datasets.size())) {
        Object::CSVLoadObject::SetSelectedIndex(GameSelectedIndex);
    }
    else {
        // CSVLoadObject 側の選択インデックスが未設定ならデフォルトを設定
        int currentSelected = Object::CSVLoadObject::GetSelectedIndex();
        if(currentSelected < 0) {
            if(datasets.size() >= 2) {
                Object::CSVLoadObject::SetSelectedIndex(1);    // 2番目を選択
            }
            else if(!datasets.empty()) {
                Object::CSVLoadObject::SetSelectedIndex(0);
            }
        }
    }

    // CSVと音源のペア読み込みオブジェクトの追加
    AddObjectSetName<Object::CSVLoadObject>("CSVLoadObject");

    // ラインの数分ループ
    for(int i = 0; i < NOTES_LINE_MAX; ++i) {
        std::string loop_str = std::to_string(i);    // ループ回数を文字列に変換

        // ラインの隙間を開ける場所じゃなければ(2～3)
        if(i != 2 && i != 3) {
            // ノーツを流すラインオブジェクトの追加
            AddObjectSetName<Object::LineObject>("LineObject" + loop_str);

            // ノーツを流すラインの座標の設定
            if(auto line_obj = GetSceneObject<Object::LineObject>("LineObject" + loop_str)) {
                line_obj->SetLineBoxPos(NOTES_LINE_X * i, NOTES_LINE_Y, NOTES_LINE_W, NOTES_LINE_H);

                // 判定ラインの座標の設定
                line_obj->SetDrawLinePos(
                    LINE_JUDGEMENT_X_LEFT_UP * i, LINE_JUDGEMENT_Y_LEFT_UP, LINE_JUDGEMENT_X_RIGHT_DOWN * (i + 1), LINE_JUDGEMENT_Y_RIGHT_DOWN);
            }
        }
    }

    // 入力キーの設定
    if(auto line_obj = GetSceneObject<Object::LineObject>("LineObject0")) {
        line_obj->SetKey(KEY_INPUT_A, "A");
    }
    if(auto line_obj = GetSceneObject<Object::LineObject>("LineObject1")) {
        line_obj->SetKey(KEY_INPUT_S, "S");
    }
    if(auto line_obj = GetSceneObject<Object::LineObject>("LineObject4")) {
        line_obj->SetKey(KEY_INPUT_K, "K");
    }
    if(auto line_obj = GetSceneObject<Object::LineObject>("LineObject5")) {
        line_obj->SetKey(KEY_INPUT_L, "L");
    }

    // サウンドビジュアライザーオブジェクトの追加
    AddObjectSetName<Object::SoundVisualizerObject>("SoundVisualizerObject");

    // オーディオマネージャーオブジェクトの追加
    auto audio_manager_obj = std::make_shared<Object::ObjectBase>();    // オブジェクトの作成
    if(audio_manager_obj) {
        audio_manager_obj->SetName("AudioManagerObject");                   // 名前の設定
        audio_manager_obj->Init();                                          // オブジェクトの初期化処理
        audio_manager_obj->AddComponent<Audio::AudioManagerComponent>();    // コンポーネントの追加
        AddObject(audio_manager_obj);                                       // シーンにオブジェクトを追加
    }

    // CSVと音源のペア読み込みオブジェクトの取得
    if(auto csvload_obj = GetSceneObject<Object::CSVLoadObject>("CSVLoadObject")) {
        // サウンドビジュアライザーオブジェクトの取得
        if(auto sound_visualizer_obj = GetSceneObject<Object::SoundVisualizerObject>("SoundVisualizerObject")) {
            sound_visualizer_obj->SetSoftHandle(csvload_obj->GetSoftHandle());    // 解析音源ハンドルの設定

            // ビジュアライザーの描画位置の設定
            sound_visualizer_obj->SetPosition(SOUND_VISUALIZER_X, SOUND_VISUALIZER_Y);
        }

        // オーディオマネージャーオブジェクトの取得
        if(auto audio_manager_obj = GetSceneObject<Object::ObjectBase>("AudioManagerObject")) {
            if(auto audio_manager_cmp = audio_manager_obj->GetComponent<Audio::AudioManagerComponent>()) {
                // 音源ハンドルの設定。再生はカウントダウン後に行う
                if(csvload_obj->GetHandle()) {
                    audio_manager_cmp->SetHandle(csvload_obj->GetHandle());     // 音源ハンドルの設定
                    audio_manager_cmp->SetSoundVolume(DEFAULT_SOUND_VOLUME);    // 音源音量の設定(0～255)
                    // 一旦停止してカウントダウンを開始
                    audio_manager_cmp->HandleStopSound();
                    SceneIsCountingDown = true;
                    CountdownTimer      = COUNTDOWN_FRAMES;
                    PrevCountdownSecond = -1;
                    if(CountdownSoundHandle == -1) {
                        CountdownSoundHandle = LoadSoundMem(COUNTDOWN_SE_PATH);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------
//! @brief シーンの更新処理
//-----------------------------------------------------------
void GameScene::SceneUpdate()
{
    // H キーで遊び方のトグル表示
    if(InputManager::PushHitKey(KEY_INPUT_H)) {
        ShowHowto = !ShowHowto;
        // 表示中はゲームをポーズ扱いにして音を止める
        if(ShowHowto) {
            SceneIsPaused = true;
            if(auto audio_manager_obj = GetSceneObject<Object::ObjectBase>("AudioManagerObject")) {
                if(auto audio_manager_cmp = audio_manager_obj->GetComponent<Audio::AudioManagerComponent>()) {
                    audio_manager_cmp->HandleStopSound();
                }
            }
        }
        else {
            // 閉じるときはカウントダウンで再開
            SceneIsPaused = false;
            if(auto audio_manager_obj = GetSceneObject<Object::ObjectBase>("AudioManagerObject")) {
                if(auto audio_manager_cmp = audio_manager_obj->GetComponent<Audio::AudioManagerComponent>()) {
                    audio_manager_cmp->HandleStopSound();
                }
            }
            SceneIsCountingDown = true;
            CountdownTimer      = COUNTDOWN_FRAMES;
            PrevCountdownSecond = -1;
            if(CountdownSoundHandle == -1) {
                CountdownSoundHandle = LoadSoundMem(COUNTDOWN_SE_PATH);
            }
        }
    }
    // ESC キーでポーズトグル
    if(InputManager::PushHitKey(KEY_INPUT_ESCAPE)) {
        // ポーズ状態をトグル（カウント中でもポーズに戻せるようにする）
        SceneIsPaused = !SceneIsPaused;

        if(auto audio_manager_obj = GetSceneObject<Object::ObjectBase>("AudioManagerObject")) {
            if(auto audio_manager_cmp = audio_manager_obj->GetComponent<Audio::AudioManagerComponent>()) {
                if(SceneIsPaused) {
                    // ポーズ時は音声を停止
                    audio_manager_cmp->HandleStopSound();
                }
                else {
                    // ポーズ解除時は即時再生せず、カウントダウンを開始してから再生する
                    audio_manager_cmp->HandleStopSound();
                    SceneIsCountingDown = true;
                    CountdownTimer      = COUNTDOWN_FRAMES;
                    PrevCountdownSecond = -1;
                    // カウントダウンSEを未ロードならロード
                    if(CountdownSoundHandle == -1) {
                        CountdownSoundHandle = LoadSoundMem(COUNTDOWN_SE_PATH);
                    }
                }
            }
        }
    }

    // ポーズ中の入力処理（復帰以外の操作を受け付ける）
    if(SceneIsPaused) {
        // タイトルへ戻る (T)
        if(InputManager::PushHitKey(KEY_INPUT_T)) {
            if(auto audio_manager_obj = GetSceneObject<Object::ObjectBase>("AudioManagerObject")) {
                if(auto audio_manager_cmp = audio_manager_obj->GetComponent<Audio::AudioManagerComponent>()) {
                    audio_manager_cmp->HandleStopSound();
                }
            }
            ChangeScene(Rhythm::SceneName::TITLE);
            return;
        }

        // 曲選択画面に戻る (S)
        if(InputManager::PushHitKey(KEY_INPUT_S)) {
            if(auto audio_manager_obj = GetSceneObject<Object::ObjectBase>("AudioManagerObject")) {
                if(auto audio_manager_cmp = audio_manager_obj->GetComponent<Audio::AudioManagerComponent>()) {
                    audio_manager_cmp->HandleStopSound();
                }
            }
            ChangeScene(Rhythm::SceneName::SONG_SELECT);
            return;
        }

        // ポーズ中はゲーム更新をスキップ
        // カウントダウン解除トリガーがない場合は早期リターン
        return;
    }

    // カウントダウン中の処理: タイマー更新と音源再生開始
    if(SceneIsCountingDown) {
        --CountdownTimer;
        int sec = (CountdownTimer + FRAME_RATE - 1) / FRAME_RATE;    // 切り上げで表示
        if(sec != PrevCountdownSecond) {
            PrevCountdownSecond = sec;
            // SE 再生
            if(CountdownSoundHandle != -1) {
                PlaySoundMem(CountdownSoundHandle, DX_PLAYTYPE_BACK, TRUE);
            }
        }

        if(CountdownTimer <= 0) {
            // カウントダウン終了: 音源を再生
            if(auto audio_manager_obj = GetSceneObject<Object::ObjectBase>("AudioManagerObject")) {
                if(auto audio_manager_cmp = audio_manager_obj->GetComponent<Audio::AudioManagerComponent>()) {
                    audio_manager_cmp->HandlePlaySound();
                }
            }
            SceneIsCountingDown = false;
            PrevCountdownSecond = -1;
        }
        // カウントダウン中はゲーム更新をスキップ
        return;
    }

    // CSVファイル読み込みオブジェクトの取得
    if(auto csv_load_obj = GetSceneObject<Object::CSVLoadObject>("CSVLoadObject")) {
        // CSVファイルが読み込めるならデータを取得
        auto& notes = const_cast<std::vector<NoteData>&>(csv_load_obj->GetNotesData());
        if(!notes.empty()) {
            for(auto& note : notes) {
                if(note.spawned)
                    continue;    // すでに生成済みならスキップ

                // ノーツは音源の再生時間(ms)を基準に生成。音源が再生されていない場合はフレーム基準で生成
                int current_ms = 0;
                if(auto audio_manager_obj = GetSceneObject<Object::ObjectBase>("AudioManagerObject")) {
                    if(auto audio_manager_cmp = audio_manager_obj->GetComponent<Audio::AudioManagerComponent>()) {
                        if(audio_manager_cmp->GetPlayeMode()) {
                            current_ms = audio_manager_cmp->GetMsTime();
                        }
                    }
                }

                bool shouldSpawn = false;
                if(current_ms > 0) {
                    // 音源再生位置(ms)がノーツ時間を越えたら生成
                    shouldSpawn = (current_ms >= note.time_ms);
                }
                else {
                    // フレーム基準に変換して比較
                    int spawnFrame = static_cast<int>(note.time_ms * (static_cast<float>(FRAME_RATE) / 1000.0f) + 2.0f);
                    shouldSpawn    = (m_frame_count >= spawnFrame);
                }

                if(shouldSpawn) {
                    std::string line_str;    // 流すラインの文字列変数

                    // ラインが2か3じゃなければそのまま
                    if(note.line != 2 && note.line != 3) {
                        line_str = std::to_string(note.line);
                    }
                    else {
                        // 除外列なら+2して変換
                        line_str = std::to_string(note.line + 2);
                    }

                    // 流すラインがあるなら
                    if(auto line_obj = GetSceneObject<Object::LineObject>("LineObject" + line_str)) {
                        line_obj->AddNotes(note.type);    // ノーツ生成(ノーツタイプも設定)
                        note.spawned = true;              // 生成済みフラグをオン
                    }
                }
            }
        }
    }

    //--------------------------------------------------------
    // 音楽再生終了の確認とシーン遷移
    //--------------------------------------------------------
    // シーン遷移がまだリクエストされていない場合のみチェック
    if(!SceneTransitionRequested) {
        // オーディオマネージャーオブジェクトの取得
        if(auto audio_manager_obj = GetSceneObject<Object::ObjectBase>("AudioManagerObject")) {
            // オーディオマネージャーコンポーネントの取得
            if(auto audio_manager_cmp = audio_manager_obj->GetComponent<Audio::AudioManagerComponent>()) {
                // 音源ハンドルの取得
                if(auto csv_load_obj = GetSceneObject<Object::CSVLoadObject>("CSVLoadObject")) {
                    int audio_handle = csv_load_obj->GetHandle();

                    // 音源ハンドルが有効で、再生中だったが終了した場合
                    if(audio_handle != 0 && audio_manager_cmp->GetPlayeMode()) {
                        // 音源の再生状態を確認(0:停止中, 1:再生中)
                        if(CheckSoundMem(audio_handle) == 0) {
                            //------------------------------------
                            // ライン毎の判定結果を収集
                            //------------------------------------
                            std::vector<LineJudgeResult> lineResults{};

                            // ラインの数分ループ
                            for(int i = 0; i < NOTES_LINE_MAX; ++i) {
                                // ラインの隙間を開ける場所じゃなければ
                                if(i != 2 && i != 3) {
                                    std::string loop_str = std::to_string(i);    // ループ回数を文字列に変換

                                    // ノーツを流すラインオブジェクトの取得
                                    if(auto line_obj = GetSceneObject<Object::LineObject>("LineObject" + loop_str)) {
                                        LineJudgeResult result{};
                                        result.missCount    = line_obj->GetMissCount();       // MISS回数の取得
                                        result.badCount     = line_obj->GetBadCount();        // BAD回数の取得
                                        result.goodCount    = line_obj->GetGoodCount();       // GOOD回数の取得
                                        result.perfectCount = line_obj->GetPerfectCount();    // PERFECT回数の取得
                                        result.totalScore   = line_obj->GetTotalScore();      // 合計スコアの取得

                                        lineResults.push_back(result);    // 結果を配列に追加
                                    }
                                }
                            }

                            // リザルトシーンに判定結果を設定
                            Scene::ResultScene::SetLineJudgeResults(lineResults);

                            // 合計スコアを算出してベストスコアを保存
                            int totalScoreAll = 0;
                            for(const auto& lr : lineResults) totalScoreAll += lr.totalScore;

                            // 利用可能なデータセットから現在の選択ディレクトリを取得
                            {
                                const auto& datasets = Object::CSVLoadObject::GetAvailableDataSets();
                                int         sel      = Object::CSVLoadObject::GetSelectedIndex();
                                if(sel >= 0 && sel < static_cast<int>(datasets.size())) {
                                    const std::string     csvpath = datasets[sel].csv_path;
                                    std::filesystem::path p(csvpath);
                                    // csv/data.csv の親ディレクトリが dataset dir
                                    std::filesystem::path dataset_dir = p.parent_path().parent_path();
                                    std::filesystem::path score_file  = dataset_dir / "bestscore.txt";
                                    // 既存スコア読み込み
                                    int existing = 0;
                                    {
                                        std::ifstream ifs(score_file.string());
                                        if(ifs)
                                            ifs >> existing;
                                    }
                                    // 更新があれば書き込み
                                    if(totalScoreAll > existing) {
                                        std::ofstream ofs(score_file.string(), std::ios::trunc);
                                        if(ofs)
                                            ofs << totalScoreAll;
                                    }
                                }
                            }

                            // リザルトシーンへ遷移
                            ChangeScene(Rhythm::SceneName::RESULT);

                            // シーン遷移リクエスト済みフラグをオン
                            SceneTransitionRequested = true;
                        }
                    }
                }

                // サウンドビジュアライザーオブジェクトの取得
                if(auto sound_visualizer_obj = GetSceneObject<Object::SoundVisualizerObject>("SoundVisualizerObject")) {
                    // ビジュアライザーの再生位置の取得と設定
                    sound_visualizer_obj->SetSampleTime(audio_manager_cmp->GetSampleTime());
                }
            }
        }
    }

    // 現在時間表示オブジェクトの取得
    if(auto time_str_obj = GetSceneObject<UI::DrawFormatStringObject>("TimeStringObject")) {
        // 音源ハンドルがあるなら
        if(auto audio_handle = GetSceneObject<Object::CSVLoadObject>("CSVLoadObject")->GetHandle()) {
            // 時間取得
            int m_ms_time_pos   = GetSoundCurrentTime(audio_handle);                 // 現在の再生位置(ミリ秒)を取得
            int playTimeMinutes = MyLibrary::ConvertMilliToMinute(m_ms_time_pos);    // ミリ秒から分
            int playTimeSeconds = MyLibrary::ConvertMilliToSec(m_ms_time_pos);       // ミリ秒から秒 (0-59)

            // 現在時間の表示パラメータ設定
            time_str_obj->SetParam(PARAMETER_MINUTES, playTimeMinutes % 60);
            time_str_obj->SetParam(PARAMETER_SECONDS, playTimeSeconds % 60);

            // 書式の適用
            time_str_obj->ApplyFormat();

            // 書式付きでフォントの幅の取得
            float font_w = GetDrawFormatStringWidthToHandle(GetFont(Font::FONT_SIZE_50_INDEX), TIME_STR_W, playTimeMinutes % 60, playTimeSeconds % 60);
            // 現在時間表示のX座標調整
            time_str_obj->SetPos(TIME_STR_X - (font_w * HALF), TIME_STR_Y);
        }
    }

    // スコア表示オブジェクトの取得
    if(auto score_str_obj = GetSceneObject<UI::DrawFormatStringObject>("ScoreStringObject")) {
        int score = 0;    // 合計スコア変数

        // ラインの数分ループ
        for(int i = 0; i < NOTES_LINE_MAX; ++i) {
            // ラインの隙間を開ける場所じゃなければ
            if(i != 2 && i != 3) {
                std::string loop_str = std::to_string(i);    // ループ回数を文字列に変換
                // ノーツを流すラインオブジェクトの取得
                if(auto score_str_obj = GetSceneObject<Object::LineObject>("LineObject" + loop_str)) {
                    score += score_str_obj->GetTotalScore();    // 合計スコアの加算
                }
            }
        }

        // レーンごとのスコア値の取得
        score_str_obj->SetParam(PARAMETER_SCORE, score);    // スコアのパラメータ設定

        // 書式の適用
        score_str_obj->ApplyFormat();

        // 書式付きでフォントの幅の取得
        float font_w = GetDrawFormatStringWidthToHandle(GetFont(Font::FONT_SIZE_40_INDEX), SCORE_STR_W, score);

        // スコア表示のX座標調整
        score_str_obj->SetPos(SCORE_STR_X - (font_w * HALF), SCORE_STR_Y);    // スコア表示のX座標調整
    }
}

//-----------------------------------------------------------
//! @brief シーンの描画処理
//-----------------------------------------------------------
void GameScene::SceneDraw()
{
    // ポーズオーバーレイ描画
    // ポーズ表示は PauseObject が担当するためここでは行わない

    // カウントダウン表示
    if(SceneIsCountingDown) {
        int         sec  = (CountdownTimer + FRAME_RATE - 1) / FRAME_RATE;    // 切り上げ
        const char* text = nullptr;
        static char buf[16];
        snprintf(buf, sizeof(buf), "%d", sec);
        text = buf;

        int font = m_font50;
        int w    = GetDrawStringWidthToHandle(text, -1, font);
        int h    = GetFontSizeToHandle(font);
        int x    = (SCREEN_W / 2) - (w / 2);
        int y    = (SCREEN_H / 2) - (h / 2);

        // 黒の半透明背景を薄く表示
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, COUNTDOWN_BG_ALPHA);
        DrawBox(x - COUNTDOWN_BG_PAD_X, y - COUNTDOWN_BG_PAD_Y, x + w + COUNTDOWN_BG_PAD_X, y + h + COUNTDOWN_BG_PAD_Y, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        // 数字を白で描画
        DrawStringToHandle(x, y, text, GetColor(255, 255, 255), font);
    }

    // ゲーム音量表示（カウントダウン表示とは別に右下に表示）
    if(GameVolumeDisplayTimer > 0) {
        char buf[64];
        int  disp = (GameVolume * 100 + 127) / 255;
        if(disp > 100)
            disp = 100;
        snprintf(buf, sizeof(buf), "Game Volume: %d%%", disp);
        int font = m_font25;
        int w    = GetDrawStringWidthToHandle(buf, -1, font);
        int x    = SCREEN_W - w - VOLUME_DISPLAY_MARGIN_RIGHT;
        int y = SCREEN_H - GetFontSizeToHandle(font) - VOLUME_DISPLAY_MARGIN_BOTTOM - VOLUME_DISPLAY_EXTRA_Y;    // カウントダウンの表示と被らないよう少し上に
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, DRAW_BLEND_ALPHA_SEMI);
        DrawBox(x - VOLUME_DISPLAY_PADDING,
                y - VOLUME_DISPLAY_PADDING,
                x + w + VOLUME_DISPLAY_PADDING,
                y + GetFontSizeToHandle(font) + VOLUME_DISPLAY_PADDING,
                GetColor(0, 0, 0),
                TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        DrawFormatStringToHandle(x, y, GetColor(255, 255, 255), font, "%s", buf);
    }

    // 遊び方画像の表示
    if(ShowHowto && HowtoImageHandle != -1) {
        DrawRotaGraphF(HALF_SCREEN_W, HALF_SCREEN_H, 1.0f, 0.0f, HowtoImageHandle, TRUE);
    }

    // ポーズオブジェクトを最前面で描画（カウント中に ESC でポーズに戻したときに上に表示するため）
    if(SceneIsPaused) {
        if(auto pause_obj = GetSceneObject<Object::ObjectBase>("PauseObject")) {
            // Scene 側で最前面に再描画
            pause_obj->Draw();
        }
    }
}

//-----------------------------------------------------------
//! @brief シーンの終了処理
//-----------------------------------------------------------
void GameScene::SceneEnd()
{
    // 読み込んだ遊び方画像を解放
    if(HowtoImageHandle != -1) {
        DeleteGraph(HowtoImageHandle);
        HowtoImageHandle = -1;
    }

    // カウントダウン SE ハンドルを解放（未解放だとシーンをまたぐたびにリークする）
    if(CountdownSoundHandle != -1) {
        DeleteSoundMem(CountdownSoundHandle);
        CountdownSoundHandle = -1;
    }
}

// 選択された曲のインデックスを設定
void GameScene::SetSelectedSongIndex(int index)
{
    GameSelectedIndex = index;
}

// 設定されている選択インデックスを取得
int GameScene::GetSelectedSongIndex() const
{
    return GameSelectedIndex;
}

bool GameScene::IsPaused()
{
    return SceneIsPaused;
}

bool GameScene::IsCountingDown()
{
    return SceneIsCountingDown;
}
}    // namespace Scene
