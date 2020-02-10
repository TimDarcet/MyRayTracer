#pragma once

#include<vector>
#include<fstream>
#include<iostream>
#include "Vec3.h"

using namespace std;


class Image {
    public:
        int m_width, m_height;
        vector<Vec3f> m_data; // Values between 0 and 1
        
        Image() {
            m_width = 0;
            m_height = 0;
            m_data = vector<Vec3f>();
        }
        Image(int w, int h) {
            m_width = w;
            m_height = h;
            m_data = vector<Vec3f>();
            m_data.assign(w * h, {0.0, 0.0, 0.0});
        }
        

        void write(const char *fname) {
            int max_color = 255;
            ofstream outfile;
            outfile.open(fname, ios::out | ios::trunc );
            outfile << "P3" << endl << m_width << " " << m_height << endl << max_color << endl;
            for (int y = 0; y < m_height; y++) {
                for (int x = 0; x < m_width; x++) {
                    Vec3f pxl = m_data.at(y * m_width + x);
                    int r = (int)(max_color * max(0.0f, min(1.0f, pxl[0])));
                    int g = (int)(max_color * max(0.0f, min(1.0f, pxl[1])));
                    int b = (int)(max_color * max(0.0f, min(1.0f, pxl[2])));
                    outfile << r << " " << g << " " << b << " ";
                }
                outfile << endl;
            }
            outfile.close();
        }

        void fillBackground(Vec3f color1={0.0, 0.0, 1.0}, Vec3f color2={1.0, 1.0, 1.0}) {
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
