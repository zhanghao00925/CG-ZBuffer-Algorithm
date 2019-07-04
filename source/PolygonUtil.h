#pragma once

#include <string>
#include <list>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;

class Polygon {
public:
    // Constructor
    Polygon();

    // Method
    string toString() const;

    float getZ(float x, float y);

    inline bool getFlag() const {
        return in;
    }

    void setFlag(bool flag) {
        in = flag;
    }

    void setPolyID(int _polyID);

    void setEquation(float _A, float _B, float _C, float _D);

    // Property
    Polygon *before;
    Polygon *next;
    int polyID;
    float A, B, C, D;
    glm::vec4 color;

private:
    bool in;
};

class Edge {
public:
    // Constructor
    explicit Edge(float _x = 0, float _dX = 0, int _yMin = 0, int _yMax = 0, Polygon *_polyPtr = nullptr);

    string toString() const;

    // Property
    float x, dX;
    int yMin, yMax;
    Polygon *polyPtr;
};

class Triangle {
public:
    Triangle(int _polyID, glm::vec3 v0, glm::vec3 v1,
             glm::vec3 v2, glm::vec4 color);

    void ApplyShading(glm::vec3 const &eye, glm::vec3 const &light);

    void ApplyModel(glm::mat4 const &model);

    void ApplyMatrix(int height, int width, glm::mat4 const &project, glm::mat4 const &view);

    void build();

    float maxX() {
        return max(max(translated[0].x, translated[1].x), translated[2].x);
    }

    float minX() {
        return min(min(translated[0].x, translated[1].x), translated[2].x);
    }

    float maxY() {
        return max(max(translated[0].y, translated[1].y), translated[2].y);
    }

    float minY() {
        return min(min(translated[0].y, translated[1].y), translated[2].y);
    }

    int polyID;
    bool bFront, bPointInside;
    glm::vec3 vertex[3];
    glm::vec3 mVertex[3];
    glm::vec4 translated[3];
    glm::vec3 normal;
    glm::vec4 color;
    Edge edge[3];
    Polygon polygon;
};