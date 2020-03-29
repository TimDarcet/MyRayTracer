#pragma once

#include <vector>
#include "Vec3.h"
#include "Worley.h"
#include "Color.h"

#define M_BLINN_PHONG 0
#define M_MICROFACETS 1

using namespace std;

class Material {
    public:
        Vec3f m_albedo;
        float m_shininess; // For Blinn-Phong model
        float m_alpha; // For microfacets model (in the normal distribution)
        Vec3f m_F0; // Fresnel reflectance, for microfacets model (in the Fresnel term)
        int m_type;
        float m_diffuse_coef;
        Worley m_noise;

        Material(Vec3f albedo={1,0.7,0.8},
                 int type=M_BLINN_PHONG,
                 float diffuse_coef=0.5,
                 float s=50,
                 float alpha=0.5,
                 Vec3f F0={0.5, 0.5, 0.5},
                 int worley_n=10,
                 Vec3f worley_pos={0,0,0},
                 Vec3f worley_size={1,1,1}) {
            m_type = type;
            m_albedo = albedo;
            m_diffuse_coef = diffuse_coef;
            m_shininess = s;
            m_alpha = alpha;
            m_F0 = F0;
            m_noise = Worley(worley_n, worley_pos, worley_size);
        }

        Vec3f get_albedo(Vec3f pos) const {
            // Color albedo;
            // albedo.from_hsv({m_noise.sample(pos, 1) * 360.0f, 0.5, 0.5});
            // return albedo.m_rgb;
            return m_albedo;
        }

        float get_shininess(Vec3f pos) const {
            // return m_shininess * (1 + 0.2 * (m_noise.sample(pos, 2) - 1) / 2);
            return m_shininess;
        }

        float get_alpha(Vec3f pos) const {
            // return m_alpha * (1 + 0.2 * (m_noise.sample(pos, 1) - 1) / 2);
            return m_alpha;
        }

        Vec3f get_F0(Vec3f pos) const {
            // return m_F0 * (1 + 0.2 * (m_noise.sample(pos, 2) - 1) / 2);
            return m_F0;
        }

        Vec3f evaluateColorResponse(const Vec3f normal, const Vec3f wi, const Vec3f wo, Vec3f pos) const {
            return m_diffuse_coef * diffuse_response(pos) + (1 - m_diffuse_coef) * specular_response(normalize(normal), normalize(wi), normalize(wo), pos);
        }

        Vec3f diffuse_response(Vec3f pos) const {
            // Lambert model
            return this->get_albedo(pos) / M_PI;
        }

        Vec3f specular_response(const Vec3f normal, const Vec3f wi, const Vec3f wo, Vec3f pos) const {
            switch (this->m_type) {
                case M_BLINN_PHONG:
                    return this->get_albedo(pos) * evaluateBlinnPhong(normal, wi, wo, pos);
                    break;
                case M_MICROFACETS:
                    return evaluateMicrofacets(normal, wi, wo, pos);
                    break;
                default:
                    return {0, 0, 0};
                    break;
            }
        }

        Vec3f evaluateBlinnPhong(const Vec3f normal, const Vec3f wi, const Vec3f wo, Vec3f pos) const {
            Vec3f wh = normalize((normalize(wo) + normalize(wi)) / 2);
            float d = dot(wh, normalize(normal));
            float r = pow(d, this->get_shininess(pos));
            return {r, r, r};
        }

        Vec3f evaluateMicrofacets(const Vec3f normal, const Vec3f wi, const Vec3f wo, Vec3f pos) const {
            Vec3f r = normalDistrib(normal, wi, wo, pos) * fresnel(normal, wi, wo, pos) * geometric(normal, wi, wo, pos) / (4 * dot(normal, wi) * dot(normal, wo));
            return r;
        }

        inline float normalDistrib(const Vec3f normal, const Vec3f wi, const Vec3f wo, Vec3f pos) const {
            // GGX normal distribution
            Vec3f wh = normalize((normalize(wo) + normalize(wi)) / 2);
            float a2 = pow(this->get_alpha(pos), 2);
            float r = a2 / (M_PI * pow(1 + (a2 - 1) * pow(dot(normal, wh), 2), 2));
            return r;
        }
        
        inline Vec3f fresnel(const Vec3f normal, const Vec3f wi, const Vec3f wo, Vec3f pos) const {
            // Schlick approximation
            Vec3f wh = normalize((normalize(wo) + normalize(wi)) / 2);
            Vec3f r = this->get_F0(pos) + (Vec3f({1, 1, 1}) - this->get_F0(pos)) * pow(1 - max(0.0f, dot(wi, wh)), 5);
            return r;
        }

        inline float geometric(const Vec3f normal, const Vec3f wi, const Vec3f wo, Vec3f pos) const {
            // Smith geometric term
            float r = G_Smith(normal, wi, pos) * G_Smith(normal, wo, pos);
            return r;

            // Vec3f wh = normalize((normalize(wo) + normalize(wi)) / 2);
            // float shadowing = 2 * dot(normal, wh) * dot(normal, wi) / dot(wo, wh);
            // float masking = 2 * dot(normal, wh) * dot(normal, wo) / dot(wo, wh);
            // return min(1.0f, min(shadowing, masking));
        }

        inline float G_Smith(const Vec3f normal, const Vec3f w, Vec3f pos) const {
            // Schlick approximation
            float d = dot(normal, w);
            // float d = max(0.0f, dot(normal, w));
            float k = this->get_alpha(pos) * sqrt(2 / M_PI);
            return d / (k + d * (1 - k));
        }
};
