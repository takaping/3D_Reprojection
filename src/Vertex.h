/* 
 * Vertex Class
 *  - The 3D vertex is implemented.
 *  - The 3D vertex is configured by position, color and normal vector.
 *  - There are 2 types of color.
 *    The one is in the 0 - 255 range, the other is in the 0.0 - 1.0 range.
 * 
 * File:   Vertex.h
 * Author: munehiro
 *
 * Created on March 1, 2014, 11:07 PM
 */

#ifndef VERTEX_H
#define	VERTEX_H

typedef unsigned char uchar;

using namespace std;

class Vertex {
public:
    Vertex();
    Vertex(const Vertex& orig);
    virtual ~Vertex();
    const double* position3d() const { return pos3d; }; // get the position
    const uchar* color3b() const { return col3b; };     // get the color in the 0 - 255 range
    const double* color3d() const { return col3d; };    // get the color in the 0.0 - 1.0 range
    const double* normal3d() const { return nml3d; };   // get the normal vector
    void setPosition(double x, double y, double z);
    void setColor(uchar r, uchar g, uchar b);
    void setColor(double r, double g, double b);
    void setNormal(double x, double y, double z);
private:
    double pos3d[3];    // position
    uchar col3b[3];     // color in the 0 - 255 range
    double col3d[3];    // color in the 0.0 - 1.0 range
    double nml3d[3];    // normal vector

};

#endif	/* VERTEX_H */

