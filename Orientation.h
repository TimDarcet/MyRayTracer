#pragma once

#include<vector>
#include "Vec3.h"

using namespace std;

class Orientation {
    public:
        Orientation(float yaw=0, float pitch=0, float roll=0) {
            m_yaw = yaw;
            m_pitch = pitch;
            m_roll = roll;
        }

        static void apply_rot(Vec3f &v, Vec3f base_v, float angle) {
            Vec3f rot_vec = base_v;
            rot_vec.normalize();
            Vec3f v_par = dot(v, rot_vec) * rot_vec;
            Vec3f v_per = v - v_par;
            v = v_par + cos(angle) * v_per + sin(angle) * cross(rot_vec, v_per);
        }

        void apply_rotations(Vec3f &v) {
            // Apply yaw
            Vec3f z0 = {0, 0, 1};
            apply_rot(v, z0, m_yaw);
            // Apply pitch
            Vec3f y1 = {0, -1, 0};
            apply_rot(y1, z0, m_yaw);
            apply_rot(v, y1, m_pitch);
            // Apply_roll
            Vec3f x2 = {1, 0, 0};
            apply_rot(x2, z0, m_yaw);
            apply_rot(x2, y1, m_pitch);
            apply_rot(v, x2, m_roll);
        }

    protected:
        // 3 floats: yaw-pitch-roll angles. yaw turns around z, pitch turns around -y, roll turns around x
        float m_yaw, m_pitch, m_roll;
};
