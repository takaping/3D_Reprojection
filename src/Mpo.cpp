/* 
 * Mpo Class
 *  - The MPO file is opened and the JPEG data is extracted.
 * 
 * File:   Mpo.cpp
 * Author: munehiro
 * 
 * Created on February 28, 2014, 8:44 PM
 */

#include <fstream>
#include <boost/shared_array.hpp>
#include "Mpo.h"
#include "Image.h"

const int Mpo::COUNTSIZE =  2;
const int Mpo::FIELDSIZE = 12;
const int Mpo::ENTRYSIZE = 16;
const int Mpo::NIMAGESTAG = 0xb001;
const int Mpo::MPENTRYTAG = 0xb002;
const int Mpo::NOTJPEG    = 0x0007;
const uchar Mpo::SOI[]        = { 0xff, 0xd8 };
const uchar Mpo::APP1MARKER[] = { 0xff, 0xe1 };
const uchar Mpo::EXIFIDCODE[] = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
const uchar Mpo::APP2MARKER[] = { 0xff, 0xe2 };
const uchar Mpo::APP2IDCODE[] = { 0x4d, 0x50, 0x46, 0x00 };
const uchar Mpo::BIGENDCODE[] = { 0x4d, 0x4d, 0x00, 0x2a };

/**
 * Constructor and Destructor
 */
Mpo::Mpo() {
}

Mpo::~Mpo() {
}

/**
 * Open the MPO file
 * @param file name
 * @return images as the Image object
 */
vector<Image> Mpo::open(const string& fn) const {
    // open the MPO file
    ifstream file(fn.c_str(), ios::in | ios::binary);
    if (!file) {
        string msg("Could not open ");
        msg += fn;
        throw msg;
    }
    ulong mpoSize = file.seekg(0, ios::end).tellg();
    file.seekg(0, ios::beg);
    boost::shared_array<uchar> mpo(new uchar[mpoSize]);
    file.read((char*)mpo.get(), mpoSize);
    file.close();
    // extract the JPEG data and convert to the Image object
    return extractJpeg(mpo.get(), mpHeader(mpo.get(), mpoSize));
}

/**
 * Get the MP header
 * @param MPO data
 * @param MPO data size
 * @return MP header
 */
const uchar* Mpo::mpHeader(const uchar* mpo, ulong mpoSize) const {
    if (mpoSize == 0L || !mpo) {
        throw string("MPO is empty");
    }
    // verify APP1
    if (strncmp((char*)&mpo[0], (char*)SOI, 2) != 0        ||
        strncmp((char*)&mpo[2], (char*)APP1MARKER, 2) != 0 ||
        strncmp((char*)&mpo[6], (char*)EXIFIDCODE, 6) != 0) {
        throw string("Invalid MPO");
    }
    ushort app1Size = sizeof(APP1MARKER) + toLtlEnd((ushort*)&mpo[4]);
    // verify APP2
    const uchar* app2 = &mpo[sizeof(SOI)+app1Size];
    if (strncmp((char*)app2, (char*)APP2MARKER, 2) != 0 ||
        strncmp((char*)(app2+4), (char*)APP2IDCODE, 4) != 0) {
        throw string("Invalid MPO");
    }
    return (app2 + 8);
}

/**
 * Extract the JPEG data and convert to the Image object
 * @param MPO data
 * @param MP Header
 * @return images as the Image object
 */
vector<Image> Mpo::extractJpeg(const uchar* mpo, const uchar* mpHead) const {
    // verify MP
    bool bigEnd = (strncmp((char*)mpHead, (char*)BIGENDCODE, 4) == 0 ? true : false);
    const uchar* mpIdxIfd = mpHead + (bigEnd ? toLtlEnd((ulong*)(mpHead+4)) : *(ulong*)(mpHead+4));
    const ushort count = (bigEnd ? toLtlEnd((ushort*)mpIdxIfd) : *(ushort*)mpIdxIfd);
    const uchar* mpIdx = mpIdxIfd + COUNTSIZE;
    ulong mpEntryOffset = 0L;
    ulong nJpgs = 0L;
    for (int i = 0; i < count; i++, mpIdx += FIELDSIZE) {
        ushort tag = (bigEnd ? toLtlEnd((ushort*)mpIdx) : *(ushort*)mpIdx);
        if (tag == NIMAGESTAG) {
            nJpgs = (bigEnd ? toLtlEnd((ulong*)(mpIdx+8)) : *(ulong*)(mpIdx+8));
        } else if (tag == MPENTRYTAG) {
            mpEntryOffset = (bigEnd ? toLtlEnd((ulong*)(mpIdx+8)) : *(ulong*)(mpIdx+8));
        }
    }
    // extract the JPEG data and convert to the Image object
    vector<Image> jpgs;
    for (uint i = 0; i < (uint)nJpgs; i++) {
        const uchar* mpEntries = mpHead+mpEntryOffset;
        const uchar* mpEntry = mpEntries + ENTRYSIZE * i;
        if (*mpEntry & NOTJPEG) {
            throw string("Invalid JPEG");
        }
        ulong jpgSize = (bigEnd ? toLtlEnd((ulong*)(mpEntry+4)) : *(ulong*)(mpEntry+4));
        ulong offset = (bigEnd ? toLtlEnd((ulong*)(mpEntry+8)) : *(ulong*)(mpEntry+8));
        const uchar* jpgPtr = (i == 0 ? mpo : mpHead+offset);
        vector<uchar> jpg;
        jpg.reserve(jpgSize);
        copy(jpgPtr, jpgPtr+jpgSize, back_inserter(jpg));
        jpgs.push_back(Image(cv::imdecode(jpg, CV_LOAD_IMAGE_COLOR)));
    }
    return jpgs;
}

