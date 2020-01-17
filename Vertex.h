#pragma once

#include<vector>
#include "Vec3.h"

using namespace std;

class Vertex {
    public:
        Vertex() {
            m_point = {0.0, 0.0, 0.0};    
        }

    protected:
        Vec3<float> m_point;
};
