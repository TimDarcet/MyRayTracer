#pragma once

#include <vector>
#include <fstream>

#include "Vec3.h"
#include "Vertex.h"
#include "Triangle.h"

using namespace std;

class Mesh {
    public:
        vector<Vertex> m_vertices;
        vector<Triangle> m_triangles;
        vector<Vec3f> m_normals;

        Mesh() {
            m_vertices = vector<Vertex>();
            m_triangles = vector<Triangle>();
            m_normals = vector<Vec3f>();
        }

        /*
        * Deletes all geometry in mesh and replaces it with the off file
        */
        void loadOFF(const char *fname) {
            ifstream offstream;
            offstream.open(fname);
            if (!offstream.is_open()) {
                cout << "grmbl" << endl;
            }
            string line;
            offstream >> line;
            if (line.compare("OFF") != 0) {
                cout << "c mor 1" << endl;
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
                    cout << "c mor" << endl;
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

        // inline void add_vertex(Vertex v) {
        //     m_vertices.push_back(v);
        // }

        // inline void add_triangle(Triangle t) {
        //     m_triangles.push_back(t);
        // }
};
