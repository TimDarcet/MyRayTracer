MyRayTracer - An INF584 project
===============================
This is a complete implementation of a path tracer, plus an implementation of [StyleBlit](https://dcgi.fel.cvut.cz/home/sykorad/styleblit.html), done as part of the course INF585 - Image synthesis, at École polytechnique.

# Runnning the project
Running the project requires cmake 3.13. If it is not installed (such as on École polytechnique's lab rooms), one should install it before trying to compile this project.
## If cmake>=3.13 is installed
```
git clone https://github.com/TimDarcet/MyRayTracer.git
cd MyRayTracer
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
./MyRayTracer
```
## If cmake>=3.13 is not already installed
The following commands will fetch cmake 3.17 binary release (latest release as of 31/03/2019), and use the binary downloaded instead of system-wide cmake.
```
wget https://github.com/Kitware/CMake/releases/download/v3.17.0/cmake-3.17.0-Linux-x86_64.tar.gz
tar xvf cmake-3.17.0-Linux-x86_64.tar.gz
git clone https://github.com/TimDarcet/MyRayTracer.git
cd MyRayTracer
mkdir build
cd build
~/cmake-3.17.0-Linux-x86_64/bin/cmake -DCMAKE_BUILD_TYPE=Release ..
make
./MyRayTracer
```
