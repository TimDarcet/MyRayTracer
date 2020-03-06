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
        vector<Triangle> m_triangles;
        Material m_material;
        AABB m_aabb;
        BVH m_bvh;

        Mesh() {
            m_vertices = vector<Vertex>();
            m_triangles = vector<Triangle>();
            m_material = Material();
            m_aabb = AABB();
            m_bvh = BVH();
        }

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
                m_triangles.push_back(Triangle(&m_vertices[a], &m_vertices[b], &m_vertices[c]));
            }
            offstream.close();
            compute_normals();
        }

        void compute_normals() {
            for (Vertex v : m_vertices) {
                v.m_normal = {0,0,0};
            }
            for (Triangle t : m_triangles) {
                Vec3f n = t.normal();
                Vertex *v0 = t.m_vertices[0];
                Vertex *v1 = t.m_vertices[1];
                Vertex *v2 = t.m_vertices[2];
                v0->m_normal += n;
                v1->m_normal += n;
                v2->m_normal += n;
            }
            for (Vertex v : m_vertices) {
                v.m_normal.normalize();
            }
        }

        void compute_aabb() {
            for (Vertex v: m_vertices) {
                for (int i=0; i<3; i++) {
                    m_aabb.m_p1[i] = min(m_aabb.m_p1[i], v.m_point[i] - __FLT_EPSILON__ * 10);
                    m_aabb.m_p2[i] = max(m_aabb.m_p2[i], v.m_point[i] + __FLT_EPSILON__ * 10);
                }
            }
        }

        void compute_BVH() {
            this->compute_aabb();
            this->m_bvh.m_aabb = this->m_aabb;
            for (Triangle &t: this->m_triangles) {
                this->m_bvh.m_triangles->push_back(&t);
            }
            this->m_bvh.from_triangles(this->m_bvh.m_triangles);
        }
};
