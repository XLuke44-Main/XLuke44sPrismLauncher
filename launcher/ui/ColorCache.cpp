#include "ColorCache.h"

QColor ColorCache::blend(QColor color)
{
    if (Rainbow::luma(m_front) > Rainbow::luma(m_back)) {

        color = Rainbow::tint(m_front, color, 0.5);
    }

    return Rainbow::mix(m_front, color, m_bias);
}

QColor ColorCache::blendBackground(QColor color)
{

    return Rainbow::mix(m_back, color, m_bias);
}

void ColorCache::recolorAll()
{
    auto iter = m_colors.begin();
    while (iter != m_colors.end()) {
        iter->front = blend(iter->original);
        iter->back = blendBackground(iter->original);
    }
}
