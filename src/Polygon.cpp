/* 
 * Polygon Class
 *  - The 3D point cloud and the polygon mesh are implemented
 *    by Point Cloud Library (PCL).
 *  - The polygon mesh is constructed by pcl::GreedyProjectionTriangulation
 * 
 * File:   Polygon.cpp
 * Author: munehiro
 * 
 * Created on March 1, 2014, 6:31 PM
 */

#include <cfloat>
#include <cmath>
#include <pcl-1.7/pcl/kdtree/kdtree_flann.h>
#include <pcl-1.7/pcl/features/normal_3d.h>
#include <pcl-1.7/pcl/surface/gp3.h>
#include "Polygon.h"
#include "Vertex.h"

/**
 * Constructors and Destructor
 */
Polygon::Polygon() : searchRadius(5.0), mu(2.5), scl(1.0f) {
}

Polygon::Polygon(const Polygon& orig) : scl(orig.scl) {
    cloudWithNormals.reset(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
    copy(orig.cloudWithNormals->begin(), orig.cloudWithNormals->end(), cloudWithNormals->begin());
    copy(orig.min, orig.min+3, min);
    copy(orig.max, orig.max+3, max);
}

Polygon::~Polygon() {
}

/**
 * Get the normalized vertices of the point cloud
 * @return vertices
 */
vector<Vertex> Polygon::normalizedVertices() const {
    vector<Vertex> vtcs;
    for_each(cloudWithNormals->begin(), cloudWithNormals->end(), [&](const pcl::PointXYZRGBNormal& pt) {
        Vertex vtx;
        vtx.setPosition((pt.x - (min[0] + max[0]) / 2.0f) / scl,
                        (pt.y - (min[1] + max[1]) / 2.0f) / scl,
                        (pt.z - (min[2] + max[2]) / 2.0f) / scl);
        vtx.setColor(pt.r, pt.g, pt.b);
        vtx.setNormal(pt.normal_x, pt.normal_y, pt.normal_z);
        vtcs.push_back(vtx);
    });
    return vtcs;
}

/**
 * Get the indexes of the polygon mesh vertices
 * @return indexes of the polygon mesh vertices
 */
vector<vector<uint>> Polygon::faceVertexIndexes() const {
    vector<vector<uint>> faceVtxIdxs;
    for_each(triangles->polygons.begin(), triangles->polygons.end(), [&](const pcl::Vertices& vtcs) {
        vector<uint> vtxIdxs;
        for_each(vtcs.vertices.begin(), vtcs.vertices.end(), [&](const uint32_t vtx) {
            vtxIdxs.push_back(vtx);
        });
        faceVtxIdxs.push_back(vtxIdxs);
    });
    return faceVtxIdxs;
}

/**
 * Set the vertices of the point cloud
 * @param vertices
 */
void Polygon::setVertices(const vector<Vertex>& vtcs) {
    if (vtcs.size() == 0) {
        throw string("Point cloud is empty");
    }
    // set the vertices
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
    min[0] = min[1] = min[2] =  FLT_MAX;
    max[0] = max[1] = max[2] = -FLT_MAX;
    for_each(vtcs.begin(), vtcs.end(), [&](const Vertex& vtx) {
        const double* pos = vtx.position3d();
        for (int i = 0; i < 3; i++) {
            if (min[i] > pos[i]) {
                min[i] = pos[i];
            }
            if (max[i] < pos[i]) {
                max[i] = pos[i];
            }
        }
        pcl::PointXYZRGB pt;
        pt.x = pos[0];
        pt.y = pos[1];
        pt.z = pos[2];
        const uchar* col = vtx.color3b();
        pt.rgba = static_cast<uint32_t>(col[0]) << 16 |
                  static_cast<uint32_t>(col[1]) <<  8 |
                  static_cast<uint32_t>(col[2]);
        cloud->push_back(pt);
    });
    float sub[] = { max[0]-min[0], max[1]-min[1], max[2]-min[2] };
    scl = (sub[0] > sub[1] ? sub[0] : (sub[1] > sub[2] ? sub[1] : sub[2]));
    // estimate normal vectors
    pcl::NormalEstimation<pcl::PointXYZRGB, pcl::Normal> ne;
    pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
    pcl::search::KdTree<pcl::PointXYZRGB>::Ptr cTree(new pcl::search::KdTree<pcl::PointXYZRGB>);
    cTree->setInputCloud(cloud);
    ne.setInputCloud(cloud);
    ne.setSearchMethod(cTree);
    ne.setKSearch(20);
    ne.compute(*normals);
    // concatenate the vertices and normal vectors
    cloudWithNormals.reset(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
    pcl::concatenateFields(*cloud, *normals, *cloudWithNormals);
    // triangulate
    triangulate();
    if (triangles->polygons.size() == 0) {
        throw string("Surface is empty");
    }
}

/**
 * Triangulate
 */
void Polygon::triangulate() {
    // create the search tree
    pcl::search::KdTree<pcl::PointXYZRGBNormal>::Ptr cnTree(new pcl::search::KdTree<pcl::PointXYZRGBNormal>);
    cnTree->setInputCloud(cloudWithNormals);
    // initialize the objects
    pcl::GreedyProjectionTriangulation<pcl::PointXYZRGBNormal> gp3;
    triangles.reset(new pcl::PolygonMesh);
    // set the typical values for the parameters
    gp3.setSearchRadius(searchRadius);
    gp3.setMu(mu);
    gp3.setMaximumNearestNeighbors(100);
    gp3.setMaximumSurfaceAngle(M_PI/4.0);
    gp3.setMinimumAngle(M_PI/18.0);
    gp3.setMaximumAngle(2.0*M_PI/3.0);
    gp3.setNormalConsistency(false);
    // get the result
    gp3.setInputCloud(cloudWithNormals);
    gp3.setSearchMethod(cnTree);
    gp3.reconstruct(*triangles);
}

