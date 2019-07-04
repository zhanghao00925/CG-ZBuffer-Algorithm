#include "PolygonUtil.h"

#define ENABLE_FLAT_SHADING

Edge::Edge(const float _x, const float _dX, const int _yMin, const int _yMax, Polygon *_polyPtr)
        : x(_x), dX(_dX), yMin(_yMin), yMax(_yMax), polyPtr(_polyPtr) {}

string Edge::toString() const {
    char buff[128];
    snprintf(buff, sizeof(buff), "[%.2lf|%.2lf|%d|%d]", x, dX, yMax, polyPtr->polyID);
    return string(buff);
}

Polygon::Polygon() :
        before(nullptr), next(nullptr), polyID(-1),
        A(0), B(0), C(0), color(0, 0, 0, 1), in(false) {}

string Polygon::toString() const {
    char buff[32];
    snprintf(buff, sizeof(buff), "[%d]", polyID);
    return string(buff);
}

float Polygon::getZ(float x, float y) {
    return (A * x + B * y + D);
}

void Polygon::setPolyID(const int _polyID) {
    polyID = _polyID;
}

void Polygon::setEquation(const float _A, const float _B, const float _C, const float _D) {
    A = _A;
    B = _B;
    C = _C;
    D = _D;
}

Triangle::Triangle(int _polyID, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec4 _color)
        : polyID(_polyID), bFront(true) {
    // init vertex
    vertex[0] = v0;
    vertex[1] = v1;
    vertex[2] = v2;
    // init polygon
    color = _color;
    polygon.color = _color;
    polygon.polyID = _polyID;

}

void Triangle::ApplyShading(glm::vec3 const &eye, glm::vec3 const &light) {
#ifdef ENABLE_FLAT_SHADING
    glm::vec3 lightDir = glm::normalize(light - 1.0f / 3.0f * (mVertex[0] + mVertex[1] + mVertex[2]));
    float factor = dot(normal, lightDir);
    polygon.color = abs(factor) * color;
    bFront = dot(normal, eye - 1.0f / 3.0f * (mVertex[0] + mVertex[1] + mVertex[2])) > 0;
    if (!bFront) {
        polygon.color = glm::vec4(1.f, 1.f, 1.f, 1.0f) - polygon.color;
    }
#endif
}

void Triangle::ApplyModel(glm::mat4 const &model) {
    // Apply Model Transform
    for (int i = 0; i < 3; i++) {
        mVertex[i] = model * glm::vec4(vertex[i], 1);
    }
    // Calculate Plane equation
    glm::vec3 e0 = vertex[2] - vertex[1];
    glm::vec3 e1 = vertex[0] - vertex[1];
    normal = glm::cross(e0, e1);
    normal = glm::normalize(glm::transpose(glm::inverse(glm::mat3(model))) * normal);
}

void Triangle::ApplyMatrix(int height, int width, glm::mat4 const &project, glm::mat4 const &view) {
    // Apply View and projection transform
    glm::vec4 projectVertex;
    bPointInside = false;
    for (int i = 0; i < 3; i++) {
        projectVertex = project * view * glm::vec4(mVertex[i], 1);
        if (-projectVertex.w <= projectVertex.z && projectVertex.z <= projectVertex.w
            && projectVertex.w > 0) {
            bPointInside = true;
        }
        translated[i] = projectVertex / projectVertex.w;
        translated[i].x = (translated[i].x + 1) * (height) / 2;
        translated[i].y = (translated[i].y + 1) * (width) / 2;
    }
    if (bPointInside) {
        build();
    }
}

void Triangle::build() {
    // modify edge info
    for (int i = 0; i < 3; i++) {
        int low, high;
        if (translated[i].y < translated[(i + 1) % 3].y) {
            low = i;
            high = (i + 1) % 3;
        } else {
            low = (i + 1) % 3;
            high = i;
        }
        edge[i].yMax = (int) translated[high].y - 1;
        edge[i].yMin = (int) translated[low].y;
        edge[i].dX = (translated[low].x - translated[high].x) / (translated[low].y - translated[high].y);
        edge[i].x = translated[low].x;
    }
    // Calculate Plane equation
    glm::vec3 e0 = translated[2] - translated[1];
    glm::vec3 e1 = translated[0] - translated[1];
    glm::vec3 n = glm::normalize(glm::cross(e0, e1));
    polygon.A = n.x / -n.z;
    polygon.B = n.y / -n.z;
    polygon.C = n.z / -n.z;
    polygon.D = glm::dot(-n, glm::vec3(translated[0])) / -n.z;
    // init edge
    for (auto &e : edge) {
        e.polyPtr = &polygon;
    }
}
