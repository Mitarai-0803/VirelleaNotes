//----------------------------------------------------------------------------
//! @file    SongSelectScene.cpp
//! @brief   曲選択シーンの実装
//! @detail 曲一覧の表示と上下キーでの選択、Enterキーで選択確定の処理を提供します。
//----------------------------------------------------------------------------
#include "SongSelectScene.h"
#include "InputManager.h"
#include "Font.h"
#include "CSVLoadObject.h"
#include "SceneManager.h"
#include "UIConstants.h"
#include "BgmComponent.h"
#include "ResourceHandles.h"
#include "ConstantsRhythm.h"
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <DxLib.h>
#include <vector>
#include <string>
#include <cctype>
#include <Windows.h>

namespace Scene {
// ファイルローカル: 遊び方表示フラグと画像ハンドル
static bool ShowHowto        = false;
static int  HowtoImageHandle = -1;

//----------------------------------------------------------------------------
//! data.txt を解析してタイトルと BPM を取得する
//! @param [in] path data.txt のパス
//! @param [out] outTitle 取得したタイトル（未取得時は空文字）
//! @param [out] outBpm 取得した BPM（未取得時は -1）
//----------------------------------------------------------------------------
static void ReadDataFileMetadata(const std::string& path, std::string& outTitle, int& outBpm)
{
    outTitle.clear();
    outBpm = -1;

    std::ifstream ifs(path);
    if(!ifs)
        return;

    auto trim = [](std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    };

    std::string line;
    while(std::getline(ifs, line)) {
        // UTF-8 BOM が先頭に含まれる場合は除去
        if(!line.empty() && static_cast<unsigned char>(line[0]) == 0xEF) {
            // BOM の 3 バイトをチェック
            if(line.size() >= 3 && static_cast<unsigned char>(line[0]) == 0xEF && static_cast<unsigned char>(line[1]) == 0xBB &&
               static_cast<unsigned char>(line[2]) == 0xBF) {
                line.erase(0, 3);
            }
        }

        // コメントや空行はスキップ
        std::string tmp = line;
        trim(tmp);
        if(tmp.empty())
            continue;
        if(tmp.size() >= 1 && tmp[0] == '#')
            continue;

        // キーと値を分割 (':' または '=')
        size_t pos = tmp.find(':');

        // ':' が見つからない場合は '=' も試す
        if(pos == std::string::npos)
            pos = tmp.find('=');

        // キーと値が分割できる場合
        if(pos != std::string::npos) {
            std::string key = tmp.substr(0, pos);
            std::string val = tmp.substr(pos + 1);
            trim(key);
            trim(val);

            // 小文字化
            std::string key_l = key;
            std::transform(key_l.begin(), key_l.end(), key_l.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            if(key_l.find("title") != std::string::npos || key_l.find("name") != std::string::npos || key_l.find("song") != std::string::npos) {
                outTitle = val;
                continue;
            }

            if(key_l.find("bpm") != std::string::npos) {
                try {
                    outBpm = std::stoi(val);
                }
                catch(...) {
                    outBpm = -1;
                }
                continue;
            }
        }
        else {
            // キーがない行は、先頭の非コメント行をタイトル候補として扱う（タイトル未設定時）
            if(outTitle.empty()) {
                trim(tmp);
                outTitle = tmp;
            }
        }
    }
}

//----------------------------------------------------------------------------
//! UTF-8 文字列を Shift-JIS に変換する
//----------------------------------------------------------------------------
static std::string ConvertUtf8ToShiftJis(const std::string& utf8)
{
    if(utf8.empty())
        return std::string();

    // UTF-8 -> ワイド文字
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    if(wlen == 0)
        return utf8;

    std::vector<wchar_t> wbuf;
    wbuf.resize(static_cast<std::size_t>(wlen));
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, wbuf.data(), wlen);

    // ワイド文字 -> Shift-JIS (CP932)
    int slen = WideCharToMultiByte(932, 0, wbuf.data(), -1, nullptr, 0, nullptr, nullptr);
    if(slen == 0)
        return utf8;

    std::string sbuf;
    sbuf.resize(static_cast<std::size_t>(slen));
    WideCharToMultiByte(932, 0, wbuf.data(), -1, &sbuf[0], slen, nullptr, nullptr);

    if(!sbuf.empty() && sbuf.back() == '\0')
        sbuf.pop_back();
    return sbuf;
}

//----------------------------------------------------------------------------
//! 指定したデータセットの music フォルダ内から music.* を探索して返す
//----------------------------------------------------------------------------
static std::string FindMusicFileInDataset(const std::filesystem::path& dataset_dir)
{
    namespace fs       = std::filesystem;
    fs::path music_dir = dataset_dir / "music";
    if(!fs::exists(music_dir) || !fs::is_directory(music_dir))
        return std::string();

    for(const auto& entry : fs::directory_iterator(music_dir)) {
        if(!entry.is_regular_file())
            continue;
        std::string filename = entry.path().filename().string();
        std::string lower    = filename;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        std::string ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if(lower.rfind("music.", 0) == 0 && (ext == ".mp3" || ext == ".wav" || ext == ".ogg"))
            return entry.path().string();
    }
    return std::string();
}

// ファイルローカル: Hキーで遊び方を開閉
static void HandleHowToToggle()
{
    if(InputManager::PushHitKey(KEY_INPUT_H)) {
        ShowHowto = !ShowHowto;
        // 表示トグルのみ。画像の読み込みは初期化で行う
    }
}

// ファイルローカル: 遊び方オーバーレイ描画
static void DrawHowToOverlay()
{
    if(!ShowHowto)
        return;
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
    DrawBox(80, 60, SCREEN_W - 80, SCREEN_H - 60, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    if(HowtoImageHandle != -1) {
        DrawRotaGraphF(HALF_SCREEN_W, HALF_SCREEN_H - 30, 1.0f, 0.0f, HowtoImageHandle, TRUE);
    }
}

//------------------------------------------------------------
//! シーン初期化
//------------------------------------------------------------
void SongSelectScene::SceneInit()
{
    m_song_list.clear();
    m_song_paths.clear();
    m_song_titles.clear();
    m_song_bpm.clear();
    m_song_icons.clear();

    Object::CSVLoadObject::RefreshAvailableDataSets("data");
    namespace fs        = std::filesystem;
    const fs::path root = "data";
    if(fs::exists(root)) {
        for(const auto& entry : fs::recursive_directory_iterator(root)) {
            if(!entry.is_regular_file())
                continue;
            if(entry.path().filename() == "data.txt") {
                const fs::path data_file_path = entry.path();
                const fs::path dataset_dir    = data_file_path.parent_path();
                m_song_list.push_back(dataset_dir.filename().string());
                m_song_paths.push_back(data_file_path.string());
                std::string title;
                int         bpm = -1;
                ReadDataFileMetadata(data_file_path.string(), title, bpm);
                if(title.empty())
                    title = dataset_dir.filename().string();
                m_song_titles.push_back(title);
                m_song_bpm.push_back(bpm);
                m_song_icons.push_back(ImageHandle());
                const auto icon_path = dataset_dir / "icon.png";
                if(fs::exists(icon_path)) {
                    int h = LoadGraph(icon_path.string().c_str());
                    if(h != -1)
                        m_song_icons.back() = ImageHandle(h);
                }
            }
        }
    }

    // 遊び方画像を初期化時に読み込む（描画は DrawHowToOverlay、終了で解放）
    if(HowtoImageHandle == -1) {
        HowtoImageHandle = LoadGraph(HOWTO_IMAGE_PATH);
    }

    m_selected_index = 0;
    m_is_selected    = false;
    m_song_best_scores.clear();
    m_song_best_scores.resize(m_song_list.size(), 0);
    for(std::size_t i = 0; i < m_song_list.size(); ++i) {
        const std::string scoreFile = std::string("data/") + m_song_list[i] + "/bestscore.txt";
        std::ifstream     ifs(scoreFile);
        if(!ifs)
            continue;
        int s = 0;
        ifs >> s;
        m_song_best_scores[i] = s;
    }

    // 背景画像をキャッシュ
    m_back_image = ImageHandle::Load(SONG_SELECT_BG_IMAGE_PATH);

    // フォントハンドルをキャッシュ
    m_font50 = GetFont(Font::FONT_SIZE_50_INDEX);
    m_font30 = GetFont(Font::FONT_SIZE_30_INDEX);
    m_font25 = GetFont(Font::FONT_SIZE_20_INDEX);

    // 曲プレビュー用に全曲の音源を事前読み込みしてキャッシュ
    m_preloaded_sound_handles.clear();
    const auto& datasets = Object::CSVLoadObject::GetAvailableDataSets();
    m_preloaded_sound_handles.reserve(datasets.size());
    for(const auto& ds : datasets) {
        if(ds.music_path.empty()) {
            m_preloaded_sound_handles.push_back(SoundHandle());
            continue;
        }
        SoundHandle sh = SoundHandle::Load(ds.music_path);
        m_preloaded_sound_handles.push_back(std::move(sh));
    }

    // 初期選択曲のプレビュー再生
    m_current_preview_index = -1;
    if(!m_preloaded_sound_handles.empty() && m_selected_index >= 0 && m_selected_index < static_cast<int>(m_preloaded_sound_handles.size())) {
        const int h = m_preloaded_sound_handles[m_selected_index].Get();
        if(h != -1) {
            SetSoundCurrentTime(0, h);
            ChangeVolumeSoundMem(m_preview_volume, h);
            PlaySoundMem(h, DX_PLAYTYPE_BACK, true);
            m_current_preview_index = m_selected_index;
        }
    }
}

//------------------------------------------------------------
//! シーン更新
//------------------------------------------------------------
void SongSelectScene::SceneUpdate()
{
    // 遊び方の表示トグル処理
    HandleHowToToggle();

    // 曲選択の入力処理
    if(InputManager::PushHitKey(KEY_INPUT_UP)) {
        if(!m_song_list.empty()) {
            --m_selected_index;
            if(m_selected_index < 0)
                m_selected_index = static_cast<int>(m_song_list.size()) - 1;
            if(m_current_preview_index != m_selected_index) {
                if(m_current_preview_index >= 0 && m_current_preview_index < static_cast<int>(m_preloaded_sound_handles.size())) {
                    int ph = m_preloaded_sound_handles[m_current_preview_index].Get();
                    if(ph != -1)
                        StopSoundMem(ph);
                }
                if(m_selected_index >= 0 && m_selected_index < static_cast<int>(m_preloaded_sound_handles.size())) {
                    int nh = m_preloaded_sound_handles[m_selected_index].Get();
                    if(nh != -1) {
                        SetSoundCurrentTime(0, nh);
                        ChangeVolumeSoundMem(m_preview_volume, nh);
                        PlaySoundMem(nh, DX_PLAYTYPE_BACK, true);
                        m_current_preview_index = m_selected_index;
                    }
                    else {
                        m_current_preview_index = -1;
                    }
                }
            }
        }
    }

    // 上下キーで選択が変わったらプレビュー再生も切り替える
    if(InputManager::PushHitKey(KEY_INPUT_DOWN)) {
        if(!m_song_list.empty()) {
            ++m_selected_index;
            if(m_selected_index >= static_cast<int>(m_song_list.size()))
                m_selected_index = 0;
            if(m_current_preview_index != m_selected_index) {
                if(m_current_preview_index >= 0 && m_current_preview_index < static_cast<int>(m_preloaded_sound_handles.size())) {
                    int ph = m_preloaded_sound_handles[m_current_preview_index].Get();
                    if(ph != -1)
                        StopSoundMem(ph);
                }
                if(m_selected_index >= 0 && m_selected_index < static_cast<int>(m_preloaded_sound_handles.size())) {
                    int nh = m_preloaded_sound_handles[m_selected_index].Get();
                    if(nh != -1) {
                        SetSoundCurrentTime(0, nh);
                        ChangeVolumeSoundMem(m_preview_volume, nh);
                        PlaySoundMem(nh, DX_PLAYTYPE_BACK, FALSE);
                        m_current_preview_index = m_selected_index;
                    }
                    else {
                        m_current_preview_index = -1;
                    }
                }
            }
        }
    }

    // 左右キーでプレビュー音量調整
    if(InputManager::PushHitKey(KEY_INPUT_LEFT)) {
        m_preview_volume -= 8;
        if(m_preview_volume < 0)
            m_preview_volume = 0;
        if(m_current_preview_index >= 0 && m_current_preview_index < static_cast<int>(m_preloaded_sound_handles.size())) {
            int h = m_preloaded_sound_handles[m_current_preview_index].Get();
            if(h != -1)
                ChangeVolumeSoundMem(m_preview_volume, h);
        }
        m_preview_volume_display_timer = FRAME_RATE * 2;
    }
    if(InputManager::PushHitKey(KEY_INPUT_RIGHT)) {
        m_preview_volume += 8;
        if(m_preview_volume > 255)
            m_preview_volume = 255;
        if(m_current_preview_index >= 0 && m_current_preview_index < static_cast<int>(m_preloaded_sound_handles.size())) {
            int h = m_preloaded_sound_handles[m_current_preview_index].Get();
            if(h != -1)
                ChangeVolumeSoundMem(m_preview_volume, h);
        }
        m_preview_volume_display_timer = FRAME_RATE * 2;
    }

    // プレビュー音量表示タイマー更新
    if(InputManager::PushHitKey(KEY_INPUT_RETURN)) {
        if(!m_song_list.empty()) {
            m_is_selected                                = true;
            auto                        datasets         = Object::CSVLoadObject::GetAvailableDataSets();
            const std::string           selectedDataPath = m_song_paths[m_selected_index];
            const std::filesystem::path selectedDir      = std::filesystem::path(selectedDataPath).parent_path();
            for(size_t i = 0; i < datasets.size(); ++i) {
                const std::string& csvpath = datasets[i].csv_path;
                if(csvpath.find(selectedDir.string()) != std::string::npos) {
                    Object::CSVLoadObject::SetSelectedIndex(static_cast<int>(i));
                    break;
                }
            }
            ChangeScene(Rhythm::SceneName::GAME);
        }
    }

    // タイトルに戻る (T)
    if(InputManager::PushHitKey(KEY_INPUT_T)) {
        // プレビュー停止
        if(m_current_preview_index >= 0 && m_current_preview_index < static_cast<int>(m_preloaded_sound_handles.size())) {
            int h = m_preloaded_sound_handles[m_current_preview_index].Get();
            if(h != -1)
                StopSoundMem(h);
        }
        // BGM コンポーネント停止 (もしあれば)
        if(auto obj = GetSceneObject<Object::ObjectBase>("SongPreviewBgm")) {
            if(auto bgm_comp = obj->GetComponent<Audio::BgmComponent>()) {
                bgm_comp->Stop();
            }
        }
        ChangeScene(Rhythm::SceneName::TITLE);
        return;
    }
}

//------------------------------------------------------------
//! シーン描画
//------------------------------------------------------------
void SongSelectScene::SceneDraw()
{
    // 背景描画
    if(m_back_image)
        DrawGraph(0, 0, m_back_image.Get(), FALSE);
    DrawFormatStringToHandle(SONG_SELECT_HEADER_X, SONG_SELECT_HEADER_Y, GetColor(255, 255, 255), m_font50, "Song Select");

    // 曲リスト描画
    for(std::size_t i = 0; i < m_song_list.size(); ++i) {
        int y = SONG_SELECT_START_Y + static_cast<int>(i) * SONG_LIST_LINE_HEIGHT;
        if(static_cast<int>(i) == m_selected_index) {
            std::string sj = ConvertUtf8ToShiftJis(m_song_titles[i]);
            DrawFormatStringToHandle(SONG_SELECT_START_X - SELECT_ARROW_OFFSET, y, GetColor(255, 255, 0), m_font30, "> %s", sj.c_str());
        }
        else {
            std::string sj = ConvertUtf8ToShiftJis(m_song_titles[i]);
            DrawFormatStringToHandle(SONG_SELECT_START_X, y, GetColor(200, 200, 200), m_font30, "%s", sj.c_str());
        }
    }

    // 遊び方を表示する旨のヒント
    DrawFormatStringToHandle(HINT_X, SCREEN_H - HINT_MARGIN_BOTTOM, COLOR_WHITE, m_font25, "Hキー：遊び方 Tキー：タイトルに戻る");

    // 選択中の曲情報描画
    if(!m_song_list.empty() && m_selected_index >= 0 && m_selected_index < static_cast<int>(m_song_list.size())) {
        const int info_x    = SONG_SELECT_START_X + SONG_INFO_OFFSET_X;
        const int info_y    = SONG_SELECT_START_Y;
        const int icon_x    = SCREEN_W - SONG_ICON_SIZE - SONG_ICON_MARGIN_RIGHT;
        const int box_right = icon_x - SONG_INFO_BOX_MARGIN;
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
        DrawBox(info_x - 10, info_y - 10, box_right, info_y + SONG_ICON_SIZE + 10, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        std::string sj = ConvertUtf8ToShiftJis(m_song_titles[m_selected_index]);
        DrawFormatStringToHandle(info_x, info_y, COLOR_WHITE, m_font30, "%s", sj.c_str());
        int bpm = m_song_bpm[m_selected_index];
        if(bpm > 0)
            DrawFormatStringToHandle(info_x, info_y + SONG_INFO_BPM_OFFSET, GetColor(200, 200, 200), GetFont(Font::FONT_SIZE_20_INDEX), "BPM: %d", bpm);
        else
            DrawFormatStringToHandle(info_x, info_y + SONG_INFO_BPM_OFFSET, GetColor(200, 200, 200), GetFont(Font::FONT_SIZE_20_INDEX), "BPM: -");
        if(m_selected_index < static_cast<int>(m_song_best_scores.size())) {
            int best = m_song_best_scores[m_selected_index];
            DrawFormatStringToHandle(info_x, info_y + SONG_INFO_SCORE_OFFSET, COLOR_WHITE, GetFont(Font::FONT_SIZE_20_INDEX), "SCORE: %d", best);
        }
        if(m_preview_volume_display_timer > 0) {
            char buf[64];
            int  disp = (m_preview_volume * 100 + 127) / 255;
            if(disp > 100)
                disp = 100;
            snprintf(buf, sizeof(buf), "Volume: %d%%", disp);
            int font = m_font25;
            int w    = GetDrawStringWidthToHandle(buf, -1, font);
            int x    = SCREEN_W - w - VOLUME_MARGIN_RIGHT;
            int y    = SCREEN_H - GetFontSizeToHandle(font) - VOLUME_MARGIN_BOTTOM;
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
            DrawBox(x - VOLUME_BOX_PADDING,
                    y - VOLUME_BOX_PADDING,
                    x + w + VOLUME_BOX_PADDING,
                    y + GetFontSizeToHandle(font) + VOLUME_BOX_PADDING,
                    GetColor(0, 0, 0),
                    TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
            DrawFormatStringToHandle(x, y, GetColor(255, 255, 255), font, "%s", buf);
        }
        if(m_selected_index < static_cast<int>(m_song_icons.size())) {
            int icon = m_song_icons[m_selected_index].Get();
            if(icon >= 0) {
                int img_w = 0, img_h = 0;
                GetGraphSize(icon, &img_w, &img_h);
                if(img_w > 0 && img_h > 0) {
                    float scale_x = static_cast<float>(SONG_ICON_SIZE) / static_cast<float>(img_w);
                    float scale_y = static_cast<float>(SONG_ICON_SIZE) / static_cast<float>(img_h);
                    float scale   = (scale_x + scale_y) * 0.5f;
                    float cx      = static_cast<float>(icon_x) + SONG_ICON_SIZE * 0.5f;
                    float cy      = static_cast<float>(info_y) + SONG_ICON_SIZE * 0.5f;
                    DrawRotaGraphF(cx, cy, scale, 0.0f, icon, TRUE);
                }
            }
            else {
                DrawBox(icon_x, info_y, icon_x + SONG_ICON_SIZE, info_y + SONG_ICON_SIZE, COLOR_DARKGRAY, TRUE);
            }
        }
    }

    // 遊び方オーバーレイ描画
    DrawHowToOverlay();
}

//------------------------------------------------------------
//! シーン終了
//------------------------------------------------------------
void SongSelectScene::SceneEnd()
{
    // リソース解放
    m_song_icons.clear();
    m_song_list.clear();
    m_selected_index = 0;
    m_is_selected    = false;

    // プレビュー停止
    if(auto obj = GetSceneObject<Object::ObjectBase>("SongPreviewBgm")) {
        if(auto bgm_comp = obj->GetComponent<Audio::BgmComponent>()) {
            bgm_comp->Stop();
        }
    }
    for(auto& sh : m_preloaded_sound_handles) {
        int h = sh.Get();
        if(h != -1) {
            StopSoundMem(h);
        }
    }    // 解放は SoundHandle のデストラクタに任せる（二重解放防止）
    m_preloaded_sound_handles.clear();

    // 遊び方画像を解放
    if(HowtoImageHandle != -1) {
        DeleteGraph(HowtoImageHandle);
        HowtoImageHandle = -1;
        ShowHowto        = false;
    }
}

//------------------------------------------------------------
//! 選択した曲のインデックスを取得
//------------------------------------------------------------
int SongSelectScene::GetSelectedIndex() const
{
    return m_selected_index;
}

}    // namespace Scene
