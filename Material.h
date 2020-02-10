#pragma once

#include <vector>
#include "Vec3.h"

using namespace std;

class Material {
    public:
        Vec3f m_albedo;
        float m_diffuse_coef;
        float m_specular_coef;
        float m_shininess;

        Material(Vec3f albedo={1,0.7,0.8}, float diffuse_coef=0.5, float specular_coef=0.5, float s=50) {
            m_albedo = albedo;
            m_diffuse_coef = diffuse_coef;
            m_specular_coef = specular_coef;
            m_shininess = s;
        }

        Vec3f diffuse_response() const {
            return m_albedo * m_diffuse_coef / M_PI;
        }

        Vec3f evaluateColorResponse(const Vec3f &normal, const Vec3f &wi, const Vec3f wo) const {
            Vec3f wh = normalize((normalize(wo) + normalize(wi)) / 2);
            float d = dot(wh, normalize(normal));
            return  diffuse_response() + m_albedo * (m_specular_coef * pow(d, m_shininess));
        }
};
