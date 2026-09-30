//============================================================================
//! @file   Engine.cpp
//! @brief  エンジンコアの実装
//! @author レオ
//============================================================================
#include "Engine.h"
#include <algorithm>

namespace Engine {

void Engine::AddObject(std::shared_ptr<Object::ObjectBase> obj)
{
    m_game_objects.push_back(obj);
}

bool Engine::RemoveObject(const std::shared_ptr<Object::ObjectBase>& obj)
{
    auto it = std::find(m_game_objects.begin(), m_game_objects.end(), obj);
    if(it == m_game_objects.end()) {
        return false;
    }

    m_game_objects.erase(it);
    return true;
}

bool Engine::RemoveObject(const std::string& name)
{
    auto it = std::find_if(m_game_objects.begin(), m_game_objects.end(), [&](const std::shared_ptr<Object::ObjectBase>& obj) {
        return obj->GetName() == name;
    });
    if(it == m_game_objects.end()) {
        return false;
    }

    m_game_objects.erase(it);
    return true;
}

void Engine::UpdateAll()
{
    for(auto& obj : m_game_objects) {
        obj->Update();
    }
}

void Engine::DrawAll()
{
    for(auto& obj : m_game_objects) {
        obj->Draw();
    }
}

void Engine::EndAll()
{
    for(auto& obj : m_game_objects) {
        obj->End();
    }
}

}    // namespace Engine
