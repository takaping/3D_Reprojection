/* 
 * GraphicsModel Class
 *  - The model of the model/view architecture is implemented.
 *  - Subject class is inherited.
 *  - The Image, StereoCamera and Polygon objects are instantiated.
 * 
 * File:   GraphicsModel.cpp
 * Author: munehiro
 * 
 * Created on February 28, 2014, 6:32 PM
 */

#include "GraphicsModel.h"
#include "Mpo.h"
#include "Image.h"
#include "StereoCamera.h"
#include "Polygon.h"
#include "Vertex.h"

/**
 * Constructor and Destructor
 */
GraphicsModel::GraphicsModel() {
    sCam.reset(new StereoCamera);
    if (!sCam->open()) {
        sCam.reset();
    }
}

GraphicsModel::~GraphicsModel() {
}

/**
 * Open the MPO file
 * @param file name
 */
void GraphicsModel::open(const string& fn) {
    this->img.reset();
    ply.reset();
    Mpo mpo;
    vector<Image> img = mpo.open(fn);
    if (img.size() != 2) {
        throw string("Number of image must be 2");
    }
    // construct the 3D polygon, if the stereo camera is calibrated
    if (sCam && sCam->isValid()) {
        ply.reset(new Polygon);
        ply->setVertices(sCam->reprojectImageTo3D(img));
    }
    this->img.reset(new Image(img));
    notify();
}

/**
 * Calibrate the stereo camera
 * @param file names
 * @param type of the calibration rig pattern
 * @param number of rows on the calibration rig pattern
 * @param number of columns on the calibration rig pattern
 * @param distance corners or centers on the calibration rig pattern
 */
void GraphicsModel::calibrate(const vector<string>& fns, RigDialog::Pattern ptn, int rows, int cols, double dist) {
    img.reset();
    sCam.reset();
    if (fns.size() < StereoCamera::MINOFNIMAGES) {
        throw string("Number of MPO must be 3 or more");
    }
    // open the calibration rig images
    Mpo mpo;
    vector<Image> imgs[2];
    for_each(fns.begin(), fns.end(), [&](const string& fn) {
        vector<Image> img = mpo.open(fn);
        if (img.size() != 2) {
            throw string("Number of image must be 2");
        }
        imgs[0].push_back(img[0]);
        imgs[1].push_back(img[1]);
    });
    sCam.reset(new StereoCamera);
    // calibrate the stereo camera
    sCam->calibrate(imgs, ptn, rows, cols, dist);
    img.reset(new Image(imgs[0]));
    notify();
}

