#pragma once

#include <vector>
#include "Vec3.h"

#define M_BLINN_PHONG 0
#define M_MICROFACETS 1

using namespace std;

class Material {
    public:
        Vec3f m_albedo;
        int m_type;
        float m_diffuse_coef;
        float m_specular_coef;
        float m_shininess; // For Blinn-Phong model
        float m_alpha; // For microfacets model (in the normal distribution)
        Vec3f m_F0; // Fresnel reflectance, for microfacets model (in the Fresnel term)

        Material(Vec3f albedo={1,0.7,0.8},
                 int type=M_BLINN_PHONG,
                 float diffuse_coef=0.5,
                 float specular_coef=0.5,
                 float s=50,
                 float alpha=0.5,
                 Vec3f F0={0.05, 0.05, 0.05}) {
            m_type = type;
            m_albedo = albedo;
            m_diffuse_coef = diffuse_coef;
            m_specular_coef = specular_coef;
            m_shininess = s;
            m_alpha = alpha;
            m_F0 = F0;
        }

        Vec3f evaluateColorResponse(const Vec3f normal, const Vec3f wi, const Vec3f wo) const {
            return diffuse_response() + specular_response(normalize(normal), normalize(wi), normalize(wo));
        }

        Vec3f diffuse_response() const {
            // Lambert model    
            return this->m_albedo * m_diffuse_coef / M_PI;
        }

        Vec3f specular_response(const Vec3f normal, const Vec3f wi, const Vec3f wo) const {
            switch (this->m_type) {
                case M_BLINN_PHONG:
                    return this-> m_albedo * evaluateBlinnPhong(normal, wi, wo);
                    break;
                case M_MICROFACETS:
                    return evaluateMicrofacets(normal, wi, wo); // TODO: should I put the albedo as a factor here or not?
                    break;
                default:
                    return {0, 0, 0};
                    break;
            }
        }

        Vec3f evaluateBlinnPhong(const Vec3f normal, const Vec3f wi, const Vec3f wo) const {
            Vec3f wh = normalize((normalize(wo) + normalize(wi)) / 2);
            float d = dot(wh, normalize(normal));
            float r = m_specular_coef * pow(d, m_shininess);
            return {r, r, r};
        }

        Vec3f evaluateMicrofacets(const Vec3f normal, const Vec3f wi, const Vec3f wo) const {
            Vec3f r = normalDistrib(normal, wi, wo) * fresnel(normal, wi, wo) * geometric(normal, wi, wo) / (4 * dot(normal, wi) * dot(normal, wo));
            return r;
        }

        inline float normalDistrib(const Vec3f normal, const Vec3f wi, const Vec3f wo) const {
            // GGX normal distribution
            Vec3f wh = normalize((normalize(wo) + normalize(wi)) / 2);
            float a2 = pow(this->m_alpha, 2);
            float r = a2 / (M_PI * pow(1 + (a2 - 1) * pow(dot(normal, wh), 2), 2));
            return r;
        }
        
        inline Vec3f fresnel(const Vec3f normal, const Vec3f wi, const Vec3f wo) const {
            // Schlick approximation
            Vec3f wh = normalize((normalize(wo) + normalize(wi)) / 2);
            Vec3f r = this->m_F0 + (Vec3f({1, 1, 1}) - this->m_F0) * pow(1 - max(0.0f, dot(wi, wh)), 5);
            return r;
        }

        inline float geometric(const Vec3f normal, const Vec3f wi, const Vec3f wo) const {
            // Smith geometric term
            float r = G_Smith(normal, wi) * G_Smith(normal, wo);
            return r;

            // Vec3f wh = normalize((normalize(wo) + normalize(wi)) / 2);
            // float shadowing = 2 * dot(normal, wh) * dot(normal, wi) / dot(wo, wh);
            // float masking = 2 * dot(normal, wh) * dot(normal, wo) / dot(wo, wh);
            // return min(1.0f, min(shadowing, masking));
        }

        inline float G_Smith(const Vec3f normal, const Vec3f w) const {
            // Schlick approximation
            float d = dot(normal, w);
            // float d = max(0.0f, dot(normal, w));
            float k = this->m_alpha * sqrt(2 / M_PI);
            return d / (k + d * (1 - k));
        }
};
