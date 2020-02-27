#include <iostream>
#include <cstring>
#include <ctime>
#include <math.h>

#include "Image.h"
#include "Scene.h"
#include "Transform.h"
#include "LightSource.h"
#include "Worley.h"

int main(int argc, char **argv) {
    int width = 50;
    int height = 75;
    int n_samples = 4;
    char *output = (char *)"out.ppm";
    // Parse arguments
    for (int i = 1; i < argc - 1; i++) {
        if (!strcmp(argv[i], "-width"))
            width = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-height"))
            height = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-output"))
            output = (char *)argv[i + 1];
    }
    // Create image
    Image imtest = Image(width, height);
    cout << "Made an image of size " << imtest.m_width << "x" << imtest.m_height << endl;
    // Fill background
    imtest.fillBackground({0, 0, 0}, {1, 1, 1});
    cout << "Filled bg" << endl;
    // Create scene
    Scene test_scene;
    test_scene.m_n_samples = n_samples;
    test_scene.m_cam = Camera({0, 0, 3}, {0, -M_PI/2, 0}, M_PI/4, 0.6666f);
    cout << "Created scene" << endl;
    // Create ground
    Mesh ground;
    Vertex v0 = {-0.3, -3, -3};
    Vertex v1 = {-0.3, -3, 3};
    Vertex v2 = {-0.3, 3, -3};
    Vertex v3 = {-0.3, 3, 3};
    ground.m_vertices.push_back(v0);
    ground.m_vertices.push_back(v1);
    ground.m_vertices.push_back(v2);
    ground.m_vertices.push_back(v3);
    ground.m_triangles.push_back(Triangle(&ground.m_vertices[1], &ground.m_vertices[0], &ground.m_vertices[2]));
    ground.m_triangles.push_back(Triangle(&ground.m_vertices[1], &ground.m_vertices[2], &ground.m_vertices[3]));
    for (Vertex &v : ground.m_vertices) {
        v.m_normal = {1, 0, 0};
    }
    // ground.compute_normals();
    test_scene.m_meshes.push_back(ground);
    cout << "Created ground" << endl;
    // Import model
    Mesh test_mesh;
    test_mesh.loadOFF("../example.off");
    Transform t({0,0,0}, {1.5, 3, 3}, 1);
    t.apply_transform(test_mesh);
    test_mesh.compute_normals();
    test_mesh.m_material.m_type = M_MICROFACETS;
    test_mesh.m_material.m_noise = Worley(50, {-0.3, -0.5, -0.5}, {0.7, 1, 1});
    test_scene.m_meshes.push_back(test_mesh);
    cout << "Imported model" << endl;
    // Import model 2
    // Mesh test_mesh_2;
    // test_mesh_2.loadOFF("../EiffelTower.off");
    // Transform t_2({0.5,0,0}, {-0.3, -0.5, -0.5}, 0.02);
    // t_2.apply_transform(test_mesh_2);
    // test_mesh_2.compute_normals();
    // test_mesh_2.m_material.m_type = M_MICROFACETS;
    // test_mesh_2.m_material.m_noise = Worley(50, {-0.3, -0.5, -0.5}, {0.7, 1, 1});
    // test_scene.m_meshes.push_back(test_mesh_2);
    // cout << "Imported model 2" << endl;
    // Create light
    LightSource point_light = LightSource({1,2,2}, {1.0,1.0,1.0}, 4, L_RECTANGLE);
    test_scene.m_lights.push_back(point_light);
    LightSource ambient_light = LightSource({0,0,0}, {1,1,1}, 2, L_AMBIENT);
    test_scene.m_lights.push_back(ambient_light);
    cout << "Created light source" << endl;
    // Raytrace
    test_scene.rayTrace(imtest);
    cout << "Traced rays" << endl;
    // Write image
    imtest.write(output);
    cout << "Wrote to " << output << endl;
    return 0;
}
