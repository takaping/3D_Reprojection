/* 
 * Vertex Class
 *  - The 3D vertex is implemented.
 *  - The 3D vertex is configured by position, color and normal vector.
 *  - There are 2 types of color.
 *    The one is in the 0 - 255 range, the other is in the 0.0 - 1.0 range.
 * 
 * File:   Vertex.cpp
 * Author: munehiro
 * 
 * Created on March 1, 2014, 11:07 PM
 */

#include <cmath>
#include <algorithm>
#include "Vertex.h"

/**
 * Constructors and Destructor
 */
Vertex::Vertex() {
}

Vertex::Vertex(const Vertex& orig) {
    copy(orig.pos3d, orig.pos3d+3, pos3d);
    copy(orig.col3b, orig.col3b+3, col3b);
    copy(orig.col3d, orig.col3d+3, col3d);
    copy(orig.nml3d, orig.nml3d+3, nml3d);
}

Vertex::~Vertex() {
}

/**
 * Set the 3D position
 * @param x position
 * @param y position
 * @param z position
 */
void Vertex::setPosition(double x, double y, double z) {
    pos3d[0] = x; pos3d[1] = y; pos3d[2] = z;
}

/**
 * Set color in the 0 - 255 range
 * and convert in the 0.0 - 1.0 range
 * @param red value
 * @param green value
 * @param blue value
 */
void Vertex::setColor(uchar r, uchar g, uchar b) {
    col3b[0] = r; col3b[1] = g; col3b[2] = b;
    for (int i = 0; i < 3; i++) {
        col3d[i] = (double)col3b[i] / 255.0;
    }
}

/**
 * Set color in the 0.0 - 1.0 range
 * and convert in the 0 - 255 range
 * @param red value
 * @param green value
 * @param blue value
 */
void Vertex::setColor(double r, double g, double b) {
    r = (r < 0.0 ? 0.0 : (r > 1.0 ? 1.0 : r));
    g = (g < 0.0 ? 0.0 : (g > 1.0 ? 1.0 : g));
    b = (b < 0.0 ? 0.0 : (b > 1.0 ? 1.0 : b));
    col3d[0] = r; col3d[1] = g; col3d[2] = b;
    for (int i = 0; i < 3; i++) {
        col3b[i] = (uchar)(col3d[i] * 255.0);
    }
}

/**
 * Set the 3D normal vector
 * @param x vector
 * @param y vector
 * @param z vector
 */
void Vertex::setNormal(double x, double y, double z) {
    double p = sqrt(x * x + y * y + z * z);
    p = (p == 0.0 ? 1.0 : p);
    nml3d[0] = x / p; nml3d[1] = y / p; nml3d[2] = z / p;
}

