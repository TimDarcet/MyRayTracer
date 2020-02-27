#pragma once

#include <vector>
#include "Vec3.h"
#include <random>

using namespace std;

class Worley {
    public:
        int m_N;
        vector<Vec3f> m_points;
        Vec3f m_bb_pos; // Bounding box start position
        Vec3f m_bb_size; // Bounding box size (bb goes from m_bb_pos to m_bb_pos + m_bb_size)

        Worley(int n_points=10, Vec3f bb_pos={0, 0, 0}, Vec3f bb_size={1, 1, 1}) {
            m_N = n_points;
            m_bb_pos = bb_pos;
            m_bb_size = bb_size;
            // rng from https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
            std::random_device rd;  // Will be used to obtain a seed for the random number engine
            std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
            std::uniform_real_distribution<> dis(0.0, 1.0);
            for (int i=0; i<n_points; i++) {
                m_points.push_back({m_bb_pos[0] + m_bb_size[0] * (float)dis(gen),
                                    m_bb_pos[1] + m_bb_size[1] * (float)dis(gen),
                                    m_bb_pos[2] + m_bb_size[2] * (float)dis(gen)});
            }
        }

        float sample(Vec3f query_p, int k=2) const {
            vector<float> distances = vector<float>();
            for (Vec3f p : this->m_points) {
                float d = (query_p - p).length();
                float tmp;
                for (int i=0; i<distances.size(); i++) {
                    if (distances.at(i) > d) {
                        distances.insert(distances.begin() + i, d);
                        break;
                    }
                }
                if (distances.size() < k) {
                    distances.push_back(d);
                }
            }
            return distances.at(k - 1) / this->m_bb_size.length();
        }
};
