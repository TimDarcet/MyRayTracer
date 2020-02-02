#pragma once

#include <vector>
#include "Vec3.h"
#include "Vertex.h"
#include "Camera.h"
#include "Ray.h"
#include "Triangle.h"
#include "Mesh.h"
#include "LightSource.h"

using namespace std;

class Scene {
    public:
        vector<Mesh> m_meshes;
        Camera m_cam;
        LightSource m_light;

        Scene() {
            m_meshes = vector<Mesh>();
            m_cam = Camera();
            m_light = LightSource();
        }
    
    // inline void add_mesh(Mesh m) {
    //     m_meshes.push_back(m);
    // }

    void rayTrace(Image &im) {
        for (int i = 0; i < im.m_width; i++) {
            for (int j = 0; j < im.m_height; j++) {
                Ray rij = m_cam.launch_ray(float(i) / float(im.m_width), float(j) / float(im.m_height));
                vector<float> nearest_intersection = {};
                for (Mesh const m : m_meshes) {
                    for (Triangle t : m.m_triangles) {
                        vector<float> intersection = rij.intersect(t);
                        if (intersection.size() > 0) {
                            if (nearest_intersection.size() == 0 || nearest_intersection[3] > intersection[3]) {
                                nearest_intersection = intersection;
                                Vec3f normal_at_point = intersection[0] * t.m_vertices[0]->m_normal
                                                    + intersection[1] * t.m_vertices[1]->m_normal
                                                    + intersection[2] * t.m_vertices[2]->m_normal;
                                normal_at_point.normalize();
                                Vec3f intersection_position = intersection[0] * t.m_vertices[0]->m_point
                                                            + intersection[1] * t.m_vertices[1]->m_point
                                                            + intersection[2] * t.m_vertices[2]->m_point;
                                Vec3f color = m.m_material.evaluateColorResponse(normal_at_point,
                                                                                -rij.m_direction,
                                                                                m_light.m_position - intersection_position);
                                im.m_data[j * im.m_width + i] = color;
                            }
                        }
                    }
                }
            }
        }
    }
};
