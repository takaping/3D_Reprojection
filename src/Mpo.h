/* 
 * Mpo Class
 *  - The MPO file is opened and the JPEG is extracted.
 * 
 * File:   Mpo.h
 * Author: munehiro
 *
 * Created on February 28, 2014, 8:44 PM
 */

#ifndef MPO_H
#define	MPO_H

#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;

class Image;

class Mpo {
public:
    Mpo();
    virtual ~Mpo();
    vector<Image> open(const string& fn) const;
private:
    static const int COUNTSIZE;         // byte of MP entry's count
    static const int FIELDSIZE;         // byte of MP entry field
    static const int ENTRYSIZE;         // byte of MP entry
    static const int NIMAGESTAG;        // tag of image number
    static const int MPENTRYTAG;        // tag of MP entry
    static const int NOTJPEG;           // not JPEG flag
    static const uchar SOI[];           // start of image
    static const uchar APP1MARKER[];    // APP1 marker
    static const uchar APP2MARKER[];    // APP2 marker
    static const uchar APP2IDCODE[];    // APP2 ID code
    static const uchar EXIFIDCODE[];    // EXIF ID code
    static const uchar BIGENDCODE[];    // big endian ID code
    const uchar* mpHeader(const uchar* mpo, ulong mpoSize) const;
    vector<Image> extractJpeg(const uchar* mpo, const uchar* mpHead) const;
    // convert to little endian
    ushort toLtlEnd(const ushort* val) const {
        return (*val >> 8 | *val << 8);
    };
    // convert to little endian
    ulong  toLtlEnd(const ulong* val) const {
        return (*val >> 24 |
               (*val >> 8 & 0x0000ff00) |
               (*val << 8 & 0x00ff0000) |
                *val << 24);
    };

};

#endif	/* MPO_H */

