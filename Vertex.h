#pragma once

#include <vector>
#include "Vec3.h"

using namespace std;

class Vertex {
    public:
        Vertex(float x=0.0f, float y=0.0f, float z=0.0f) {
            m_point = {x, y, z};    
        }
        
        Vec3f get_p() {
            return m_point;
        }

        Vec3f get_normal() {
            return m_normal;
        }

        void set_normal(Vec3f n) {
            m_normal = n;
        }

    protected:
        Vec3f m_point;
        Vec3f m_normal;
};
