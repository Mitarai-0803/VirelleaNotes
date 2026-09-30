#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include "ObjectBase.h"

namespace UI {
//------------------------------------------------------------
//! @class DrawFormatStringObject
//! @brief 名前付きプレースホルダによるフォーマット付き文字描画クラス
//------------------------------------------------------------
class DrawFormatStringObject : public Object::ObjectBase
{
public:
    //--------------------------------------------------------
    //! @brief 初期化処理
    //--------------------------------------------------------
    void OnInit() override;

    //--------------------------------------------------------
    //! @brief 描画処理
    //--------------------------------------------------------
    void OnDraw() override;

    //--------------------------------------------------------
    //! @brief 書式文字列を設定
    //! @param fmt フォーマット文字列（例： "Score: {score}"）
    //--------------------------------------------------------
    void SetFormat(const std::string& fmt);

    //--------------------------------------------------------
    //! @brief フォーマット用パラメータを設定
    //! @param OptionManager パラメータの名前
    //! @param v 設定する整数値
    //--------------------------------------------------------
    void SetParam(const std::string& OptionManager, int v);

    //--------------------------------------------------------
    //! @brief フォーマット用パラメータを設定
    //! @param OptionManager パラメータの名前
    //! @param v 設定する浮動小数値
    //--------------------------------------------------------
    void SetParam(const std::string& OptionManager, float v);

    //--------------------------------------------------------
    //! @brief フォーマット用パラメータを設定
    //! @param OptionManager パラメータの名前
    //! @param v 設定する文字列
    //--------------------------------------------------------
    void SetParam(const std::string& OptionManager, const std::string& v);

    //--------------------------------------------------------
    //! @brief 現在のフォーマット文字列＋パラメータを展開して完成文字列を生成
    //--------------------------------------------------------
    void ApplyFormat();

    //--------------------------------------------------------
    //! @brief 座標を設定
    //--------------------------------------------------------
    void SetPos(float x, float y);

    //--------------------------------------------------------
    //! @brief 色を設定
    //--------------------------------------------------------
    void SetColor(unsigned int color);

    //--------------------------------------------------------
    //! @brief フォントハンドルを設定
    //--------------------------------------------------------
    void SetFontHandle(int h);

private:
    std::string m_format;       //!< 元のフォーマット文字列
    std::string m_formatted;    //!< プレースホルダ展開後の文字列

    // 名前と値のペア
    std::unordered_map<std::string, std::variant<int, float, std::string>> m_params;    //!< パラメータ一覧

    struct
    {
        float x, y;
    } m_pos;                       //!< 描画座標
    unsigned int m_color;          //!< 色
    int          m_font_handle;    //!< フォントハンドル（0 = デフォルト）
};
}    // namespace UI
