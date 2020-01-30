#include <iostream>
#include <cstring>
#include <ctime>
#include <math.h>

#include "Image.h"
#include "Scene.h"

int main(int argc, char **argv) {
    int width = 450;
    int height = 300;
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
    test_scene.set_camera(Camera({0, 0, 2}, {0, -M_PI/2, 0}, M_PI/4, 1.5));
    cout << "Created scene" << endl;
    Mesh test_mesh;
    Vertex v0 = {0, 0, -1};
    Vertex v1 = {1, 0, -1};
    Vertex v2 = {0, 1, -1};
    test_mesh.add_vertex(v0);
    test_mesh.add_vertex(v1);
    test_mesh.add_vertex(v2);
    test_mesh.add_triangle(Triangle(&v0, &v1, &v2));
    test_mesh.compute_normals();
    cout << &v0 << endl;
    // test_mesh.loadOFF("../example.off");
    test_scene.add_mesh(test_mesh);
    // Raytrace
    test_scene.rayTrace(imtest);
    cout << "Traced rays" << endl;
    // Write image
    imtest.write(output);
    cout << "Wrote to " << output << endl;
    return 0;
}
