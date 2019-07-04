#pragma once

#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;

class Triangle;

class Mesh {
public:
    void LoadObject(string filename, glm::vec4 const &color, glm::mat4 const &_model, bool hasNormal = false);

    void ApplyMatrix(int height, int width, glm::mat4 const &project, glm::mat4 const &view);

    void ApplyShading(glm::vec3 const &eye, glm::vec3 const &light);

    vector<Triangle> elements;
private:
    static int polyID;
    glm::mat4 model;
};

class Scene {
public:
    vector<Mesh> elements;

    void AddMesh(string filename, glm::vec4 const &color, glm::mat4 const &_model, bool hasNormal = false);

    void ApplyMatrix(int height, int width, glm::mat4 const &project, glm::mat4 const &view);

    void ApplyShading(glm::vec3 const &eye, glm::vec3 const &light);

    glm::vec4 background;
};
