#pragma once

#include "Vec3.h"

using namespace std;

class Color {
    public:
        Vec3f m_rgb;

        Color(Vec3f rgb = {0, 0, 0}) {
            m_rgb = rgb;
        }

        void from_hsv(Vec3f hsv) {
            Vec3f rgb;
            int region;
            float remainder, p, q, t;

            if (hsv[1] <= 0)
            {
                rgb = {hsv[2], hsv[2], hsv[2]};
                m_rgb = rgb;
            }

            region = (int) (hsv[0] / 60.0f);
            remainder = (hsv[0] / 60.0f - region); 

            p = hsv[2] * (1 - hsv[1]);
            q = hsv[2] * (1 - hsv[1] * remainder);
            t = hsv[2] * (1 - hsv[1] * (1 - remainder));

            switch (region)
            {
                case 0:
                    m_rgb = {hsv[2], t, p};
                    break;
                case 1:
                    m_rgb = {q, hsv[2], p};
                    break;
                case 2:
                    m_rgb = {p, hsv[2], t};
                    break;
                case 3:
                    m_rgb = {p, q, hsv[2]};
                    break;
                case 4:
                    m_rgb = {t, p, hsv[2]};
                    break;
                default:
                    m_rgb = {hsv[2], p, q};
                    break;
            }
        }
};
