#include <iostream>
#include <cstring>
#include <ctime>
#include <math.h>

#include "Image.h"
#include "Scene.h"
#include "Transform.h"

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
    test_scene.m_cam = Camera({0, 0, 2}, {0, -M_PI/2, 0}, M_PI/4, 1.5);
    cout << "Created scene" << endl;
    Mesh test_mesh;
    // Vertex v0 = {0, 0, -1};
    // Vertex v1 = {1, 0, -1};
    // Vertex v2 = {0, 1, -1};
    // test_mesh.m_vertices.push_back(v0);
    // test_mesh.m_vertices.push_back(v1);
    // test_mesh.m_vertices.push_back(v2);
    // test_mesh.m_triangles.push_back(Triangle(&test_mesh.m_vertices[0], &test_mesh.m_vertices[1], &test_mesh.m_vertices[2]));
    test_mesh.loadOFF("../example.off");
    Transform t({0,0,0}, {1.5, 0, 0}, 1);
    t.apply_transform(test_mesh);
    test_mesh.compute_normals();
    test_scene.m_meshes.push_back(test_mesh);
    // Raytrace
    test_scene.rayTrace(imtest);
    cout << "Traced rays" << endl;
    // Write image
    imtest.write(output);
    cout << "Wrote to " << output << endl;
    return 0;
}
