#pragma once

#include<vector>
#include<fstream>
#include<iostream>
#include "Vec3.h"

using namespace std;


class Image {
    public:
        int m_width, m_height;
        vector<Vec3<float>> m_data;
        
        Image() {
            m_width = 0;
            m_height = 0;
            m_data = vector<Vec3<float>>();
        }
        Image(int w, int h) {
            m_width = w;
            m_height = h;
            m_data = vector<Vec3<float>>();
            m_data.assign(w * h, {0.0, 0.0, 0.0});
        }
        

        void write(const char *fname) {
            int max_color = 255;
            ofstream outfile;
            outfile.open(fname, ios::out | ios::trunc );
            outfile << "P3" << endl << m_width << " " << m_height << endl << max_color << endl;
            for (int y = 0; y < m_height; y++) {
                for (int x = 0; x < m_width; x++) {
                    Vec3<float> pxl = m_data.at(y * m_width + x);
                    outfile << (int)(max_color * pxl[0]) << " " << (int)(max_color * pxl[1]) << " " << (int)(max_color * pxl[2]) << " ";
                }
                outfile << endl;
            }
            outfile.close();
        }

        void fillBackground(Vec3<float> color1={0.0, 0.0, 1.0}, Vec3<float> color2={1.0, 1.0, 1.0}) {
                // #pragma omp parallel for
                for (int y = 0; y < m_height; y++) {
                    for (int x = 0; x < m_width; x++) {
                        float t = (float)y / (float)m_height;
                        m_data[y * m_width + x] = {(1 - t) * color1[0] + t * color2[0],
                                                   (1 - t) * color1[1] + t * color2[1],
                                                   (1 - t) * color1[2] + t * color2[2]};
                    }
                }
        }
};
