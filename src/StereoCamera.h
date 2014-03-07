/* 
 * StereoCamera Class
 *  - The stereo camera is implemented.
 *  - The stereo camera is calibrated by using OpenCV Library.
 *  - The 3D point cloud is constructed from the stereo image
 *    by using OpenCV Library.
 * 
 * File:   StereoCamera.h
 * Author: munehiro
 *
 * Created on March 1, 2014, 8:19 AM
 */

#ifndef STEREOCAMERA_H
#define	STEREOCAMERA_H

#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include "RigDialog.h"

using namespace std;

class Image;
class Vertex;

class StereoCamera {
public:
    // minimum the number of images required for calibration
    static const uint MINOFNIMAGES;
    StereoCamera();
    StereoCamera(const StereoCamera& orig);
    virtual ~StereoCamera();
    // verify whether the stereo camera is calibrated
    bool isValid() const { return !funMat.empty(); };
    bool open();
    void calibrate(vector<Image>* imgs, RigDialog::Pattern ptn, int rows, int cols, double dist);
    vector<Vertex> reprojectImageTo3D(vector<Image>& imgs);
private:
    // file name for camera parameters
    static const string PARAMFILENAME;
    vector<vector<cv::Point3f>> calcObjectPoints(int nImgs, int rows, int cols, double dist);
    cv::Mat transformRectification(vector<Image>& imgs);
    // camera intrinsic parameters and distortion coefficients
    cv::Mat camMat[2], dstCof[2];
    // rotation matrix and translation vector between the left and the right cameras,
    // essential matrix and fundamental matrix
    cv::Mat rotMat, trnVec, essMat, funMat;
    cv::Rect validRoi[2];   // ROI of the rectified image
    double maxZ;            // maximum range of the z axis 

};

#endif	/* STEREOCAMERA_H */

