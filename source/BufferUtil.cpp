#include "BufferUtil.h"

FrameBuffer::FrameBuffer(const unsigned long _height, const unsigned long _width)
        : height(_height), width(_width) {
#ifdef DEBUG
    colorBuffer.resize(_height);
    for (unsigned long h = 0; h < _height; h++) {
        colorBuffer[h].resize(_width);
    }
#else
    frame = Mat(Size(width, height), CV_8UC3, Scalar(0, 0, 0));
#endif
}

void FrameBuffer::DrawHorizontalLine(const int h, const int startW, const int endW, glm::vec4 const &color) {
#ifdef DEBUG
    std::fill_n(colorBuffer[h].begin() + startW, endW - startW, color);
#else
    cv::line(frame, Point(startW, height - h - 1), Point(endW + 1, height - h - 1),
             Scalar(int(255.99 * color.b), int(255.99 * color.g), int(255.99 * color.r)), 1, 0);
#endif
}

void FrameBuffer::Render() {
#ifdef DEBUG
    ofstream ofs("render.ppm");
    ofs << "P3\n"
        << width << " " << height << "\n255\n";
    for (int h = height - 1; h >= 0; h--) {
        for (int w = 0; w < width; w++) {
            auto ir = int(255.99 * colorBuffer[h][w].x);
            auto ig = int(255.99 * colorBuffer[h][w].y);
            auto ib = int(255.99 * colorBuffer[h][w].z);
            ofs << ir << " " << ig << " " << ib << "\n";
        }
    }
    ofs.close();
#endif
}