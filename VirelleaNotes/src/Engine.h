#pragma once
//============================================================================
//! @file   Engine.h
//! @brief  エンジンコアの宣言
//! @details ObjectBase のライフサイクルを管理するクラス群を提供します。
//! @author レオ
//============================================================================
#include "ObjectBase.h"
#include <vector>
#include <memory>
#include <string>

namespace Engine {

//-----------------------------------------------------------
//! @class Engine
//! @brief ObjectBase のライフサイクルを一括管理するクラス
//-----------------------------------------------------------
class Engine
{
public:
    //! @param obj 追加する ObjectBase
    void AddObject(std::shared_ptr<Object::ObjectBase> obj);

    //-----------------------------------------------------------
    //! @brief 指定した ObjectBase を登録解除する
    //! @details vector からの削除のみを行う。End() の呼び出しは
    //!          呼び出し側の責任とする（このクラスが End() を暗黙に
    //!          呼ぶと、多重 End() や意図しないタイミングでの終了処理を
    //!          招くおそれがあるため）。
    //! @param obj 削除する ObjectBase
    //! @return true 削除できた（見つかった） / false 見つからなかった
    //-----------------------------------------------------------
    bool RemoveObject(const std::shared_ptr<Object::ObjectBase>& obj);

    //-----------------------------------------------------------
    //! @brief 指定した名前の ObjectBase を登録解除する
    //! @details 同名オブジェクトが複数登録されている場合は最初に見つかった
    //!          1件のみを削除する。
    //! @param name 削除するオブジェクト名
    //! @return true 削除できた（見つかった） / false 見つからなかった
    //-----------------------------------------------------------
    bool RemoveObject(const std::string& name);

    //! 登録されている ObjectBase をすべて更新する
    void UpdateAll();

    //! 登録されている ObjectBase をすべて描画する
    void DrawAll();

    //! 登録されている ObjectBase の終了処理をすべて呼び出す
    void EndAll();

private:
    std::vector<std::shared_ptr<Object::ObjectBase>> m_game_objects;    //!< 登録されている ObjectBase の一覧
};

}    // namespace Engine
