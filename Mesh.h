#pragma once

#include <vector>
#include <fstream>

#include "Vec3.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Material.h"
#include "AABB.h"
#include "BVH.h"

using namespace std;

class Mesh {
    public:
        vector<Vertex> m_vertices;
        vector<Vec3i> m_triangles;
        Material m_material;
        AABB m_aabb;
        BVH m_bvh;

        Mesh() {
            m_vertices = vector<Vertex>();
            m_triangles = vector<Vec3i>();
            m_material = Material();
            m_aabb = AABB();
            m_bvh = BVH();
        }

        // Mesh(Mesh &other) {
        //     m_vertices = other.m_vertices;
        //     m_triangles = other.m_triangles;
        //     m_material = other.m_material;
        //     m_aabb = other.m_aabb;
        //     m_bvh = other.m_bvh;
        // }

        // Mesh &operator=(const Mesh &other) {
        //     if (this != &other){
        //         m_vertices = other.m_vertices;
        //         m_triangles = other.m_triangles;
        //         m_material = other.m_material;
        //         m_aabb = other.m_aabb;
        //         m_bvh = other.m_bvh;
        //     }
        //     return *this;
        // }

        /*
        * Deletes all geometry in mesh and replaces it with the off file
        */
        void loadOFF(const char *fname) {
            ifstream offstream;
            offstream.open(fname);
            if (!offstream.is_open()) {
                std::cout << "grmbl" << endl;
            }
            string line;
            offstream >> line;
            if (line.compare("OFF") != 0) {
                std::cout << "c mor 1" << endl;
                return;
            }
            int nv, nf, ne;
            offstream >> nv >> nf >> ne;
            m_vertices.clear();
            for (int i=0; i<nv; i++) {
                float x, y, z;
                offstream >> x >> y >> z;
                m_vertices.push_back(Vertex(x, y, z));
            }
            m_triangles.clear();
            for (int i=0; i<nf; i++) {
                int d;
                offstream >> d;
                if (d != 3) {
                    std::cout << "c mor" << endl;
                    return; // Can't handle that
                }
                int a, b, c;
                offstream >> a >> b >> c;
                m_triangles.push_back({a, b, c});
            }
            offstream.close();
            compute_normals();
        }

        void compute_normals() {
            for (Vertex &v : m_vertices) {
                v.m_normal = {0,0,0};
            }
            for (Vec3i &t : m_triangles) {
                Vec3f n = this->normal_at_triangle(t);
                m_vertices[t[0]].m_normal += n;
                m_vertices[t[1]].m_normal += n;
                m_vertices[t[2]].m_normal += n;
            }
            for (Vertex &v : m_vertices) {
                v.m_normal.normalize();
            }
        }

        void compute_aabb() {
            m_aabb.m_p1 = m_vertices[0].m_point - __FLT_EPSILON__ * 2 * Vec3f(1, 1, 1);
            m_aabb.m_p2 = m_vertices[0].m_point + __FLT_EPSILON__ * 2 * Vec3f(1, 1, 1);
            for (const Vertex &v: m_vertices) {
                for (int i=0; i<3; i++) {
                    m_aabb.m_p1[i] = min(m_aabb.m_p1[i], v.m_point[i] - __FLT_EPSILON__ * 2);
                    m_aabb.m_p2[i] = max(m_aabb.m_p2[i], v.m_point[i] + __FLT_EPSILON__ * 2);
                }
            }
        }

        void compute_BVH() {
            this->compute_aabb();
            this->m_bvh.m_aabb = this->m_aabb;
            for (int t_idx=0; t_idx<m_triangles.size(); t_idx++)
                m_bvh.m_triangles.push_back(t_idx);
            this->m_bvh.from_triangles(m_bvh.m_triangles.begin(), m_bvh.m_triangles.end(), m_vertices, m_triangles);
        }

        Vec3f normal_at_triangle(Vec3i &t) {
            Vec3f p0 = m_vertices[t[0]].m_point;
            Vec3f p1 = m_vertices[t[1]].m_point;
            Vec3f p2 = m_vertices[t[2]].m_point;
            Vec3f n = cross(p1 - p0, p2 - p0);
            n.normalize();
            return n;
        }

        float area_at_triangle(Vec3i &t) {
            Vec3f p0 = m_vertices[t[0]].m_point;
            Vec3f p1 = m_vertices[t[1]].m_point;
            Vec3f p2 = m_vertices[t[2]].m_point;
            return cross(p1 - p0, p2 - p0).length() / 2.0f;
        }
};
