/* 
 * GraphicsModel Class
 *  - The model of the model/view architecture is implemented.
 *  - Subject class is inherited.
 *  - The Image, StereoCamera and Polygon objects are instantiated.
 * 
 * File:   GraphicsModel.h
 * Author: munehiro
 *
 * Created on February 28, 2014, 6:32 PM
 */

#ifndef GRAPHICSMODEL_H
#define	GRAPHICSMODEL_H

#include <memory>
#include <vector>
#include "Subject.h"
#include "RigDialog.h"

using namespace std;

class Image;
class StereoCamera;
class Polygon;

class GraphicsModel : public Subject {
public:
    GraphicsModel();
    virtual ~GraphicsModel();
    void open(const string& fn);
    void calibrate(const vector<string>& fns, RigDialog::Pattern ptn, int rows, int cols, double dist);
    // get the image
    Image* image() const { return img.get(); };
    // get the polygon
    Polygon* polygon() const { return ply.get(); };
private:
    shared_ptr<Image> img;          // image
    shared_ptr<StereoCamera> sCam;  // stereo camera
    shared_ptr<Polygon> ply;        // 3D polygon

};

#endif	/* GRAPHICSMODEL_H */

