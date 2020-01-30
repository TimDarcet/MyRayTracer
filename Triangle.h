#pragma once

#include <vector>
#include "Vec3.h"
#include "Vertex.h"

using namespace std;

class Triangle {
    public:
        Triangle(Vertex *p0, Vertex *p1, Vertex *p2) {
            m_vertices = {p0, p1, p2};
        }

        Vertex get_v(int i) {
            return *m_vertices[i];
        }

        Vec3f normal() {
            Vec3f p0 = m_vertices[0]->get_p();
            Vec3f p1 = m_vertices[1]->get_p();
            Vec3f p2 = m_vertices[2]->get_p();
            Vec3f n = cross(p1 - p0, p2 - p0);
            n.normalize();
            return n;
        }

        float area() {
            Vec3f p0 = m_vertices[0]->get_p();
            Vec3f p1 = m_vertices[1]->get_p();
            Vec3f p2 = m_vertices[2]->get_p();
            return cross(p1 - p0, p2 - p0).length() / 2.0f;
        }

    protected:
        vector<Vertex *> m_vertices;
};
