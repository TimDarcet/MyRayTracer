#pragma once

#include <vector>
#include "Vec3.h"

using namespace std;

class LightSource {
    public:
        Vec3f m_position;
        Vec3f m_color;
        float m_intensity;

        LightSource(Vec3f position={0,0,0}, Vec3f color={1,1,1}, float intensity=1) {
            m_position = position;
            m_color = color;
            m_intensity = intensity;
        }
};
