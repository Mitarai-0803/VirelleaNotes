#include "DrawFormatStringObject.h"
#include <regex>
#include <DxLib.h>

namespace UI {
//-----------------------------------------------------------
//! @brief 初期化処理
//-----------------------------------------------------------
void DrawFormatStringObject::OnInit()
{
    m_format    = "";                //!< フォーマット文字列
    m_formatted = "";                //!< 展開された文字列
    m_params.clear();                //!< パラメータ一覧を初期化
    m_pos         = {0.0f, 0.0f};    //!< 描画座標
    m_color       = 0xFFFFFF;        //!< 色（白）
    m_font_handle = 0;               //!< フォントハンドル（0 = デフォルト）
}

//-----------------------------------------------------------
//! @brief 書式文字列を設定
//-----------------------------------------------------------
void DrawFormatStringObject::SetFormat(const std::string& fmt)
{
    m_format = fmt;    //!< フォーマットを保存
}

//-----------------------------------------------------------
//! @brief 整数パラメータを設定
//-----------------------------------------------------------
void DrawFormatStringObject::SetParam(const std::string& OptionManager, int v)
{
    m_params[OptionManager] = v;    //!< 整数を登録
}

//-----------------------------------------------------------
//! @brief 浮動小数点パラメータを設定
//-----------------------------------------------------------
void DrawFormatStringObject::SetParam(const std::string& OptionManager, float v)
{
    m_params[OptionManager] = v;    //!< float を登録
}

//-----------------------------------------------------------
//! @brief 文字列パラメータを設定
//-----------------------------------------------------------
void DrawFormatStringObject::SetParam(const std::string& OptionManager, const std::string& v)
{
    m_params[OptionManager] = v;    //!< string を登録
}

//-----------------------------------------------------------
//! @brief パラメータを使ってフォーマット文字列を展開
//-----------------------------------------------------------
void DrawFormatStringObject::ApplyFormat()
{
    std::string result = m_format;    //!< 展開対象文字列

    for(const auto& [key, value] : m_params) {
        std::string search_str = "{" + key;    // {key または {key:...
        size_t      pos        = 0;

        while((pos = result.find(search_str, pos)) != std::string::npos) {
            size_t end_brace = result.find('}', pos);
            if(end_brace == std::string::npos)
                break;

            std::string inside = result.substr(pos + 1, end_brace - pos - 1);    // key:format
            std::string fmt    = "%s";                                           // デフォルト書式

            size_t colon = inside.find(':');
            if(colon != std::string::npos) {
                fmt = inside.substr(colon + 1);    // 書式部分
            }

            char buf[128] = {};

            // 型ごとに整形
            if(std::holds_alternative<int>(value))
                snprintf(buf, sizeof(buf), fmt.c_str(), std::get<int>(value));
            else if(std::holds_alternative<float>(value))
                snprintf(buf, sizeof(buf), fmt.c_str(), std::get<float>(value));
            else {
                // 文字列はそのまま使用
                const std::string& utf8 = std::get<std::string>(value);
                snprintf(buf, sizeof(buf), fmt.c_str(), utf8.c_str());
            }

            result.replace(pos, end_brace - pos + 1, buf);
            pos += strlen(buf);    // 次の探索位置を更新
        }
    }

    m_formatted = result;    //!< 展開後の文字列を保存
}

//-----------------------------------------------------------
//! @brief 描画処理
//-----------------------------------------------------------
void DrawFormatStringObject::OnDraw()
{
    if(m_formatted.empty())
        return;    //!< 文字列が空なら描画しない

    if(m_font_handle == 0)
        DrawStringF(m_pos.x, m_pos.y, m_formatted.c_str(), m_color);
    else {
        // フォントハンドルを使う場合直接渡す
        DrawStringFToHandle(m_pos.x, m_pos.y, m_formatted.c_str(), m_color, m_font_handle);
    }
}

//-----------------------------------------------------------
//! @brief 座標設定
//-----------------------------------------------------------
void DrawFormatStringObject::SetPos(float x, float y)
{
    m_pos.x = x;
    m_pos.y = y;
}

//-----------------------------------------------------------
//! @brief 色設定
//-----------------------------------------------------------
void DrawFormatStringObject::SetColor(unsigned int color)
{
    m_color = color;
}

//-----------------------------------------------------------
//! @brief フォント設定
//-----------------------------------------------------------
void DrawFormatStringObject::SetFontHandle(int h)
{
    m_font_handle = h;
}
}    // namespace UI
