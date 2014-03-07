/* 
 * StereoCamera Class
 *  - The stereo camera is implemented.
 *  - The stereo camera is calibrated by using OpenCV Library.
 *  - The 3D point cloud is constructed from the stereo image
 *    by using OpenCV Library.
 * 
 * File:   StereoCamera.cpp
 * Author: munehiro
 * 
 * Created on March 1, 2014, 8:19 AM
 */

#include <cfloat>
#include "StereoCamera.h"
#include "Image.h"
#include "Vertex.h"

const uint StereoCamera::MINOFNIMAGES = 3;
const string StereoCamera::PARAMFILENAME("param.yml");

/**
 * Constructors and Destructor
 */
StereoCamera::StereoCamera() : maxZ(1000.0) {
}

StereoCamera::StereoCamera(const StereoCamera& orig) : maxZ(orig.maxZ) {
    copy(orig.camMat, orig.camMat+2, camMat);
    copy(orig.dstCof, orig.dstCof+2, dstCof);
    rotMat = orig.rotMat.clone();
    trnVec = orig.trnVec.clone();
    essMat = orig.essMat.clone();
    funMat = orig.funMat.clone();
    copy(orig.validRoi, orig.validRoi+2, validRoi);
}

StereoCamera::~StereoCamera() {
}

/**
 * Open the camera parameters
 * @return read or not
 */
bool StereoCamera::open() {
    cv::FileStorage fs;
    if (fs.open(PARAMFILENAME, cv::FileStorage::READ)) {
        fs["C1"] >> camMat[0];
        fs["D1"] >> dstCof[0];
        fs["C2"] >> camMat[1];
        fs["D2"] >> dstCof[1];
        fs["R"] >> rotMat;
        fs["T"] >> trnVec;
        fs["E"] >> essMat;
        fs["F"] >> funMat;
        fs.release();
        return true;
    } else {
        return false;
    }
}

/**
 * Calibrate the stereo camera
 * @param stereo images
 * @param type of calibration rig pattern
 * @param number of rows on the calibration rig pattern
 * @param number of columns on the calibration rig pattern
 * @param distance between corners or centers on the calibration rig pattern
 */
void StereoCamera::calibrate(vector<Image>* imgs, RigDialog::Pattern ptn, int rows, int cols, double dist) {
    funMat.release();
    if (imgs[0].size() != imgs[1].size() || imgs[0].size() < MINOFNIMAGES) {
        throw string("Number of image must be 3 or more");
    }
    // pre-calibration
    vector<vector<cv::Point3f>> objPts = calcObjectPoints(imgs[0].size(), rows, cols, dist);
    vector<vector<cv::Point2f>> imgPts[2];
    cv::Size imgSize(imgs[0][0].size());
    vector<cv::Mat> rvecs[2], tvecs[2];
    // calibrate the left and the right cameras
    for (int i = 0; i < 2; i++) {
        // find corners or centers of the image that taken the calibration rig
        for_each(imgs[i].begin(), imgs[i].end(), [&](Image& img) {
            vector<cv::Point2f> centers;
            if (ptn == RigDialog::Pattern::Chessboard) {
                centers = img.findChessboardCorners(rows, cols);
            } else if (ptn == RigDialog::Pattern::CircleGrid) {
                centers = img.findCircleGrid(rows, cols);
            } else {
                throw string("Invalid calibration pattern");
            }
            imgPts[i].push_back(centers);
        });
        // calibrate the left or the right cameras
        cv::calibrateCamera(objPts, imgPts[i], imgSize,
                camMat[i], dstCof[i], rvecs[i], tvecs[i], 0,
                cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 1000, 1.0e-8));
    }
    // calibrate the stereo camera
    cv::stereoCalibrate(objPts, imgPts[0], imgPts[1], camMat[0], dstCof[0],
            camMat[1], dstCof[1], imgSize, rotMat, trnVec, essMat, funMat,
            cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 1000, 1.0e-8),
            CV_CALIB_FIX_INTRINSIC);
    // save the camera parameters
    cv::FileStorage fs(PARAMFILENAME, cv::FileStorage::WRITE);
    fs << "C1" << camMat[0];
    fs << "D1" << dstCof[0];
    fs << "C2" << camMat[1];
    fs << "D2" << dstCof[1];
    fs << "R" << rotMat;
    fs << "T" << trnVec;
    fs << "E" << essMat;
    fs << "F" << funMat;
    fs.release();
}

/**
 * Calculate the position of the corners or centers on the calibration rig
 * @param number of images for calibration
 * @param number of rows on the calibration rig
 * @param number of columns on the calibration rig
 * @param distance between corners or centers on the calibration rig pattern
 * @return position of the corners or centers on the calibration rig
 */
vector<vector<cv::Point3f>> StereoCamera::calcObjectPoints(int nImgs, int rows, int cols, double dist) {
    vector<cv::Point3f> centers;
    for (int i = 0; i < rows; i++) {
        double y = (double)i * dist;
        for (int j = 0; j < cols; j++) {
            centers.push_back(cv::Point3f((double)j*dist, y, 0.0));
        }
    }
    vector<vector<cv::Point3f>> objPts;
    objPts.resize(nImgs, centers);
    return objPts;
}

/**
 * Construct the 3D point cloud from the stereo image
 * @param stereo image
 * @return 3D point cloud
 */
vector<Vertex> StereoCamera::reprojectImageTo3D(vector<Image>& imgs) {
    if (imgs.size() != 2) {
        throw string("Number of image must be 2");
    }
    // transform rectification
    cv::Mat qMat = transformRectification(imgs);
    Image disp;
    // compute the disparity map
//    imgs.push_back(disp.computeDisparityMapBM(imgs, validRoi));
    imgs.push_back(disp.computeDisparityMapSGBM(imgs));
    // construct the 3D point cloud from the disparity map
    cv::Mat _3dImg;
    cv::reprojectImageTo3D(disp.image(), _3dImg, qMat);
    // relieve the invalid data that
    // infinite, negative and out of range data
    float minZ = FLT_MAX;
    for (int i = 0; i < _3dImg.rows; i++) {
        for (int j = 0; j < _3dImg.cols; j++) {
            cv::Point3f pt = _3dImg.at<cv::Point3f>(i, j);
            if (isinf(pt.z) || pt.z <= 0.0 || pt.z > maxZ) {
                continue;
            }
            minZ = (minZ > pt.z ? pt.z : minZ);
        }
    }
    vector<Vertex> vtcs;
    for (int i = 0; i < _3dImg.rows; i++) {
        for (int j = 0; j < _3dImg.cols; j++) {
            cv::Point3f pt = _3dImg.at<cv::Point3f>(i, j);
            if (isinf(pt.z) || pt.z < minZ || pt.z > maxZ) {
                continue;
            }
            Vertex vtx;
            vtx.setPosition(pt.x, -pt.y, maxZ-pt.z);
            cv::Vec3b c = imgs[0].image().at<cv::Vec3b>(i, j);
            vtx.setColor(c(2), c(1), c(0));
            vtcs.push_back(vtx);
        }
    }
    if (vtcs.size() == 0) {
        throw string("Point cloud is empty");
    }
    return vtcs;
}

/**
 * Transform rectification
 * @param stereo image
 * @return Q matrix
 */
cv::Mat StereoCamera::transformRectification(vector<Image>& imgs) {
    cv::Size imgSize(imgs[0].size());
    cv::Mat recMat[2], prjMat[2], qMat;
    // compute stereo rectification transformation
    cv::stereoRectify(camMat[0], dstCof[0], camMat[1], dstCof[1], imgSize,
            rotMat, trnVec, recMat[0], recMat[1], prjMat[0], prjMat[1], qMat,
            cv::CALIB_ZERO_DISPARITY, -1.0, imgSize, &validRoi[0], &validRoi[1]);
    // transform rectification
    for (int i = 0; i < 2; i++) {
        cv::Mat rmap[2];
        cv::initUndistortRectifyMap(camMat[i], dstCof[i], recMat[i], prjMat[i],
                imgSize, CV_16SC2, rmap[0], rmap[1]);
        imgs[i].remap(rmap);
    }
    return qMat;
}

