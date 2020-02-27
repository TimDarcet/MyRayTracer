#pragma once

#include <vector>
#include <random>
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
        vector<LightSource> m_lights;
        int m_n_samples; // Should be a square of an integer, so that jittered sampling works properly

        Scene() {
            m_meshes = vector<Mesh>();
            m_cam = Camera();
            m_lights = vector<LightSource>();
            m_n_samples = 4;
        }

        void rayTrace(Image &im) {
            // rng from https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
            std::random_device rd;  //Will be used to obtain a seed for the random number engine
            std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
            std::uniform_real_distribution<> dis(0.0, 1.0);
            for (int i = 0; i < im.m_width; i++) {
                // Progress bar
                int progress = round(50.0f * (float)(i + 1) / (float)im.m_width);
                string progressBar = "";
                for (int i=0; i<progress; i++) {
                    progressBar += "\u2588";
                }
                std::cout << "Raytracing... [" << progressBar << string(50 - progress, ' ') << "] " << progress * 2 << "%\r" << flush;
                // std::cout << "Raytraced " << i+1 << "/" << im.m_width << "\r" << flush;
                for (int j = 0; j < im.m_height; j++) {
                    im.m_data[j * im.m_width + i] = {0, 0, 0};
                    for (int sample_idx = 0; sample_idx < m_n_samples; sample_idx++) {
                        // Jittered sampling
                        int d = int(sqrt(float(m_n_samples)));
                        int j2 = sample_idx / d;
                        int i2 = sample_idx % d;
                        float x = (float(i) + (float(i2) + dis(gen)) / float(d)) / float(im.m_width);
                        float y = (float(j) + (float(j2) + dis(gen)) / float(d)) / float(im.m_height);
                        Ray rij = m_cam.launch_ray(x, y);
                        vector<float> nearest_intersection = {};
                        Vec3f this_ray_color = {0, 0, 0};
                        for (Mesh const m : m_meshes) {
                            Vec3f entry, exit;
                            if (m.m_aabb.ray_intersection(rij, entry, exit)) {
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
                                            this_ray_color = {0, 0, 0};
                                            Vec3f color;
                                            for (LightSource light : this->m_lights) {
                                                Vec3f random_source;
                                                switch (light.m_type)
                                                {
                                                case L_AMBIENT:
                                                    color = light.m_intensity * light.m_color * max(0.0f, dot(normal_at_point, -rij.m_direction));
                                                    color *= m.m_material.diffuse_response(intersection_position);
                                                    break;
                                                case L_POINT:
                                                    if (is_visible(intersection_position + 2 * __FLT_EPSILON__ * t.normal(), light.m_position)) {
                                                        color = light.m_intensity * light.m_color * max(0.0f, dot(normal_at_point, -rij.m_direction));
                                                        color *= m.m_material.evaluateColorResponse(normal_at_point,
                                                                                                    light.m_position - intersection_position,
                                                                                                    -rij.m_direction,
                                                                                                    intersection_position);
                                                    }
                                                    else {
                                                        color = {0, 0, 0};
                                                    }
                                                    break;
                                                case L_RECTANGLE:
                                                    random_source = light.m_position + float(dis(gen)) * light.m_vec1 + float(dis(gen)) * light.m_vec2;
                                                    if (is_visible(intersection_position + 2 * __FLT_EPSILON__ * t.normal(), random_source)) {
                                                        color = light.m_intensity * light.m_color * max(0.0f, dot(normal_at_point, -rij.m_direction));
                                                        color *= m.m_material.evaluateColorResponse(normal_at_point,
                                                                                                    random_source - intersection_position,
                                                                                                    -rij.m_direction,
                                                                                                    intersection_position);
                                                    }
                                                    else {
                                                        color = {0, 0, 0};
                                                    }
                                                    break;
                                                default:
                                                    color = {0, 0, 0}; 
                                                    break;
                                                }
                                                this_ray_color += color;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        im.m_data[j * im.m_width + i] += this_ray_color;
                    }
                    im.m_data[j * im.m_width + i] *= 1 / float(m_n_samples);
                }
            }
        }

        /*
         * Checks if ls is visible from v
         *
         */
        bool is_visible(Vec3f v, Vec3f ls) {
            Ray rvl = Ray(v, normalize(ls - v));
            for (Mesh const m : this->m_meshes) {
                for (Triangle t : m.m_triangles) {
                    vector<float> intersection = rvl.intersect(t);
                    if (intersection.size() > 0) {
                        float t = intersection[3];
                        if (t <= (ls - v).length())
                            return false;
                    }
                }
            }
            return true;
        }
};
