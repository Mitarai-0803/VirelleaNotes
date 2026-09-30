//----------------------------------------------------------------------------
//! @file   CSVLoadObject.cpp
//! @brief  CSV 読み込みと音源管理の実装
//! @details CSV ファイルからノーツ情報を読み込み、解析用ソフトサウンドおよび再生用サウンドを
//         読み込んで管理する処理を提供します。曲データの列挙やハンドル提供を行います。
//----------------------------------------------------------------------------
#include "CSVLoadObject.h"    // クラス宣言のヘッダをインクルード
#include <fstream>            // ファイル読み込み用
#include <sstream>            // 文字列分割用
#include <iostream>           // 標準出力用
#include <DxLib.h>            // DxLibの関数（LoadSoundMemなど）を使うため
#include <filesystem>         // ディレクトリ走査用
#include <cstdio>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;    // filesystem 名前空間短縮

//============================================================================
//! @class  CSVLoadObject
//! @details CSV 読み込みと音源ハンドル管理を実装するクラスの具体実装。
//         ディレクトリ走査、CSV パース、音源ロード／解放を担当します。
//============================================================================

namespace Object {

// 譜面と音源のペアを保持するリスト
static std::vector<DataSet> data_sets;
static int                  selected_index = 0;    // デフォルト選択インデックス

//--------------------------------------------------------
//! @brief 初期化処理
//--------------------------------------------------------
void CSVLoadObject::OnInit()
{
    // データディレクトリを走査してデータセットを構築
    ScanDataDirectory("data");

    m_current_index = 0;                 // インデックスは 0 から開始
    m_current_index = selected_index;    // グローバル選択値を反映

    if(data_sets.empty())
        return;    // データがなければ終了

    auto& data = data_sets[m_current_index];

    // CSV を読み込み、必要なハンドルを初期化
    if(LoadCsv(data.csv_path)) {
        // SoftSound をロードしてから SoundHandle を作成（RAII）
        m_soft_handle  = SoftSoundHandle::Load(data.music_path);
        m_sound_handle = SoundHandle::FromSoft(m_soft_handle.Get());
    }
}

//--------------------------------------------------------
//! @brief 描画処理
//--------------------------------------------------------
void CSVLoadObject::OnDraw()
{
    const char* str = nullptr;

    if(data_sets.empty()) {
        str = "読み込めるデータがありませんでした";    // データペアなし
    }
    else if(m_notes.empty()) {
        str = "CSVデータが読み込めません";    // CSV 読み込み失敗
    }

    if(str) {
        int font_handle = Font::GetFont(Font::FontIndex::FONT_SIZE_20_INDEX);
        int str_w       = GetDrawStringWidthToHandle(str, -1, font_handle);
        int str_h       = GetFontSizeToHandle(font_handle);

        // 画面中央に描画
        DrawStringToHandle((SCREEN_W * HALF) - (str_w * HALF), (SCREEN_H * HALF) - (str_h * HALF), str, COLOR_WHITE, font_handle);
    }
}

//--------------------------------------------------------
//! @brief 終了処理
//--------------------------------------------------------
void CSVLoadObject::OnEnd()
{
    m_notes.clear();    // ノーツデータをクリア

    // により自動解放されるように置き換えられているため、明示的な削除は不要
    m_soft_handle  = SoftSoundHandle();
    m_sound_handle = SoundHandle();
}

//--------------------------------------------------------
//! @brief CSV ファイル読み込み
//--------------------------------------------------------
bool CSVLoadObject::LoadCsv(const std::string& file_path)
{
    m_notes.clear();    // 初期化

    FILE* fp = nullptr;
    if(fopen_s(&fp, file_path.c_str(), "r") != 0 || fp == nullptr) {
        return false;
    }

    // 前後空白削除ラムダ
    auto trim = [](std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    };

    char buffer[1024];
    while(true) {
        memset(buffer, 0, sizeof(buffer));

        if(!fgets(buffer, sizeof(buffer), fp))
            break;

        std::string line(buffer);

        // 空行スキップ
        if(line.find_first_not_of("\t\r\n") == std::string::npos)
            continue;

        //--------------------------------------------
        // 文字分割（簡易パース）
        //--------------------------------------------
        size_t p1 = line.find(',');
        if(p1 == std::string::npos)
            continue;

        size_t p2 = line.find(',', p1 + 1);
        if(p2 == std::string::npos)
            continue;

        std::string time_str = line.substr(0, p1);
        std::string line_str = line.substr(p1 + 1, p2 - (p1 + 1));
        std::string type_str = line.substr(p2 + 1);

        trim(time_str);
        trim(line_str);
        trim(type_str);

        //--------------------------------------------
        // 数値変換
        //--------------------------------------------
        try {
            NoteData n;
            n.time_ms = std::stoi(time_str);
            n.line    = std::stoi(line_str);

            if(type_str == "0")
                n.type = NOTE_TYPE::NOTE_TYPE_TAP;
            else if(type_str == "1")
                n.type = NOTE_TYPE::NOTE_TYPE_HOLD_START;
            else if(type_str == "2")
                n.type = NOTE_TYPE::NOTE_TYPE_HOLD_END;
            else
                n.type = NOTE_TYPE::NOTE_TYPE_SYNC;

            n.spawned = false;
            m_notes.push_back(n);
        }
        catch(...) {
            continue;    // 無効行は無視
        }
    }

    fclose(fp);
    return true;
}

//--------------------------------------------------------
//! @brief ノーツデータ取得
//--------------------------------------------------------
const std::vector<NoteData>& CSVLoadObject::GetNotesData() const
{
    return m_notes;    // 現在保持しているノーツデータ
}

//--------------------------------------------------------
//! @brief 譜面切り替え
//--------------------------------------------------------
void CSVLoadObject::SetIndex(int index)
{
    if(index >= 0 && index < data_sets.size()) {
        m_current_index = index;    // インデックス更新
        OnInit();                   // 再ロード
    }
}

//--------------------------------------------------------
//! @brief 読み込まれた音源ハンドル取得
//--------------------------------------------------------
int CSVLoadObject::GetHandle()
{
    return m_sound_handle.Get();
}

//--------------------------------------------------------
//! @brief 読み込まれた解析音源ハンドル取得
//--------------------------------------------------------
int CSVLoadObject::GetSoftHandle()
{
    return m_soft_handle.Get();
}

//--------------------------------------------------------
//! @brief data/ 以下のサブフォルダを走査してデータセット登録
//--------------------------------------------------------
void CSVLoadObject::ScanDataDirectory(const std::string& root_path)
{
    if(!fs::exists(root_path))
        return;    // フォルダ存在確認

    data_sets.clear();

    for(const auto& dir_entry : fs::directory_iterator(root_path)) {
        if(!dir_entry.is_directory())
            continue;    // フォルダ以外はスキップ

        std::string sub_path  = NormalizePath(dir_entry.path().string());    // パス正規化
        std::string csv_path  = sub_path + "/csv/data.csv";                  // CSV パス
        std::string music_dir = sub_path + "/music";                         // 音源フォルダ

        std::string csv_file, music_file;

        if(fs::exists(csv_path))
            csv_file = csv_path;

        if(fs::exists(music_dir)) {
            for(const auto& entry : fs::directory_iterator(music_dir)) {
                std::string filename = NormalizePath(entry.path().filename().string());
                std::string ext      = entry.path().extension().string();

                std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                if(filename.rfind("music.", 0) == 0 && (ext == ".mp3" || ext == ".wav" || ext == ".ogg")) {
                    music_file = NormalizePath(entry.path().string());
                    break;
                }
            }
        }

        if(!csv_file.empty() && !music_file.empty()) {
            data_sets.push_back({csv_file, music_file});
        }
    }
}

//--------------------------------------------------------
//! @brief CSV 選択のグローバルインデックス設定
//--------------------------------------------------------
void CSVLoadObject::SetSelectedIndex(int idx)
{
    if(idx < 0)
        idx = 0;
    if(idx >= static_cast<int>(data_sets.size()))
        idx = static_cast<int>(data_sets.size()) - 1;
    selected_index = idx;
}

//--------------------------------------------------------
//! @brief CSV 選択のグローバルインデックス取得
//--------------------------------------------------------
int CSVLoadObject::GetSelectedIndex()
{
    return selected_index;
}

//--------------------------------------------------------
//! @brief 利用可能データセット一覧取得
//--------------------------------------------------------
const std::vector<DataSet>& CSVLoadObject::GetAvailableDataSets()
{
    return data_sets;
}

//--------------------------------------------------------
//! @brief data セットの再走査
//--------------------------------------------------------
void CSVLoadObject::RefreshAvailableDataSets(const std::string& root_path)
{
    CSVLoadObject tmp;
    tmp.ScanDataDirectory(root_path);
}
}    // namespace Object
