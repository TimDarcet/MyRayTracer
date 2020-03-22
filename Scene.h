#pragma once

#include <vector>
#include <random>
#include <cmath>
#include "Vec3.h"
#include "Vertex.h"
#include "Camera.h"
#include "Ray.h"
#include "Triangle.h"
#include "Mesh.h"
#include "LightSource.h"

using namespace std;

void printProgressBar(float prop) {
    int progress = round(50.0f * prop);
    string progressBar = "";
    for (int i=0; i<progress; i++) {
        progressBar += "\u2588";
    }
    std::cout << "Raytracing... [" << progressBar << string(50 - progress, ' ') << "] " << progress * 2 << "%\r" << flush;
}

Vec3f sample_along(Vec3f v) {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.0, 1.0);
    v.normalize();
    Vec3f v2, v3;
    v.getTwoOrthogonals(v2, v3);
    v2.normalize();
    v3.normalize();
    float theta = asin(dis(gen));
    float phi = 2 * M_PI * dis(gen);
    Vec3f dir = v2 * cos(phi) + v3 * sin(phi);
    dir.normalize();
    return normalize(v * cos(theta) + dir * sin(theta));
}

Vec3f jit_sample(int sample_idx, int n_samples) {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.0, 1.0);
    int d = int(sqrt(float(n_samples)));
    int j2 = sample_idx / d;
    int i2 = sample_idx % d;
    float x = (float(i2) + dis(gen)) / float(d);
    float y = (float(j2) + dis(gen)) / float(d);
    return {x, y, 42};
}

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
            // #pragma omp parallel for
            for (int i = 0; i < im.m_width; i++) {
                printProgressBar((float)(i + 1) / (float)im.m_width);
                #pragma omp parallel for
                for (int j = 0; j < im.m_height; j++) {
                    im.m_data[j * im.m_width + i] = {0, 0, 0};
                    #pragma omp parallel for
                    for (int sample_idx = 0; sample_idx < m_n_samples; sample_idx++) {
                        Vec3f noise = jit_sample(sample_idx, m_n_samples);
                        float x = (float(i) + noise[0]) / float(im.m_width);
                        float y = (float(j) + noise[1]) / float(im.m_height);
                        Ray rij = m_cam.launch_ray(x, y);
                        Vec3f this_ray_color = {0, 0, 0};
                        vector<float> nearest_intersection = {};
                        // for (Mesh const &m : m_meshes) {
                        //     Vec3i t;
                        //     vector<float> intersection = m.m_bvh.intersection(rij, t, m.m_vertices, m.m_triangles);
                        //     if (intersection.size() > 0) {
                        //         if (nearest_intersection.size() == 0 || nearest_intersection[3] > intersection[3]) {
                        //             nearest_intersection = intersection;
                        //             this_ray_color = this->colorize(intersection, t, m, rij.m_direction);
                        //         }
                        //     }
                        // }
                        // im.m_data[j * im.m_width + i] += this_ray_color;
                        im.m_data[j * im.m_width + i] += recurse_ray(rij, 0);
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
            for (Mesh const &m : this->m_meshes) {
                Vec3i t;
                vector<float> intersection = m.m_bvh.intersection(rvl, t, m.m_vertices, m.m_triangles);
                if (intersection.size() > 0) {
                    float t = intersection[3];
                    if (t <= (ls - v).length())
                        return false;
                }
            }
            return true;
        }

        Vec3f colorize(const vector<float> &intersection, const Vec3i &t, const Mesh &m, Vec3f rayDir) {
            // rng from https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
            std::random_device rd;  //Will be used to obtain a seed for the random number engine
            std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
            std::uniform_real_distribution<> dis(0.0, 1.0);
            Vec3f normal_at_point = intersection[0] * m.m_vertices[t[0]].m_normal
                                  + intersection[1] * m.m_vertices[t[1]].m_normal
                                  + intersection[2] * m.m_vertices[t[2]].m_normal;
            normal_at_point.normalize();
            Vec3f intersection_position = intersection[0] * m.m_vertices[t[0]].m_point
                                        + intersection[1] * m.m_vertices[t[1]].m_point
                                        + intersection[2] * m.m_vertices[t[2]].m_point;
            Vec3f overall_color;
            Vec3f color;
            for (LightSource light : this->m_lights) {
                Vec3f random_source;
                switch (light.m_type)
                {
                case L_AMBIENT:
                    color = light.m_intensity * light.m_color * max(0.0f, dot(normal_at_point, -rayDir));
                    color *= m.m_material.m_diffuse_coef * m.m_material.diffuse_response(intersection_position);
                    break;
                case L_POINT:
                    if (is_visible(intersection_position + 2 * __FLT_EPSILON__ * normal_at_point, light.m_position)) {
                        color = light.m_intensity * light.m_color * max(0.0f, dot(normal_at_point, -rayDir));
                        color *= m.m_material.evaluateColorResponse(normal_at_point,
                                                                    light.m_position - intersection_position,
                                                                    -rayDir,
                                                                    intersection_position);
                    }
                    else {
                        color = {0, 0, 0};
                    }
                    break;
                case L_RECTANGLE:
                    random_source = light.m_position + float(dis(gen)) * light.m_vec1 + float(dis(gen)) * light.m_vec2;
                    if (is_visible(intersection_position + 2 * __FLT_EPSILON__ * normal_at_point, random_source)) {
                        color = light.m_intensity * light.m_color * max(0.0f, dot(normal_at_point, -rayDir));
                        color *= m.m_material.evaluateColorResponse(normal_at_point,
                                                                    random_source - intersection_position,
                                                                    -rayDir,
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
                overall_color += color;
            }
            return overall_color;
        }

        Vec3f recurse_ray(Ray r, int depth) {
            if (depth > 3)
                return {0, 0, 0};
            Vec3f this_ray_color = {0, 0, 0};
            vector<float> nearest_intersection = {};
            Vec3i nearest_t;
            const Mesh *nearest_m;
            for (Mesh const &m : m_meshes) {
                Vec3i t;
                vector<float> intersection = m.m_bvh.intersection(r, t, m.m_vertices, m.m_triangles);
                if (intersection.size() > 0) {
                    if (nearest_intersection.size() == 0 || nearest_intersection[3] > intersection[3]) {
                        nearest_intersection = intersection;
                        nearest_t = t;
                        nearest_m = &m;
                    }
                }
            }
            if (nearest_intersection.size() > 0) {
                this_ray_color = this->colorize(nearest_intersection, nearest_t, *nearest_m, r.m_direction);
                Vec3f normal_at_point = nearest_intersection[0] * nearest_m->m_vertices[nearest_t[0]].m_normal
                                      + nearest_intersection[1] * nearest_m->m_vertices[nearest_t[1]].m_normal
                                      + nearest_intersection[2] * nearest_m->m_vertices[nearest_t[2]].m_normal;
                normal_at_point.normalize();
                Vec3f intersection_position = nearest_intersection[0] * nearest_m->m_vertices[nearest_t[0]].m_point
                                            + nearest_intersection[1] * nearest_m->m_vertices[nearest_t[1]].m_point
                                            + nearest_intersection[2] * nearest_m->m_vertices[nearest_t[2]].m_point;
                // Sample along normal
                Vec3f random_vector = sample_along(normal_at_point);
                // random_vector = -r.m_direction + 2.0f * (r.m_direction - dot(r.m_direction, normal_at_point) * normal_at_point); // Perfect reflection
                Vec3f recursed_color = recurse_ray(Ray(intersection_position, random_vector), depth + 1)
                                     * nearest_m->m_material.evaluateColorResponse(normal_at_point,
                                                                                   random_vector,
                                                                                   -r.m_direction,
                                                                                   intersection_position);
                return this_ray_color + recursed_color;
            }
            else {
                return {0, 0, 0};
            }
        }
};
