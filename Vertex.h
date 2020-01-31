#pragma once

#include <vector>
#include "Vec3.h"

using namespace std;

class Vertex {
    public:
        Vec3f m_point;
        Vec3f m_normal;

        Vertex(float x=0.0f, float y=0.0f, float z=0.0f) {
            m_point = {x, y, z};
            m_normal = {0.0f, 0.0f, 0.0f};
        }
};
