#pragma once

#include <map>
#include <list>
#include <vector>
#include <iostream>
#include "PolygonUtil.h"

using namespace std;

class Scene;

class FrameBuffer;

// Record Scan Line
class Rasterization {
public:
    // Constructor
    Rasterization(int _height, int _width);

    // Method
    void BuildETPT(Scene &scene);

    void Scan(FrameBuffer &fb);

private:

    void insertIPL(Polygon *&polygon);

    void removeIPL(Polygon *&polygon);

    void findClosestPolygonAndDraw(int leftX, int rightX, int y, FrameBuffer &fb) const;

    void debugInfo() const;

    void debugIPL() const;

    void debugAEL() const;

    void debugET() const;

    inline static bool edgePtrCmp(Edge *&a, Edge *&b) {
        return a->x < b->x;
    }

    int height, width;

    // Property
    vector<list<Edge *>> ET;
    list<Edge *> AEL;
    Polygon *IPL;

    // Background
    Polygon BG;
    Edge X0, X1;
};