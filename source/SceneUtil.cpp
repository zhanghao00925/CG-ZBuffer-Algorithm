#include "SceneUtil.h"
#include "PolygonUtil.h"
#include <fstream>
#include <vector>
#include <iostream>

int Mesh::polyID = 1;

void Mesh::LoadObject(const string filename, glm::vec4 const &color, glm::mat4 const &_model, bool hasNormal) {
    model = _model;
    elements.clear();
    // Read .obj file
    std::ifstream ifs(filename, std::ifstream::in);
    if (ifs.is_open()) {
        // Add a new mesh
        vector<glm::vec3> vertex;
        string inputType;
        int a, b, c, redunant;
        double x, y, z;
        while (!ifs.eof()) {
            ifs >> inputType;
            if (inputType == "v") {
                ifs >> x >> y >> z;
                vertex.emplace_back(x, y, z);
            } else if (inputType == "f") {
                if (!hasNormal) {
                    ifs >> a >> b >> c;
                } else {
                    ifs >> a >> redunant >> b >> redunant >> c >> redunant;
                }
                elements.emplace_back(polyID++, vertex[a - 1], vertex[b - 1], vertex[c - 1], color);
            } else if (inputType == "vn") {
                ifs >> x >> y >> z;
            }
        }
    } else {
        cerr << "Scene::AddObject Cannot open file." << endl;
    }
    ifs.close();

    // Apply model transform
    for (auto &e : elements) {
        e.ApplyModel(model);
    }
}

void Mesh::ApplyShading(glm::vec3 const &eye, glm::vec3 const &light) {
    for (auto &e : elements) {
        e.ApplyShading(eye, light);
    }
}

void Mesh::ApplyMatrix(int height, int width, glm::mat4 const &project, glm::mat4 const &view) {
    for (auto &e : elements) {
        e.ApplyMatrix(height, width, project, view);
    }
}

void Scene::AddMesh(string filename, glm::vec4 const &color, glm::mat4 const &_model, bool hasNormal) {
    Mesh mesh;
    elements.push_back(mesh);
    elements[elements.size() - 1].LoadObject(filename, color, _model, hasNormal);
}

void Scene::ApplyShading(glm::vec3 const &eye, glm::vec3 const &light) {
    for (auto &e : elements) {
        e.ApplyShading(eye, light);
    }
}

void Scene::ApplyMatrix(int height, int width, glm::mat4 const &project, glm::mat4 const &view) {
    for (auto &e : elements) {
        e.ApplyMatrix(height, width, project, view);
    }
}