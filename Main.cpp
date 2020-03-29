#include <iostream>
#include <cstring>
#include <ctime>
#include <math.h>
#include <chrono>


#include "Image.h"
#include "Scene.h"
#include "Transform.h"
#include "LightSource.h"
#include "Worley.h"

int main(int argc, char **argv) {
    int width = 1000;
    int height = 1500;
    int n_samples = 1;
    char *output = (char *)"out.ppm";
    // Parse arguments
    for (int i = 1; i < argc - 1; i++) {
        if (!strcmp(argv[i], "-width"))
            width = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-height"))
            height = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-nsamples"))
            n_samples = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-output"))
            output = (char *)argv[i + 1];
    }
    // Create image
    Image imtest = Image(width, height);
    std::cout << "Made an image of size " << imtest.m_width << "x" << imtest.m_height << endl;
    // Fill background
    imtest.fillBackground({0, 0, 0}, {1, 1, 1});
    std::cout << "Filled bg" << endl;
    // Create scene
    Scene test_scene;
    test_scene.m_n_samples = n_samples;
    std::cout << "Created scene" << endl;
    // Create camera
    test_scene.m_cam = Camera({0, 0, 3}, {0, -M_PI/2, 0}, M_PI/4, 0.6666f);
    std::cout << "Created camera" << endl;
    // Create ground
    test_scene.m_meshes.emplace_back();
    Vertex v0 = {-0.3, -3, -3};
    Vertex v1 = {-0.3, -3, 3};
    Vertex v2 = {-0.3, 3, -3};
    Vertex v3 = {-0.3, 3, 3};
    test_scene.m_meshes.back().m_vertices.push_back(v0);
    test_scene.m_meshes.back().m_vertices.push_back(v1);
    test_scene.m_meshes.back().m_vertices.push_back(v2);
    test_scene.m_meshes.back().m_vertices.push_back(v3);
    test_scene.m_meshes.back().m_triangles.push_back({1, 0, 2});
    test_scene.m_meshes.back().m_triangles.push_back({1, 2, 3});
    for (Vertex &v : test_scene.m_meshes.back().m_vertices) {
        v.m_normal = {1, 0, 0};
    }
    test_scene.m_meshes.back().compute_BVH();
    std::cout << "Created ground" << endl;
    // Import model
    test_scene.m_meshes.emplace_back();
    test_scene.m_meshes.back().loadOFF("../example.off");
    Transform t({0,0,0}, {1.5, 3.5, 3}, 1);
    t.apply_transform(test_scene.m_meshes.back());
    test_scene.m_meshes.back().compute_normals();
    test_scene.m_meshes.back().m_material.m_type = M_MICROFACETS;
    test_scene.m_meshes.back().m_material.m_albedo = {0.83, 0.68, 0.214}; // Golden color
    test_scene.m_meshes.back().m_material.m_F0 = {1.0, 0.71, 0.29};
    test_scene.m_meshes.back().m_material.m_alpha = 0;
    // test_scene.m_meshes.back().m_material.m_type = M_BLINN_PHONG;
    // test_scene.m_meshes.back().m_material.m_albedo = {1, 1, 1};
    // test_scene.m_meshes.back().m_material.m_diffuse_coef = 0;
    // test_scene.m_meshes.back().m_material.m_shininess = 512;
    test_scene.m_meshes.back().m_material.m_noise = Worley(50, {-0.3, -0.5, -0.5}, {0.7, 1, 1});
    test_scene.m_meshes.back().compute_BVH();
    std::cout << "Imported model" << endl;
    // Import model 2
    // test_scene.m_meshes.emplace_back();
    // test_scene.m_meshes.back().loadOFF("../tetra.off");
    // Transform t_2({3,0,-1}, {1, 1, 1}, 0.2);
    // t_2.apply_transform(test_scene.m_meshes.back());
    // test_scene.m_meshes.back().compute_normals();
    // test_scene.m_meshes.back().m_material.m_type = M_MICROFACETS;
    // // test_scene.m_meshes.back().m_material.m_noise = Worley(50, {-0.3, -0.5, -0.5}, {0.7, 1, 1});
    // test_scene.m_meshes.back().compute_BVH();
    // std::cout << "Imported model 2" << endl;
    // Import model 3
    // test_scene.m_meshes.emplace_back();
    // test_scene.m_meshes.back().loadOFF("../example.off");
    // Transform t_3({1.5,0,-0.4}, {1, 1, 1}, 0.4);
    // t_3.apply_transform(test_scene.m_meshes.back());
    // test_scene.m_meshes.back().compute_normals();
    // test_scene.m_meshes.back().m_material.m_type = M_MICROFACETS;
    // test_scene.m_meshes.back().m_material.m_albedo = {0.214, 0.68, 0.83}; // Golden color
    // test_scene.m_meshes.back().m_material.m_F0 = {0.29, 0.71, 1.0};
    // // test_scene.m_meshes.back().m_material.m_type = M_BLINN_PHONG;
    // // test_scene.m_meshes.back().m_material.m_albedo = {1, 1, 1};
    // // test_scene.m_meshes.back().m_material.m_diffuse_coef = 0;
    // // test_scene.m_meshes.back().m_material.m_shininess = 32;
    // test_scene.m_meshes.back().m_material.m_noise = Worley(50, {-0.3, -0.5, -0.5}, {0.7, 1, 1});
    // test_scene.m_meshes.back().compute_BVH();
    // std::cout << "Imported model 3" << endl;
    // Create light
    LightSource rect_light = LightSource({1,-0.5,2}, {1.0,1.0,1.0}, 4, L_RECTANGLE, {0.2, 0, 0}, {0, 0.2, 0});
    test_scene.m_lights.push_back(rect_light);
    // LightSource point_light = LightSource({1,-4,0}, {1.0,0.5,0.5}, 4, L_POINT, {0.5, 0, 0}, {0, 0.5, 0});
    // test_scene.m_lights.push_back(point_light);
    LightSource ambient_light = LightSource({0,0,0}, {1,1,1}, 2, L_AMBIENT);
    test_scene.m_lights.push_back(ambient_light);
    std::cout << "Created light source" << endl;
    // Raytrace
    auto t1 = std::chrono::high_resolution_clock::now();
    test_scene.rayTrace(imtest);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << endl << "Traced rays in " << duration << "ms" << endl;
    // Shade image
    imtest.styleBlitTree("../resources/blue_scales.png", 0.05);
    // imtest.styleBlitTreeCoords();
    // imtest.linearBlur(2);
    // imtest.transferFrom();
    cout << "Shaded the image" << endl;
    // Write image
    imtest.write(output);
    std::cout << "Wrote to " << output << endl;
    return 0;
}
