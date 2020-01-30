#pragma once

#include <vector>
#include "Vec3.h"
#include "Vertex.h"
#include "Camera.h"
#include "Ray.h"
#include "Triangle.h"
#include "Mesh.h"

using namespace std;

class Scene {
    public:
        Scene() {
            m_meshes = vector<Mesh>();
            m_cam = Camera();
        }
    
    inline void add_mesh(Mesh m) {
        m_meshes.push_back(m);
    }

    inline void set_camera(Camera cam) {
        m_cam = cam;
    }

    void rayTrace(Image &im) {
        for (int i = 0; i < im.m_width; i++) {
            for (int j = 0; j < im.m_height; j++) {
                Ray rij = m_cam.launch_ray(float(i) / float(im.m_width), float(j) / float(im.m_height));
                for (Mesh const m : m_meshes) {
                    for (Triangle t : m.m_triangles) {
                        vector<float> intersection = rij.intersect(t);
                        if (intersection.size() > 0) {
                            // cout << intersection[0] << intersection[1] << intersection[2] << intersection[3] << endl; 
                            Vec3f normal_at_point = intersection[0] * t.get_v(0).get_normal()
                                                  + intersection[1] * t.get_v(1).get_normal()
                                                  + intersection[2] * t.get_v(2).get_normal();
                            im.m_data[j * im.m_width + i] = normal_at_point;
                            break;
                        }
                    }
                }
            }
        }
    }

    protected:
        vector<Mesh> m_meshes;
        Camera m_cam;
};
