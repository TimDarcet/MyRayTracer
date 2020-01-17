#pragma once

#include<vector>
#include "Vec3.h"
#include "Vertex.h"

using namespace std;

class Scene {
    public:
        Scene() {
            m_vertices = vector<Vertex>();
            m_triangles = vector<int>();
        }

    protected:
        vector<Vertex> m_vertices;
        vector<int> m_triangles;
};
