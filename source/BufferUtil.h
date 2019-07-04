#pragma once

#include <vector>
#include <fstream>

using namespace std;

#include "glm/glm.hpp"

#ifndef DEBUG

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
#endif


class FrameBuffer {
public:
    // Constructor
    FrameBuffer(unsigned long _height, unsigned long _width);

    // Method
    void DrawHorizontalLine(int h, int startW, int endW, glm::vec4 const &color);

    void Render();

#ifndef DEBUG
    Mat frame;
#endif

private:
    // Property
    int height, width;
#ifdef DEBUG
    vector<vector<glm::vec4>> colorBuffer;
#endif
};