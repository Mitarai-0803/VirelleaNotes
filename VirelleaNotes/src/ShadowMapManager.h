#pragma once
//============================================================================
//! @file   ShadowMapManager.h
//! @brief  DxLibのシャドウマップ機能を一元管理するクラスの宣言
//! @details RenderTargetManager / ShaderManager と同様にシングルトンで管理する。
//!          MakeShadowMap() で作成したシャドウマップハンドルを1つだけ保持し、
//!          下記のDxLib標準の手順をラップする。
//!            1. Init()                      … MakeShadowMap()
//!            2. SetLightDirection() / SetDrawArea() … 描画前に一度設定
//!            3. BeginRecord() 〜 EndRecord() … ShadowMap_DrawSetup() 〜 ShadowMap_DrawEnd()
//!               の間に、影を落としたいオブジェクトを通常通り描画する
//!            4. BeginUse() 〜 EndUse()       … SetUseShadowMap() のON/OFF
//! @author レオ
//============================================================================
#include <DxLib.h>

namespace Graphics {

//-----------------------------------------------------------
//! @class ShadowMapManager
//! @brief シャドウマップを名前ではなく1つだけ管理するマネージャー（シングルトン）
//-----------------------------------------------------------
class ShadowMapManager
{
public:
    //! @return シングルトンインスタンス
    static ShadowMapManager& GetInstance();

    //-----------------------------------------------------------
    //! @brief シャドウマップを作成する
    //! @details 既に作成済みの場合は新規作成せず true を返す（二重作成防止）。
    //! @param sizeX シャドウマップの横解像度（2のn乗である必要がある）
    //! @param sizeY シャドウマップの縦解像度（2のn乗である必要がある）
    //! @return true 成功（既に作成済みの場合も true） / false 作成失敗
    //-----------------------------------------------------------
    bool Init(int sizeX, int sizeY);

    //-----------------------------------------------------------
    //! @brief シャドウマップへの描画で想定するライトの向きを設定する
    //! @details BeginRecord() を呼ぶ前に設定しておくこと。
    //!          見た目のライト（ChangeLightTypeDir 等）と同じ向きを渡すこと。
    //! @param direction ライトの向き
    //-----------------------------------------------------------
    void SetLightDirection(const VECTOR& direction);

    //-----------------------------------------------------------
    //! @brief シャドウマップに描画する3D空間の範囲を設定する
    //! @details BeginRecord() を呼ぶ前に設定しておくこと。
    //!          範囲が広いほど影の解像度（粗さ）が低下するため、
    //!          必要最小限の範囲にすること。
    //! @param minPosition 範囲の最小座標
    //! @param maxPosition 範囲の最大座標
    //-----------------------------------------------------------
    void SetDrawArea(const VECTOR& minPosition, const VECTOR& maxPosition);

    //-----------------------------------------------------------
    //! @brief シャドウマップへの深度描画を開始する
    //! @details 以降 EndRecord() を呼ぶまで、描画先は自動的にシャドウマップになる。
    //-----------------------------------------------------------
    void BeginRecord();

    //! @brief BeginRecord() で開始したシャドウマップへの描画を終了する
    void EndRecord();

    //-----------------------------------------------------------
    //! @brief 以降の3D描画でこのシャドウマップを使用する（影を受け取る）よう設定する
    //! @param slot 使用するスロット番号（0〜2。同時に3つまで異なるシャドウマップを使用できる）
    //-----------------------------------------------------------
    void BeginUse(int slot = 0);

    //-----------------------------------------------------------
    //! @brief 指定スロットのシャドウマップ利用設定を解除する
    //! @param slot 解除するスロット番号（0〜2）
    //-----------------------------------------------------------
    void EndUse(int slot = 0);

    //! @return true なら Init() 済み（シャドウマップが有効）
    bool IsValid() const { return m_handle != -1; }

    //-----------------------------------------------------------
    //! @return true なら BeginRecord() ～ EndRecord() の間（深度記録中）
    //! @details 記録中は独自シェーダー（トゥーン等）を使わず、通常描画にするための判定に使う
    //-----------------------------------------------------------
    bool IsRecording() const { return m_recording; }

    //! @return シャドウマップのハンドル（未作成時は -1）
    int GetHandle() const { return m_handle; }

    //-----------------------------------------------------------
    //! @brief シャドウマップを削除する
    //! @details 未作成、または既に削除済みの場合は何もしない（多重削除防止）。
    //-----------------------------------------------------------
    void End();

private:
    ShadowMapManager()  = default;
    ~ShadowMapManager() = default;

    ShadowMapManager(const ShadowMapManager&)            = delete;
    ShadowMapManager& operator=(const ShadowMapManager&) = delete;

    int  m_handle    = -1;       //!< MakeShadowMap() で作成したハンドル（未作成時は -1）
    bool m_recording = false;    //!< 深度記録中なら true
};

}    // namespace Graphics
