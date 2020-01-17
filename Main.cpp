#include <iostream>
#include "Image.h"
#include <cstring>
#include <ctime>



int main(int argc, char **argv) {
    int width = 300;
    int height = 500;
    char *output = (char *)"out.ppm";
    for (int i = 1; i < argc - 1; i++) {
        if (!strcmp(argv[i], "-width"))
            width = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-height"))
            height = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-output"))
            output = (char *)argv[i + 1];
    }
    Image *imtest = new Image(width, height);
    cout << "Made an image of size " << imtest->m_width << "x" << imtest->m_height << endl;
    clock_t start_t = clock();
    imtest->fillBackground({1.0, 0.0, 0.5}, {0.0, 0.5, 0.5});
    clock_t end_t = clock();
    cout << "Filled bg" << endl;
    imtest->write(output);
    cout << "Wrote to " << output << endl;
    cout << "Time taken: " << double(end_t - start_t) / double(CLOCKS_PER_SEC) << " sec" << endl;
    return 0;
}
