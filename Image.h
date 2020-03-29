#pragma once

#include<vector>
#include<fstream>
#include<iostream>
#include<functional>
#include<cmath>
#include<random>

#include "Vec3.h"
#include "PNG.h"

using namespace std;

// Returns the distance between the two vectors projected on the unit sphere
float projectedDistance(float x1, float y1, float x2, float y2) {
    return 2 - 2 * x1 * x2 - 2 * y1 * y2 - 2 * sqrt((1 - x1 * x1 - y1 * y1) * (1 - x2 * x2 - y2 * y2));
}

float projectedDistance(Vec3f v1, Vec3f v2) {
    return projectedDistance(v1[0], v1[1], v2[0], v2[1]);
}

Vec3f nearestSeed(Vec3f q, const vector<vector<Vec3f>> &seeds) {
    Vec3f nearest = {0.5, 0.5, 0};
    if (seeds.size() == 0 || seeds[0].size() == 0)
        return nearest;
    for (const vector<Vec3f> &layer: seeds) {
        for (Vec3f seed: layer) {
            if ((q - seed).length() < (q - nearest).length()) {
                nearest = seed;
            }
        }
    }
    return nearest;
}

// TODO: better search, kd-tree or something
vector<Vec3f> nearestSeeds(Vec3f q, const vector<vector<Vec3f>> &seeds, function<bool(Vec3f)> test) {
    vector<Vec3f> nearests;
    if (seeds.size() == 0 || seeds[0].size() == 0)
        return nearests;
    for (const vector<Vec3f> &layer: seeds) {
        Vec3f nearest = {0.5, 0.5, 0};
        for (Vec3f seed: layer) {
            if (test(seed) && (q - seed).length() < (q - nearest).length()) {
                nearest = seed;
            }
        }
        nearests.push_back(nearest);
    }
    return nearests;
}

float grid1d(float q, int d) {
    float step = pow(0.5, d + 1);
    if (q < step)
        return step;
    if (q > 1 - step)
        return 1 - step;
    else
        return round(q / step) * step;
}

Vec3f gridSeed(Vec3f q, int d) {
    return {grid1d(q[0], d), grid1d(q[1], d), 0};
}

void genSeeds(int depth, vector<vector<Vec3f>> &seeds) {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.0, 1.0);
    for (int d=0; d<depth; d++) {
        seeds.emplace_back();
        for (int i=0; i<pow(2, d + 1) - 1; i++) {
            for (int j=0; j<pow(2, d + 1) - 1; j++) {
                seeds.back().push_back((float)pow(0.5, d + 1) * Vec3f({i + 0.75f + 0.5f * float(dis(gen)), j + 0.75f + 0.5f * float(dis(gen)), 0}));
            }
        }
    }
}

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

        // The image should be a normal map
        // Style the image according to the lit sphere in file fname
        // Uses styleBlit's brute force algorithm
        void styleBlit(const char *fname, float threshold = 0.05) {
            vector<bool> done = vector<bool>(m_width * m_height, false);
            PNG_handler pngr = PNG_handler();
            pngr.read_png_file(fname);
            for (int y = 0; y < m_height; y++) {
                for (int x = 0; x < m_width; x++) {
                    if (!done[y * m_width + x]) {
                        Vec3f normal = m_data.at(y * m_width + x);
                        if (normal[2] <= __FLT_EPSILON__) {// If there is no mesh there or only the ground (-1 or 0)
                            m_data[y * m_width + x] = {0, 0, 0};
                            done[y * m_width + x] = true;
                        }
                        else {
                            float abs_norm = 0.5f + normal[0] / 2.0f;
                            float ord_norm = 0.5f + normal[1] / 2.0f;
                            int abs = (int)floor(abs_norm * pngr.width);
                            int ord = (int)floor(ord_norm * pngr.height);
                            #pragma omp parallel for
                            for (int y_src = 0; y_src < pngr.height; y_src++) {
                                #pragma omp parallel for
                                for (int x_src = 0; x_src < pngr.width; x_src++) {
                                    int x_new = x + x_src - abs;
                                    int y_new = y + y_src - ord;
                                    if (0<=y_new && y_new<m_height &&
                                        0<=x_new && x_new<m_width &&
                                        !done[y_new * m_width + x_new] &&
                                        m_data[y_new * m_width + x_new][2]>__FLT_EPSILON__) {
                                        Vec3f new_normal = m_data[y_new * m_width + x_new];
                                        Vec3f src_normal = {float(x_src) / float(pngr.width) * 2.0f - 1.0f, float(y_src) / float(pngr.height) * 2.0f - 1.0f, 0};
                                        float error = projectedDistance(new_normal[0],
                                                                        new_normal[1],
                                                                        src_normal[0],
                                                                        src_normal[1]);
                                        // cout << error << endl;
                                        if (error < threshold) {
                                            png_byte *src_pxl = &pngr.row_pointers[y_src][x_src * 4]; // *4 because of the number of channels
                                            // printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, src_pxl[0], src_pxl[1], src_pxl[2], src_pxl[3]);
                                            m_data[y_new * m_width + x_new] = {float((int)src_pxl[0]) / 255.0f,
                                                                               float((int)src_pxl[1]) / 255.0f,
                                                                               float((int)src_pxl[2]) / 255.0f};
                                            done[y_new * m_width + x_new] = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // The image should be a normal map
        // Style the image according to the lit sphere in file fname
        // Uses styleBlit's seeded version
        void styleBlitTree(const char *fname, float threshold = 0.05, int depth=8, float ball_scale = 0.3) {
            // Copy the guiding channels
            vector<Vec3f> guides = this->m_data;
            this->m_data.assign(m_width * m_height, {0, 0, 0});
            vector<float> coefs = vector<float>();
            coefs.assign(m_width * m_height, 0);

            // Generate seeds
            vector<vector<Vec3f>> seeds;
            genSeeds(depth, seeds);
            // Transfer style
            PNG_handler pngr = PNG_handler();
            pngr.read_png_file(fname);
            #pragma omp parallel for
            for (int y = 0; y < m_height; y++) {
                #pragma omp parallel for
                for (int x = 0; x < m_width; x++) {
                    Vec3f normal = guides[y * m_width + x];
                    //     this->m_data[y * m_width + x] = {0, 0, 0};
                    // else {
                    if (normal[2] > __FLT_EPSILON__) {
                        Vec3f pos_01 = {float(x) / float(m_width), float(y) / float(m_height), 0};
                        function<bool(Vec3f)> test = [this, &guides](Vec3f s){return abs(guides[round(floor(s[1] * m_height) * m_width + floor(s[0] * m_width))][2] - 1) < __FLT_EPSILON__;};
                        vector<Vec3f> ns = nearestSeeds(pos_01, seeds, test);
                        for (Vec3f s: ns) {
                            Vec3f target_seed_normal = guides[round(floor(s[1] * m_height) * m_width + floor(s[0] * m_width))];
                            Vec3f src_seed_01 = (target_seed_normal + Vec3f(1.0f, 1.0f, 0)) / 2.0f;
                            Vec3f predicted_src_01 = src_seed_01 + (pos_01 - s) / ball_scale;
                            Vec3f predicted_normal = {2.0f * float(predicted_src_01[0]) - 1.0f,
                                                      2.0f * float(predicted_src_01[1]) - 1.0f, 0};
                            float error = projectedDistance(normal, predicted_normal);
                            // cout << target_seed_normal << " | " << normal << " | " << s << " | " << pos_01 << " | " << src_seed_01 << " | " << predicted_normal << " | " << error << endl;
                            if (!isnan(error) && abs(target_seed_normal[2] - 1) < __FLT_EPSILON__) {
                                float coef = 1.0f / (1 + exp(5000 * (error - threshold)));
                                Vec3i predicted_src = {int(predicted_src_01[0] * pngr.width), int(predicted_src_01[1] * pngr.height), 0};
                                png_byte *src_pxl = &pngr.row_pointers[predicted_src[1]][predicted_src[0] * 4]; // *4 because of the number of channels
                                this->m_data[y * m_width + x] += coef * (1 - coefs[y * m_width + x]) * Vec3f(float((int)src_pxl[0]) / 255.0f,
                                                                                                             float((int)src_pxl[1]) / 255.0f,
                                                                                                             float((int)src_pxl[2]) / 255.0f);
                                coefs[y * m_width + x] += coef * (1 - coefs[y * m_width + x]);
                            }
                        }
                        m_data[y * m_width + x] /= coefs[y * m_width + x];
                    }
                }
            }
        }

        void styleBlitTreeCoords(float threshold = 0.05, int depth=8, float ball_scale = 0.3) {
            // Copy the guiding channels
            vector<Vec3f> guides = this->m_data;
            this->m_data.assign(m_width * m_height, {0, 0, 0});
            // vector<float> coefs = vector<float>();
            // coefs.assign(m_width * m_height, 0);

            // Generate seeds
            vector<vector<Vec3f>> seeds;
            genSeeds(depth, seeds);
            // Transfer style
            // PNG_handler pngr = PNG_handler();
            // pngr.read_png_file(fname);
            #pragma omp parallel for
            for (int y = 0; y < m_height; y++) {
                #pragma omp parallel for
                for (int x = 0; x < m_width; x++) {
                    Vec3f normal = guides[y * m_width + x];
                    if (normal[2] > __FLT_EPSILON__) {
                        Vec3f pos_01 = {float(x) / float(m_width), float(y) / float(m_height), 0};
                        function<bool(Vec3f)> test = [this, &guides](Vec3f s){return abs(guides[round(floor(s[1] * m_height) * m_width + floor(s[0] * m_width))][2] - 1) < __FLT_EPSILON__;};
                        vector<Vec3f> ns = nearestSeeds(pos_01, seeds, test);
                        for (Vec3f s: ns) {
                            Vec3f target_seed_normal = guides[round(floor(s[1] * m_height) * m_width + floor(s[0] * m_width))];
                            Vec3f src_seed_01 = (target_seed_normal + Vec3f(1.0f, 1.0f, 0)) / 2.0f;
                            Vec3f predicted_src_01 = src_seed_01 + (pos_01 - s) / ball_scale;
                            Vec3f predicted_normal = {2.0f * float(predicted_src_01[0]) - 1.0f,
                                                      2.0f * float(predicted_src_01[1]) - 1.0f, 0};
                            float error = projectedDistance(normal, predicted_normal);
                            // cout << target_seed_normal << " | " << normal << " | " << s << " | " << pos_01 << " | " << src_seed_01 << " | " << predicted_normal << " | " << error << endl;
                            if (!isnan(error) && abs(target_seed_normal[2] - 1) < __FLT_EPSILON__ && error < threshold) {
                                // float coef = 1.0f / (1 + exp(5000 * (error - threshold)));
                                // Vec3i predicted_src = {int(predicted_src_01[0] * pngr.width), int(predicted_src_01[1] * pngr.height), 0};
                                // png_byte *src_pxl = &pngr.row_pointers[predicted_src[1]][predicted_src[0] * 4]; // *4 because of the number of channels
                                // this->m_data[y * m_width + x] = Vec3f(float((int)src_pxl[0]) / 255.0f,
                                //                                       float((int)src_pxl[1]) / 255.0f,
                                //                                       float((int)src_pxl[2]) / 255.0f);
                                this->m_data[y * m_width + x] = Vec3f(predicted_src_01[0], predicted_src_01[1], predicted_src_01[2]);
                                break;
                                // coefs[y * m_width + x] += coef * (1 - coefs[y * m_width + x]);
                            }
                        }
                        // m_data[y * m_width + x] /= coefs[y * m_width + x];
                    }
                }
            }
        }

        // The image should be a normal map
        // Style the image according to the lit sphere in file fname
        void litSphere(const char *fname) {
            PNG_handler pngr = PNG_handler();
            pngr.read_png_file(fname);
            for (int y = 0; y < m_height; y++) {
                for (int x = 0; x < m_width; x++) {
                    Vec3f normal = m_data.at(y * m_width + x);
                    if (normal[2] <= __FLT_EPSILON__)
                        m_data[y * m_width + x] = {0, 0, 0};
                    else {
                        float abs_norm = 0.5f + normal[0] / 2.0f;
                        float ord_norm = 0.5f + normal[1] / 2.0f;
                        int abs = (int)floor(abs_norm * pngr.width);
                        int ord = (int)floor(ord_norm * pngr.height);
                        png_byte *src_pxl = &pngr.row_pointers[ord][abs * 4]; // *4 because of the number of channels
                        // printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, src_pxl[0], src_pxl[1], src_pxl[2], src_pxl[3]);
                        m_data[y * m_width + x] = {float((int)src_pxl[0]) / 255.0f,
                                                   float((int)src_pxl[1]) / 255.0f,
                                                   float((int)src_pxl[2]) / 255.0f};
                    }
                }
            }
        }

        void linearBlur(float radius) {
            // Copy the image
            vector<Vec3f> data_bak = this->m_data;
            #pragma omp parallel for
            for (int y = 0; y < m_height; y++) {
                #pragma omp parallel for
                for (int x = 0; x < m_width; x++) {
                    if (abs(m_data[y * m_width + x][2] - 1) < 1) {
                        Vec3f value = {0, 0, 0};
                        float coef = 0;
                        for (int y_off = -ceil(radius); y_off <= ceil(radius); y_off++) {
                            for (int x_off = -ceil(radius); x_off <= ceil(radius); x_off++) {
                                if (pow(x_off, 2) + pow(y_off, 2) < pow(radius, 2)) {
                                    value += data_bak[(y + y_off) * m_width + (x + x_off)];
                                    coef += 1;
                                }
                            }
                        }
                        m_data[y * m_width + x] = value / coef;
                    }
                }
            }
        }

        void transferFrom(const char *fname) {
            PNG_handler pngr = PNG_handler();
            pngr.read_png_file(fname);
            #pragma omp parallel for
            for (int y = 0; y < m_height; y++) {
                #pragma omp parallel for
                for (int x = 0; x < m_width; x++) {
                    if (abs(m_data[y * m_width + x][2] - 1) < 0.9) {
                        png_byte *src_pxl = &pngr.row_pointers[int(round(m_data[y * m_width + x][1] * pngr.height))][int(round(m_data[y * m_width + x][0] * pngr.width)) * 4]; // *4 because of the number of channels
                        this->m_data[y * m_width + x] = Vec3f(float((int)src_pxl[0]) / 255.0f,
                                                            float((int)src_pxl[1]) / 255.0f,
                                                            float((int)src_pxl[2]) / 255.0f);
                    }
                }
            }
        }
};
