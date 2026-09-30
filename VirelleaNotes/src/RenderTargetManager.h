#pragma once
//============================================================================
//! @file   RenderTargetManager.h
//! @brief  レンダーターゲット（バックバッファ）管理クラスの宣言
//! @details 複数のレンダーターゲットを作成・管理し、描画ターゲットの切替や描画を簡易に行えるユーティリティ。
//!          ShaderManager / PhysicsManager と同様にシングルトンで一元管理する。
//!          アプリ起動後、最初に使われたタイミングで1回だけ実体が作られ、
//!          シーンが切り替わっても同じインスタンス・同じレンダーターゲットを使い回す。
//! @author レオ
//============================================================================
#include <string>
#include <unordered_map>
#include <memory>

namespace Graphics
{

class RenderTargetManager
{
public:
    //! @return シングルトンインスタンス
    static RenderTargetManager& GetInstance();

    //-----------------------------------------------------------
    //! @param name 管理名
    //! @param width 幅
    //! @param height 高さ
    //! @param useAlpha アルファを利用するか
    //! @return 作成したグラフィックハンドル（失敗時は -1）
    //! @details 既に同名で作成済みの場合は新規作成せずそのハンドルを返す
    //!          （シーン切り替えのたびに呼んでも MakeScreen は1回しか実行されない）。
    //-----------------------------------------------------------
    int CreateRenderTarget(const std::string& name, int width, int height, bool useAlpha = true);

    //-----------------------------------------------------------
    //! @brief 指定名のレンダーターゲットをアクティブにして描画先を切替える
    //! @param name 管理名
    //! @return true 成功
    //-----------------------------------------------------------
    bool SetActive(const std::string& name);

    //! 描画先をスクリーン（デフォルト）に戻す
    void ResetToScreen();

    //-----------------------------------------------------------
    //! @param name 管理名
    //! @param x 描画 X
    //! @param y 描画 Y
    //-----------------------------------------------------------
    void Present(const std::string& name, int x = 0, int y = 0);

    //! @param name 管理名
    void Release(const std::string& name);

    //! 全てのレンダーターゲットを解放する
    void ReleaseAll();

    //-----------------------------------------------------------
    //! @param name 管理名
    //! @return ハンドル（存在しない場合は -1）
    //-----------------------------------------------------------
    int GetHandle(const std::string& name) const;

private:
    RenderTargetManager();
    ~RenderTargetManager();

    RenderTargetManager(const RenderTargetManager&)            = delete;
    RenderTargetManager& operator=(const RenderTargetManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> m_impl;    //!< 実装のポインタ（Pimpl イディオム）
};

}    // namespace Graphics
