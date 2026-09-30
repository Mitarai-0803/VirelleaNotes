#include "JudgeEffect.h"
#include "Functions.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

//----------------------------------------------------------------------------
//! @file   JudgeEffect.cpp
//! @brief  判定エフェクトの実装
//! @detail 判定時に短時間表示する矩形エフェクトの実装
//----------------------------------------------------------------------------

namespace Object {
//----------------------------------------------------------------------------
//! コンストラクタ
//----------------------------------------------------------------------------
JudgeEffect::JudgeEffect()
{
    OnInit();
}

//----------------------------------------------------------------------------
//! 初期化処理
//----------------------------------------------------------------------------
void JudgeEffect::OnInit()
{
    m_x        = 0.0f;
    m_y        = 0.0f;
    m_size     = 0.0f;
    m_max_size = 40.0f;
    m_duration = 0;
    m_elapsed  = 0;
    m_color    = GetColor(255, 255, 255);
    m_alive    = false;
    m_particles.clear();
}

//----------------------------------------------------------------------------
//! エフェクト開始
//----------------------------------------------------------------------------
void JudgeEffect::Start(float x, float y, unsigned int color, int duration, int count)
{
    m_x        = x;
    m_y        = y;
    m_color    = color;
    m_duration = duration;
    m_elapsed  = 0;
    m_size     = 0.0f;
    m_alive    = true;
    m_particles.clear();

    // srand の初期化（最初の呼び出し時のみ）
    static bool seeded = false;
    if(!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    // パーティクル生成（rand を利用）
    for(int i = 0; i < count; ++i) {
        Particle p{};
        float    r   = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        float    ang = r * 2.0f * 3.14159265f;
        r            = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        float spd    = 1.0f + r * 3.0f;    // 1.0 .. 4.0
        r            = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        float sz     = 6.0f + r * 8.0f;    // 6.0 .. 14.0
        p.x          = m_x;
        p.y          = m_y;
        p.vx         = std::cos(ang) * spd;
        p.vy         = std::sin(ang) * spd - 0.5f;    // 少し上方向に偏らせる
        p.size       = sz;
        p.max_life   = duration;
        p.life       = duration;
        p.alpha      = 255;
        m_particles.push_back(p);
    }
}

//----------------------------------------------------------------------------
//! 更新処理
//----------------------------------------------------------------------------
void JudgeEffect::OnUpdate()
{
    if(!m_alive)
        return;

    ++m_elapsed;
    if(m_elapsed >= m_duration) {
        m_alive = false;
        m_particles.clear();
        return;
    }

    // パーティクル毎の更新
    for(auto& p : m_particles) {
        // 速度に減衰を適用
        p.vx *= 0.98f;
        p.vy *= 0.98f;
        // 位置更新
        p.x += p.vx;
        p.y += p.vy;
        // 大きさを徐々に縮小
        p.size *= 0.96f;
        // ライフを減らしアルファを計算
        p.life -= 1;
        if(p.life < 0)
            p.life = 0;
        p.alpha = static_cast<int>(255.0f * (static_cast<float>(p.life) / static_cast<float>(p.max_life)));
    }
}

//----------------------------------------------------------------------------
//! 描画処理
//----------------------------------------------------------------------------
void JudgeEffect::OnDraw() const
{
    if(!m_alive)
        return;

    // アルファブレンドを有効化
    int prevBlendMode  = DX_BLENDMODE_NOBLEND;
    int prevBlendParam = 0;
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

    for(const auto& p : m_particles) {
        if(p.alpha <= 0)
            continue;
        unsigned int col = GetColor(GetRValue(m_color), GetGValue(m_color), GetBValue(m_color));
        // 色にアルファを適用するために DrawRectGraph を使わず、SetDrawBlendMode でアルファ設定
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, p.alpha);
        int halfs = static_cast<int>(p.size * 0.5f);
        DrawFillBox(static_cast<int>(p.x) - halfs, static_cast<int>(p.y) - halfs, static_cast<int>(p.x) + halfs, static_cast<int>(p.y) + halfs, m_color);
    }

    // ブレンドモードを元に戻す
    SetDrawBlendMode(prevBlendMode, prevBlendParam);
}

//----------------------------------------------------------------------------
//! 生存判定
//----------------------------------------------------------------------------
bool JudgeEffect::IsAlive() const
{
    return m_alive;
}
}    // namespace Object
