#include<vector>
#include<fstream>
#include<iostream>

using namespace std;

typedef struct vec3 {
    float x, y, z;
} vec3;

class Image {
    public:
        int m_width, m_height;
        vector<vec3> *m_data;
        
        Image() {
            m_width = 0;
            m_height = 0;
            m_data = new std::vector<vec3>;
        }
        Image(int w, int h) {
            m_width = w;
            m_height = h;
            m_data = new std::vector<vec3>;
            m_data->assign(w * h, {0.0, 0.0, 0.0});
        }
        

        void write(const char *fname) {
            int max_color = 255;
            ofstream outfile;
            outfile.open(fname, ios::out | ios::trunc );
            outfile << "P3" << endl << m_width << " " << m_height << endl << max_color << endl;
            for (int y = 0; y < m_height; y++) {
                for (int x = 0; x < m_width; x++) {
                    vec3 pxl = m_data->at(y * m_width + x);
                    outfile << (int)(max_color * pxl.x) << " " << (int)(max_color * pxl.y) << " " << (int)(max_color * pxl.z) << " ";
                }
                outfile << endl;
            }
            outfile.close();
        }

        void fillBackground(vec3 color1={0.0, 0.0, 1.0}, vec3 color2={1.0, 1.0, 1.0}) {
                // #pragma omp parallel for
                for (int y = 0; y < m_height; y++) {
                    for (int x = 0; x < m_width; x++) {
                        float t = (float)y / (float)m_height;
                        (*m_data)[y * m_width + x] = {(1 - t) * color1.x + t * color2.x,
                                                      (1 - t) * color1.y + t * color2.y,
                                                      (1 - t) * color1.z + t * color2.z};
                    }
                }
        }
};
