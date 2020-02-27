#pragma once

#include <vector>
#include "Vec3.h"
#include "Orientation.h"
#include "Mesh.h"

using namespace std;

class Transform {
    public:
        Transform(Vec3f translation={0, 0, 0}, Orientation rotation={0, 0, 0}, float scale=1) {
            m_translation = translation;
            m_rotation = rotation;
            m_scale = scale;
        }

        /*
         * Resolves transforms in order : translation, orientation, scale
         */
        void apply_transform(Vec3f &v) {
            v += m_translation;
            m_rotation.apply_rotations(v);
            v = m_scale * v;
        }
        
        void apply_transform(Mesh &m) {
            for (Vertex &v : m.m_vertices) {
                this->apply_transform(v.m_point);
            }
            m.compute_aabb();
        }

    protected:
        Vec3f m_translation;
        Orientation m_rotation;
        float m_scale;
};
