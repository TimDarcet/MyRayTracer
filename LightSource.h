#pragma once

#include <vector>
#include "Vec3.h"

#define L_AMBIENT 0
#define L_POINT 1
#define L_RECTANGLE 2


using namespace std;

class LightSource {
    public:
        Vec3f m_position;
        Vec3f m_color;
        float m_intensity;
        int m_type;
        Vec3f m_vec1; // two vectors of the rectangle light
        Vec3f m_vec2;
        

        LightSource(Vec3f position={0,0,0}, Vec3f color={1,1,1}, float intensity=1, int type=L_AMBIENT, Vec3f vec1={1, 0, 0}, Vec3f vec2={0, 1, 0}) {
            m_position = position;
            m_color = color;
            m_intensity = intensity;
            m_type = type;
            m_vec1 = vec1;
            m_vec2 = vec2;
        }
};
