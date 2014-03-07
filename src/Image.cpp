/* 
 * Image Class
 *  - The image is implemented as the cv::Mat of OpenCV library.
 *  - The images are concatenated.
 *  - The corners and the centers are detected
 *    in the chessboard and the circle grid image.
 *  - The image is translated by the translation map.
 *  - The disparity map is computed.
 * 
 * File:   Image.cpp
 * Author: munehiro
 * 
 * Created on March 1, 2014, 12:30 AM
 */

#include "Image.h"

/**
 * Constructors and Destructor
 */
Image::Image() {
}

Image::Image(const Image& orig) {
    img = orig.img.clone();
}

Image::Image(const vector<Image>& orig) {
    concatenate(orig);
}

Image::Image(const cv::Mat& orig) {
    img = orig.clone();
}

Image::~Image() {
}

/**
 * Get the image that convert to RGB
 * @return image
 */
Image Image::rgbImage() const {
    if (img.empty()) {
        throw string("Image is empty");
    }
    cv::Mat rgb;
    if (img.channels() != 1) {
        cv::cvtColor(img, rgb, CV_BGR2RGB);
    } else {
        cv::cvtColor(img, rgb, CV_GRAY2RGB);
    }
    return Image(rgb);
}

/**
 * Concatenate the images to horizontal
 * @param images
 */
void Image::concatenate(const vector<Image>& imgs) {
    if (imgs.empty()) {
        throw string("Image is empty");
    }
    img = cv::Mat(imgs[0].img.rows, imgs[0].img.cols*imgs.size(), imgs[0].img.type());
    cv::Rect roiRect;
    for_each(imgs.begin(), imgs.end(), [&](const Image& img) {
        roiRect.width = img.width();
        roiRect.height = img.height();
        cv::Mat roi(this->img, roiRect);
        img.img.copyTo(roi);
        roiRect.x += img.width();
    });
}

/**
 * Find the corners in the chessboard images
 * @param number of rows to find
 * @param number of columns to find
 * @return chessboard corners
 */
vector<cv::Point2f> Image::findChessboardCorners(int rows, int cols) {
    if (img.empty()) {
        throw string("Image is empty");
    }
    vector<cv::Point2f> corners;
    cv::Mat gray;
    cv::cvtColor(img, gray, CV_BGR2GRAY);
    if (!cv::findChessboardCorners(gray, cvSize(cols, rows), corners,
            CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE)) {
        throw string("Could not find chessboard corners");
    }
    cv::cornerSubPix(gray, corners, cvSize(5,5), cvSize(-1,-1),
            cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 1000, 1.0e-8));
    cv::drawChessboardCorners(img, cvSize(cols, rows), corners, true);
    return corners;
}

/**
 * Find the centers in the circle grid images
 * @param number of rows to find
 * @param number of columns to find
 * @return circle grid centers
 */
vector<cv::Point2f> Image::findCircleGrid(int rows, int cols) {
    if (img.empty()) {
        throw string("Image is empty");
    }
    vector<cv::Point2f> centers;
    cv::Mat gray;
    cv::cvtColor(img, gray, CV_BGR2GRAY);
    if (!cv::findCirclesGrid(gray, cvSize(cols, rows), centers,
            cv::CALIB_CB_SYMMETRIC_GRID | cv::CALIB_CB_CLUSTERING)) {
        throw string("Could not find circle grid");
    }
    cv::drawChessboardCorners(img, cvSize(cols, rows), centers, true);
    return centers;
}

/**
 * Translate the image by using the translation map
 * @param translation map
 */
void Image::remap(const cv::Mat* rmap) {
    if (img.empty()) {
        throw string("Image is empty");
    }
    if (!rmap) {
        throw string("Map is empty");
    }
    cv::Mat src = img.clone();
    cv::remap(src, img, rmap[0], rmap[1], CV_INTER_LINEAR);
}

/**
 * Compute disparity map by using Block Matching
 * @param stereo image
 * @param ROI of the rectified image
 * @return disparity map
 */
Image Image::computeDisparityMapBM(const vector<Image>& imgs, const cv::Rect* roi) {
    if (imgs.empty()) {
        throw string("Image is empty");
    }
    if (!roi) {
        throw string("ROI is empty");
    }
    cv::StereoBM bm;
    bm.state->roi1 = roi[0];
    bm.state->roi2 = roi[1];
    bm.state->preFilterCap = 31;
    bm.state->SADWindowSize = 9;
    bm.state->minDisparity = 0;
    bm.state->numberOfDisparities = ((imgs[0].width() / 8) + 15) & -16;
    bm.state->textureThreshold = 10;
    bm.state->uniquenessRatio = 15;
    bm.state->speckleWindowSize = 100;
    bm.state->speckleRange = 32;
    bm.state->disp12MaxDiff = 1;

    cv::Mat gray[2];
    cv::cvtColor(imgs[0].img, gray[0], CV_BGR2GRAY);
    cv::cvtColor(imgs[1].img, gray[1], CV_BGR2GRAY);
    bm(gray[0], gray[1], img, CV_32F);

    cv::Mat dispGray, dispBGR;
    img.convertTo(dispGray, CV_8U, 255.0/bm.state->numberOfDisparities);
    cv::cvtColor(dispGray, dispBGR, CV_GRAY2BGR);
    return Image(dispBGR);
}

/**
 * Compute disparity map by using Semi Global Block Matching
 * @param stereo image
 * @return disparity map
 */
Image Image::computeDisparityMapSGBM(const vector<Image>& imgs) {
    if (imgs.empty()) {
        throw string("Image is empty");
    }
    int nDisp = ((imgs[0].width() / 8) + 15) & -16;
    int sadWinSize = 3;
    int ch = imgs[0].img.channels();
    cv::StereoSGBM sgbm(0, nDisp, sadWinSize,
            8*ch*sadWinSize*sadWinSize, 32*ch*sadWinSize*sadWinSize,
            1, 63, 10, 100, 32, true);
    cv::Mat disp;
    sgbm(imgs[0].img, imgs[1].img, disp);
    
    cv::Mat dispGray, dispBGR;
    disp.convertTo(img, CV_32F, 1.0/16.0);
    disp.convertTo(dispGray, CV_8U, 255.0/(nDisp*16.0));
    cv::cvtColor(dispGray, dispBGR, CV_GRAY2BGR);
    return Image(dispBGR);
}

