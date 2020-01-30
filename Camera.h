#pragma once

#include <math.h>
#include <vector>

#include "Vec3.h"
#include "Vertex.h"
#include "Orientation.h"
#include "Ray.h"

using namespace std;

class Camera {
    public:
        Camera(Vec3f position = {0, 0, 0}, Orientation orientation = {0, 0, 0}, float fov = M_PI / 4, float aspect_ratio = 1.5f) {
            m_position = position;
            m_orientation = orientation; // 3 floats: yaw-pitch-roll angles 
            m_fov = fov;
            m_aspect_ratio = aspect_ratio;
        }

        Vec3f direction_vec() {
            return m_orientation.apply_rotations({1, 0, 0});            
        }

        /*
         * x and y are floats between 0 and 1 representing coordinates in the image
         */
        Vec3f pixel_direction(float x, float y) {
            // Calculate dir in camera reference frame
            Vec3f up_left = {1, m_aspect_ratio * tan(m_fov/2), tan(m_fov/2)};
            up_left.normalize();
            Vec3f dir = up_left * Vec3f(1, 1-2*x, 1-2*y);
            // Rotate to canonical reference frame
            dir = m_orientation.apply_rotations(dir);
            return dir;
        }

        Ray launch_ray(float x, float y) {
            return Ray(m_position, pixel_direction(x, y));
        }

    protected:
        Vec3f m_position;
        Orientation m_orientation;
        float m_fov;
        float m_aspect_ratio;
};
