#include <iostream>
#include <cstring>
#include <ctime>
#include <math.h>

#include "Image.h"
#include "Scene.h"
#include "Transform.h"
#include "LightSource.h"

int main(int argc, char **argv) {
    int width = 150;
    int height = 100;
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
    test_scene.m_cam = Camera({0, 0, 4}, {0, -M_PI/2, 0}, M_PI/4, 1.5);
    cout << "Created scene" << endl;
    // Create ground
    // Mesh ground;
    // Vertex v0 = {-1, -3, -3};
    // Vertex v1 = {-1, -3, 3};
    // Vertex v2 = {-1, 3, -3};
    // Vertex v3 = {-1, 3, 3};
    // ground.m_vertices.push_back(v0);
    // ground.m_vertices.push_back(v1);
    // ground.m_vertices.push_back(v2);
    // ground.m_vertices.push_back(v3);
    // ground.m_triangles.push_back(Triangle(&ground.m_vertices[1], &ground.m_vertices[0], &ground.m_vertices[2]));
    // ground.m_triangles.push_back(Triangle(&ground.m_vertices[1], &ground.m_vertices[2], &ground.m_vertices[3]));
    // ground.compute_normals();
    // test_scene.m_meshes.push_back(ground);
    cout << "Created ground" << endl;
    // Import model
    Mesh test_mesh;
    test_mesh.loadOFF("../example.off");
    Transform t({0,0,0}, {1.5, 0, 0}, 1);
    t.apply_transform(test_mesh);
    test_mesh.compute_normals();
    test_scene.m_meshes.push_back(test_mesh);
    cout << "Imported model" << endl;
    // Create light
    LightSource light = LightSource({2,2,2}, {0.7,0.2,0.2}, 1);
    cout << "Created light source" << endl;
    // Raytrace
    test_scene.rayTrace(imtest);
    cout << "Traced rays" << endl;
    // Write image
    imtest.write(output);
    cout << "Wrote to " << output << endl;
    return 0;
}
