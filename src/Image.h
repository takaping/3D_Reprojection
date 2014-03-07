/* 
 * Image Class
 *  - The image is implemented as the cv::Mat of OpenCV library.
 *  - The images are concatenated.
 *  - The corners and the centers are detected
 *    in the chessboard and the circle grid image.
 *  - The image is translated by the translation map.
 *  - The disparity map is computed.
 * 
 * File:   Image.h
 * Author: munehiro
 *
 * Created on March 1, 2014, 12:30 AM
 */

#ifndef IMAGE_H
#define	IMAGE_H

#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;

class Image {
public:
    Image();
    Image(const Image& orig);
    Image(const vector<Image>& orig);
    Image(const cv::Mat& orig);
    virtual ~Image();
    // verify whether image is empty
    bool isEmpty() const { return img.empty(); };
    // get the width
    int width() const { return img.cols; };
    // get the height
    int height() const { return img.rows; };
    // get size
    cv::Size size() const { return img.size(); };
    // get the row stride
    int rowstride() const { return (img.cols * img.channels()); };
    // get data
    uchar* data() const { return img.data; };
    // get image as the cv::Mat
    const cv::Mat& image() const { return img; };
    Image rgbImage() const;
    void concatenate(const vector<Image>& imgs);
    vector<cv::Point2f> findChessboardCorners(int rows, int cols);
    vector<cv::Point2f> findCircleGrid(int rows, int cols);
    void remap(const cv::Mat* rmap);
    Image computeDisparityMapBM(const vector<Image>& imgs, const cv::Rect* roi);
    Image computeDisparityMapSGBM(const vector<Image>& imgs);
private:
    cv::Mat img;    // image

};

#endif	/* IMAGE_H */

