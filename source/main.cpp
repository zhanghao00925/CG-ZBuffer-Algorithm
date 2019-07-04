#include <iostream>
#include <ctime>

#include "glm/glm.hpp"

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "RasterisationUtil.h"
#include "SceneUtil.h"
#include "PolygonUtil.h"
#include "BufferUtil.h"

#define PI 3.1415926f

using namespace std;
using namespace cv;

static void usage() {
    cerr << "Usage : Computer-Graphics chaos.txt" << endl;
}

static float Yaw = -90.0;
static float Pitch = 0.0;

static glm::vec3 eye(3, 3, 12);
static glm::vec3 Up(0, 1, 0), worldUp(0, 1, 0);
static glm::vec3 Front, front;
static glm::vec3 Right;

static Point prevPt(-1, -1);

static void onMouse(int event, int x, int y, int flags, void *) {
    if (event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON))
        prevPt = Point(-1, -1);
    else if (event == EVENT_LBUTTONDOWN)
        prevPt = Point(x, y);
    else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON)) {
        Point pt(x, y);
        if (prevPt.x < 0)
            prevPt = pt;

        Pitch += 0.1 * (prevPt.y - pt.y);
        Yaw += 0.1 * (pt.x - prevPt.x);
        Pitch = std::min(Pitch, 89.f);
        Pitch = std::max(Pitch, -89.f);
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = normalize(front);
        Right = normalize(cross(front, worldUp));
        Up = normalize((cross(Right, front)));

        prevPt = pt;
    }
}

void readScene(string filename, Scene &scene) {
    std::ifstream ifs(filename, std::ifstream::in);
    if (ifs.is_open()) {
        string inputType;
        string meshpath;
        glm::mat4 model;
        glm::vec4 color;
        float x, y, z, w;
        while (!ifs.eof()) {
            ifs >> inputType;
            if (inputType == "background") {
                ifs >> x >> y >> z >> w;
                scene.background = glm::vec4(x, y, z, w);
            } else if (inputType == "mesh") {
                ifs >> meshpath;
                model = glm::mat4(1.0f);
                color = glm::vec4(1.0, 0.0, 0.0, 1.0);
            } else if (inputType == "end") {
                scene.AddMesh(meshpath, color, model);
            } else if (inputType == "scale") {
                ifs >> x >> y >> z;
                model = glm::scale(model, glm::vec3(x, y, z));
            } else if (inputType == "translate") {
                ifs >> x >> y >> z;
                model = glm::translate(model, glm::vec3(x, y, z));
            } else if (inputType == "rotate") {
                ifs >> x >> y >> z >> w;
                model = glm::rotate(model, x * PI / 180.0f, glm::vec3(y, z, w));
            } else if (inputType == "color") {
                ifs >> x >> y >> z >> w;
                color = glm::vec4(x, y, z, w);
            } else {
                cerr << "Wrong Scene Format!" << endl;
            }

        }
    } else {
        cerr << "Cannot open scene file : " << filename << endl;
    }
    ifs.close();

}

int main(int argc, char **argv) {
    string filename;
    if (argc != 2) {
        usage();
        exit(1);
    } else {
        filename = string(argv[1]);
    }
#ifndef DEBUG
    // print a welcome message
    cout << "\nThis is a Demo for Computer Graphic Course 2018\n" <<
         "By Hao Zhang\nOpenCV " << CV_VERSION << endl;
#endif
    // Init windows
    namedWindow("Computer Graphics");
    cv::setMouseCallback("Computer Graphics", onMouse, nullptr);
    const int height = 720;
    const int width = 1080;
    glm::mat4 projection = glm::perspective(45.0f, (float) height / (float) width, 0.5f, 100.0f);
    // Init camera
    Pitch = std::min(Pitch, 89.f);
    Pitch = std::max(Pitch, -89.f);
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = normalize(front);
    Right = normalize(cross(front, worldUp));
    Up = normalize((cross(Right, front)));
    // Read scene

    // Bunny Mesh
    Scene scene;
    readScene(filename, scene);
    // Construct Matrix
    // Construct Buffer and Rasterization
    FrameBuffer fb(height, width);
    Rasterization raster(height, width);
    // Scanline
#ifndef DEBUG
    clock_t frameClock;
    while (true) {
        frameClock = clock();
#endif
        // Update Matrix
        glm::mat4 view = glm::lookAt(eye, eye + Front, Up);
        scene.ApplyMatrix(height, width, projection, view);
        scene.ApplyShading(eye, eye);
        // Render
        raster.BuildETPT(scene);
        raster.Scan(fb);
        // Show result
#ifdef DEBUG
        fb.Render();
#else
        imshow("Computer Graphics", fb.frame);
        printf("\rfps:%.2f", (float) CLOCKS_PER_SEC / (clock() - frameClock));
        // Update View
        auto c = (char) waitKey(1);
        if (c == 27)
            break;
        switch (c) {
            case 'w':
                eye += 0.4f * Front;
                break;
            case 's':
                eye -= 0.4f * Front;
                break;
            case 'a':
                eye -= 0.1f * Right;
                break;
            case 'd':
                eye += 0.1f * Right;
                break;
        }
    }
#endif

    // Closes all the windows
    cv::destroyAllWindows();
    return 0;
}

