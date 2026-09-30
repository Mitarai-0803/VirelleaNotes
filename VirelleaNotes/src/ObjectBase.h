#pragma once
//============================================================================
//! @file   ObjectBase.h
//! @brief  ゲームオブジェクト共通基底クラス
//! @details コンポーネント管理とライフサイクルのみを持つ純粋な基底
//!          座標は ObjectBase2D / ObjectBase3D で管理する
//! @author レオ
//============================================================================
#include "ComponentBase.h"
#include "Layer.h"
#include <vector>
#include <memory>
#include <string>

namespace Object {

class ObjectBase
{
public:
    //! @brief コンストラクタ
    ObjectBase() = default;

    //-----------------------------------------------------------
    //! @brief デストラクタ
    //! @details 基底クラスのポインタから削除した場合でも、
    //!          派生クラスのデストラクタが正しく呼ばれるようにする。
    //!          リソースの解放は End() で明示的に行うため、
    //!          デストラクタでは終了処理を行わない。
    //-----------------------------------------------------------
    virtual ~ObjectBase();

    //-----------------------------------------------------------
    //! @brief オブジェクト名の取得
    //! @return オブジェクト名
    //-----------------------------------------------------------
    std::string GetName() const;

    //! @brief オブジェクト名の設定
    void SetName(const std::string& n);

    //! @brief コンポーネント追加
    template <typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args)
    {
        auto comp = std::make_shared<T>(std::forward<Args>(args)...);
        comp->Init();    // 追加した時点で初期化する（OnInit() 内や実行中に追加したコンポーネントも初期化される）
        m_components.push_back(comp);
        return comp;
    }

    //-----------------------------------------------------------
    //! @brief コンポーネント取得
    //! @return 見つかれば shared_ptr<T>、なければ nullptr
    //-----------------------------------------------------------
    template <typename T>
    std::shared_ptr<T> GetComponent() const
    {
        for(auto& comp : m_components) {
            auto ptr = std::dynamic_pointer_cast<T>(comp);
            if(ptr)
                return ptr;
        }
        return nullptr;
    }

    //! @brief ライフサイクル呼び出し
    void Init();
    void Update();
    void DrawDebug();
    void Draw();
    void End();

    //=============================================================
    // Layer / Active / Visible
    //=============================================================

    //-----------------------------------------------------------
    //! @brief 所属レイヤーの取得
    //! @return 所属レイヤー
    //-----------------------------------------------------------
    Layer::LayerID GetLayer() const { return m_layer; }

    //-----------------------------------------------------------
    //! @brief 所属レイヤーの設定
    //! @details Renderer / Camera 側の LayerMask と組み合わせて、
    //!          「どのカメラがこのオブジェクトを描画するか」を制御する
    //! @param layer 設定するレイヤー
    //-----------------------------------------------------------
    void SetLayer(Layer::LayerID layer) { m_layer = layer; }

    //! @return true ならアクティブ（Update() が実行される）
    bool IsActive() const { return m_active; }

    //! @param active false にすると Update() をスキップする
    void SetActive(bool active) { m_active = active; }

    //! @return true なら描画対象（Draw() が実行される）
    bool IsVisible() const { return m_visible; }

    //! @param visible false にすると Draw() をスキップする
    void SetVisible(bool visible) { m_visible = visible; }

    //-----------------------------------------------------------
    //! @brief 描画順の設定
    //! @details SceneBase が描画時に、この値の小さいものから順に描画する
    //!          （同じ値の場合は追加された順）。
    //! @param order 描画順（値が小さいほど先＝奥に描画される）
    //-----------------------------------------------------------
    void SetDrawOrder(int order) { m_draw_order = order; }

    //! @return 描画順（値が小さいほど先に描画される）
    int GetDrawOrder() const { return m_draw_order; }

protected:
    // ライフサイクルのオーバーライド用
    virtual void OnInit() {}
    virtual void OnUpdate() {}
    virtual void OnDrawDebug() {}
    virtual void OnDraw() {}
    virtual void OnEnd() {}

private:
    std::vector<std::shared_ptr<ComponentBase>> m_components;    //!< コンポーネントのリスト
    std::string                                 m_name;          //!< オブジェクト名

    Layer::LayerID m_layer   = Layer::Default;    //!< 所属レイヤー（デフォルトは Layer::Default）
    bool           m_active  = true;               //!< true でない場合 Update() をスキップ
    bool           m_visible = true;               //!< true でない場合 Draw() をスキップ
    int            m_draw_order = 0;               //!< 描画順（小さいほど先に描画される）
};

}    // namespace Object
