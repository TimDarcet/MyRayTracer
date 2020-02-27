#pragma once

#include <vector>
#include "Vec3.h"
#include "Ray.h"

using namespace std;

class AABB {
    public:
        Vec3f m_p1;
        Vec3f m_p2;

        AABB(Vec3f p1={0,0,0}, Vec3f p2={0,0,0}) {
            m_p1 = p1;
            m_p2 = p2;
        }

        bool inside(Vec3f v) const {
            for (int i = 0; i < 3; i++)
                if (m_p1[i] - __FLT_EPSILON__ * 10 > v[i] || m_p2[i] + __FLT_EPSILON__ * 10 < v[i]) {
                    return false;
                }
            return true;
        }

        bool ray_intersection(Ray r, Vec3f &entry, Vec3f &exit) const {
            // Check if the ray start is inside the box
            // If yes, we know for sure there is an intersection
            // And we can skip the whole intersection and entry point calc step
            if (this->inside(r.m_start)) {
                entry = r.m_start;
            }
            else {
                Vec3f t;
                // Calculate intersections with the three candidate planes
                // Idea from https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
                for (int i = 0; i < 3; i++) {
                    if (r.m_direction[i] > 0)
                        t[i] = (m_p1[i] - r.m_start[i]) / r.m_direction[i];
                    else if (r.m_direction[i] < 0)
                        t[i] = (m_p2[i] - r.m_start[i]) / r.m_direction[i];
                    else
                        t[i] = -1; // Parallel to axis, not intersection
                }
                float maxt = max(t[0], max(t[1], t[2]));
                // Ray is directional
                if (maxt < 0){
                    return false;
                }
                // Check intersection is inside AABB
                entry = r.m_start + maxt * r.m_direction;
                if (!this->inside(entry)) {
                    return false;
                }
            }
            // We have found an intersection and an entry point ! Find the exit point now
            Vec3f t;
            for (int i = 0; i < 3; i++) {
                if (r.m_direction[i] > 0)
                    t[i] = (m_p2[i] - r.m_start[i]) / r.m_direction[i];
                else if (r.m_direction[i] < 0)
                    t[i] = (m_p1[i] - r.m_start[i]) / r.m_direction[i];
                else
                    t[i] = -1; // Parallel to axis, not intersection
            }
            float mint = min(t[0], min(t[1], t[2]));
            exit = r.m_start + mint * r.m_direction;
            return true;
        }
};
