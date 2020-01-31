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
        vector<Mesh> m_meshes;
        Camera m_cam;

        Scene() {
            m_meshes = vector<Mesh>();
            m_cam = Camera();
        }
    
    // inline void add_mesh(Mesh m) {
    //     m_meshes.push_back(m);
    // }

    void rayTrace(Image &im) {
        for (int i = 0; i < im.m_width; i++) {
            for (int j = 0; j < im.m_height; j++) {
                Ray rij = m_cam.launch_ray(float(i) / float(im.m_width), float(j) / float(im.m_height));
                for (Mesh const m : m_meshes) {
                    for (Triangle t : m.m_triangles) {
                        vector<float> intersection = rij.intersect(t);
                        if (intersection.size() > 0) {
                            Vec3f normal_at_point = intersection[0] * t.m_vertices[0]->m_normal
                                                  + intersection[1] * t.m_vertices[1]->m_normal
                                                  + intersection[2] * t.m_vertices[2]->m_normal;
                            normal_at_point = {abs(normal_at_point[0]),
                                               abs(normal_at_point[1]),
                                               abs(normal_at_point[2])};
                            normal_at_point.normalize();
                            im.m_data[j * im.m_width + i] = normal_at_point;
                            break;
                        }
                    }
                }
            }
        }
    }
};
