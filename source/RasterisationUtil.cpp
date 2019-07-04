#include "RasterisationUtil.h"
#include "SceneUtil.h"
#include "PolygonUtil.h"
#include "BufferUtil.h"

Rasterization::Rasterization(const int _height, const int _width)
        : height(_height), width(_width), IPL(nullptr),
          X0(0, 0, 0, height), X1(width, 0, 0, height) {
    ET.resize(height);

    BG.setPolyID(0);
    BG.setEquation(0, 0, 1, 1);
    X0.polyPtr = &BG;
    X1.polyPtr = &BG;
}

void Rasterization::BuildETPT(Scene &scene) {
    BG.color = scene.background;
    AEL.clear();
    // Add Background Polygon
    ET[0].emplace_back(&X0);
    ET[0].emplace_back(&X1);
    // For each mesh
    for (auto &mesh : scene.elements) {
        for (auto &triangle : mesh.elements) {
            if (triangle.minX() < width && triangle.maxX() >= 0
                && triangle.minY() < height && triangle.maxY() >= 0
                && triangle.bFront && triangle.bPointInside) {
                for (auto &e : triangle.edge) {
                    if (e.yMin < height && e.yMax >= 0 && e.yMin <= e.yMax) {
                        if (e.yMin < 0) {
                            e.x -= e.dX * e.yMin;
                            ET[0].emplace_back(&e);
                        } else {
                            ET[e.yMin].emplace_back(&e);
                        }
                    }
                }
            }
        }
    }

}

void Rasterization::insertIPL(Polygon *&polygon) {
    polygon->setFlag(true);
    polygon->next = IPL;
    if (nullptr != IPL) {
        IPL->before = polygon;
    }
    IPL = polygon;
}

void Rasterization::removeIPL(Polygon *&polygon) {
    polygon->setFlag(false);
    if (nullptr != polygon->before) {
        // It is in the middle of IPL
        polygon->before->next = polygon->next;
    } else {
        // It is the start point of IPL
        IPL = polygon->next;
    }
    // Remove from IPL
    if (nullptr != polygon->next) {
        // It isn't at the end
        polygon->next->before = polygon->before;
    }
    polygon->next = nullptr;
    polygon->before = nullptr;
}

void Rasterization::Scan(FrameBuffer &fb) {
    list<Edge*>::iterator lEdgeIt, rEdgeIt;
    for (int h = 0; h < height; h++) {
        // Insert AEL
        AEL.splice(AEL.begin(), ET[h]);
        AEL.sort(edgePtrCmp);
        // Draw Line
        lEdgeIt = AEL.begin();
        rEdgeIt = AEL.begin();
        rEdgeIt++;
        while ((*lEdgeIt) != &X0) {
            // Update IPL
            if (!(*lEdgeIt)->polyPtr->getFlag()) {
                insertIPL((*lEdgeIt)->polyPtr);
            } else {
                removeIPL((*lEdgeIt)->polyPtr);
            }
            // Find next segment
            lEdgeIt++;
            rEdgeIt++;
        }
        auto startX = (int) (*lEdgeIt)->x;
        int endX;
        while ((*lEdgeIt) != &X1) {
            // Update IPL
            if (!(*lEdgeIt)->polyPtr->getFlag()) {
                insertIPL((*lEdgeIt)->polyPtr);
            } else {
                removeIPL((*lEdgeIt)->polyPtr);
            }
            endX = (int) (*rEdgeIt)->x;
            // Skip same position point
            if (endX == startX) {
                lEdgeIt++;
                rEdgeIt++;
                continue;
            } else {
                // Find Closest Polygon
                findClosestPolygonAndDraw(startX, endX, h, fb);
                // Find next segment
                lEdgeIt++;
                rEdgeIt++;
                startX = (int) (*lEdgeIt)->x;
            }
        }
        // Update AET
        lEdgeIt = AEL.begin();
        while (AEL.end() != lEdgeIt) {
            if (h == (*lEdgeIt)->yMax) {
                lEdgeIt = AEL.erase(lEdgeIt);
            } else {
                (*lEdgeIt)->x += (*lEdgeIt)->dX;
                lEdgeIt++;
            }
        }
        // Clear IPL
        Polygon *ptr;
        while (IPL != nullptr) {
            ptr = IPL;
            IPL = IPL->next;
            ptr->before = nullptr;
            ptr->next = nullptr;
            ptr->setFlag(false);
        }
    }
}

void Rasterization::findClosestPolygonAndDraw(const int leftX, const int rightX, const int y, FrameBuffer &fb) const {
    if (leftX < rightX) {
        // Draw a segment
        Polygon *closestLeftPtr = nullptr;
        Polygon *closestRightPtr = nullptr;
        float closestLeftZ = 1.001; // Max Z
        float closestRightZ = 1.001; // Max Z
        for (auto ptr = IPL; ptr != nullptr; ptr = ptr->next) {
            float leftZ = ptr->getZ(leftX, y);
            if (-1 <= leftZ && leftZ < closestLeftZ) {
                closestLeftZ = leftZ;
                closestLeftPtr = ptr;
            }
            float rightZ = ptr->getZ(rightX, y);
            if (-1 <= rightZ && rightZ < closestRightZ) {
                closestRightZ = rightZ;
                closestRightPtr = ptr;
            }
        }
        if (closestLeftPtr == closestRightPtr && nullptr != closestLeftPtr) {
            // Without penetration
            fb.DrawHorizontalLine(y, leftX, rightX, closestLeftPtr->color);
        } else {
            // With penetration
            int mid = (leftX + rightX) / 2;
            findClosestPolygonAndDraw(leftX, mid, y, fb);
            findClosestPolygonAndDraw(mid + 1, rightX, y, fb);
        }
    } else {
        // Draw one point
        Polygon *closestPtr = nullptr;
        float closestZ = 1.001; // Max Z
        for (auto ptr = IPL; ptr != nullptr; ptr = ptr->next) {
            float z = ptr->getZ(leftX, y);
            if (-1 <= z && z < closestZ) {
                closestZ = z;
                closestPtr = ptr;
            }
        }
        if (nullptr != closestPtr) {
            fb.DrawHorizontalLine(y, leftX, rightX, closestPtr->color);
        }
    }
}

void Rasterization::debugInfo() const {
    debugIPL();
    debugET();
    debugAEL();
}

void Rasterization::debugIPL() const {
    // Show AEL info
    cout << "[IPL]->";
    auto ptr = IPL;
    while (nullptr != ptr) {
        cout << ptr->toString() << "->";
        ptr = ptr->next;
    }
    cout << "[nullptr]" << endl;
    cout << endl;
}

void Rasterization::debugAEL() const {
    // Show AEL info
    cout << "[AEL]->";
    auto constEdgeIt = AEL.cbegin();
    while (AEL.cend() != constEdgeIt) {
        cout << (*constEdgeIt)->toString() << "->";
        constEdgeIt++;
    }
    cout << "[nullptr]" << endl;
    cout << endl;
}

void Rasterization::debugET() const {
    // Show ET info
    cout << "ET" << endl;
    for (int h = height - 1; h >= 0; h--) {
        cout << "[" << h << "]->";
        // Add Edge to AEL
        auto constEdgeIt = ET[h].cbegin();
        while (ET[h].cend() != constEdgeIt) {
            cout << (*constEdgeIt)->toString() << "->";
            constEdgeIt++;
        }
        cout << "[nullptr]" << endl;
    }
    cout << endl;
}