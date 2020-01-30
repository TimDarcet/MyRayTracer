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
            m_normals.assign(m_vertices.size(), {0, 0, 0});
            vector<float> dividers;
            dividers.assign(m_vertices.size(), 0);
            for (Triangle t : m_triangles) {
                Vec3f n = t.normal();
                Vertex v0 = t.get_v(0);
                Vertex v1 = t.get_v(1);
                Vertex v2 = t.get_v(2);
                v0.set_normal(v0.get_normal() + n);
                v1.set_normal(v1.get_normal() + n);
                v2.set_normal(v2.get_normal() + n);
                cout << &v0 << endl << endl;
                cout << &m_vertices << endl << endl;
                
                for (Vertex v : m_vertices)
                    cout << &v << endl;
                cout << v0.get_normal() << endl;
            }
            // for (Vertex v : m_vertices)
            //     cout << v.get_normal() << endl;
            for (Vertex v : m_vertices) {
                // cout << v.get_normal() << endl;
                v.get_normal().normalize();
                // cout << v.get_normal() << endl;
            }
        }

        inline void add_vertex(Vertex v) {
            m_vertices.push_back(v);
        }

        inline void add_triangle(Triangle t) {
            m_triangles.push_back(t);
        }
};
