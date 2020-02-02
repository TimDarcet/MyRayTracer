#pragma once

#include <vector>
#include "Vec3.h"
#include "Vertex.h"
#include "Triangle.h"

using namespace std;

class Ray {
    public:
        Vec3f m_start;
        Vec3f m_direction;

        Ray(Vec3f s={0.0f, 0.0f, 0.0f}, Vec3f d={1.0f, 0.0f, 0.0f}) {
            m_start = s;
            m_direction = d;
        }

        vector<float> intersect(Triangle t) {
            // Point vectors
            Vec3f p0 = t.m_vertices[0]->m_point;
            Vec3f p1 = t.m_vertices[1]->m_point;
            Vec3f p2 = t.m_vertices[2]->m_point;
            // Edges
            Vec3f e0 = p1 - p0;
            Vec3f e1 = p2 - p0;
            // Normal
            Vec3f n = cross(e0, e1);
            n.normalize();
            // Helper vars
            Vec3f q = cross(m_direction, e1);
            float a = dot(e0, q);
            // If is going the wrong way, is parallel to the triangle or if the triangle is degenerate, abort
            if (dot(n, m_direction) >= 0 || abs(a) < __FLT_EPSILON__)
                return vector<float>();
            // More helper vars
            Vec3f s = (m_start - p0) / a;
            Vec3f r = cross(s, e0);
            // Calculate barycentric coordinates
            float b0 = dot(s, q);
            float b1 = dot(r, m_direction);
            float b2 = 1 - b0 - b1;
            // Check the intersection is in the triangle
            if (b0 < 0 || b1 < 0 || b2 < 0)
                return vector<float>();
            float t0 = dot(e1, r);
            if (t0 < 0)
                return vector<float>();
            return {b0, b1, b2, t0};
        }
};
