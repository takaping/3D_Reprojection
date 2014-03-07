/* 
 * Polygon Class
 *  - The 3D point cloud and the polygon mesh are implemented
 *    by Point Cloud Library (PCL).
 *  - The polygon mesh is constructed by pcl::GreedyProjectionTriangulation
 * 
 * File:   Polygon.h
 * Author: munehiro
 *
 * Created on March 1, 2014, 6:31 PM
 */

#ifndef POLYGON_H
#define	POLYGON_H

#include <vector>
#include <pcl-1.7/pcl/point_types.h>
#include <pcl-1.7/pcl/point_cloud.h>
#include <pcl-1.7/pcl/PolygonMesh.h>

typedef unsigned char uchar;
typedef unsigned int uint;

using namespace std;

class Vertex;

class Polygon {
public:
    Polygon();
    Polygon(const Polygon& orig);
    virtual ~Polygon();
    // verify whether the 3D point cloud is not empty
    bool isValid() const { return (cloudWithNormals && !cloudWithNormals->empty()); };
    vector<Vertex> normalizedVertices() const;
    vector<vector<uint>> faceVertexIndexes() const;
    void setVertices(const vector<Vertex>& vtcs);
private:
    void triangulate();
    // 3D point cloud
    pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr cloudWithNormals;
    pcl::PolygonMesh::Ptr triangles;    // polygon mesh
    // the sphere radius to be used for triangulating
    // and the multiplier of the final search radius
    double searchRadius, mu;
    // the minimum and the maximum range of x, y and z axis
    // and the scale to normalize the point cloud
    float min[3], max[3], scl;

};

#endif	/* POLYGON_H */

