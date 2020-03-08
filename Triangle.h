#pragma once

#include <vector>
#include "Vec3.h"
#include "Vertex.h"

using namespace std;

class Triangle {
    public:
        vector<int> m_vertices;

        Triangle(int p0, int p1, int p2) {
            m_vertices = {p0, p1, p2};
        }

        // Vec3f normal() {
        //     Vec3f p0 = m_vertices[0]->m_point;
        //     Vec3f p1 = m_vertices[1]->m_point;
        //     Vec3f p2 = m_vertices[2]->m_point;
        //     Vec3f n = cross(p1 - p0, p2 - p0);
        //     n.normalize();
        //     return n;
        // }

        // float area() {
        //     Vec3f p0 = m_vertices[0]->m_point;
        //     Vec3f p1 = m_vertices[1]->m_point;
        //     Vec3f p2 = m_vertices[2]->m_point;
        //     return cross(p1 - p0, p2 - p0).length() / 2.0f;
        // }
};
