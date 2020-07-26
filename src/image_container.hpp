#pragma once
#include "structures.hpp"

class Image_Container
{
public:
    Image_Container(const char *input_filename, const char *output_filename) { load_bmp_tag(input_filename, output_filename); }
    void write(const char *yuvfilename);
    void write_yuvideo(const char *yuvfilename);

private:
    BMPFileHeader bmp_file_header;
    BMPHeaderInfo bmp_header_info;
    BYTE *databuf;
    YUV *yuv_canva;

    void load_bmp_tag(const char *bmpfilename, const char *yuvfilename);
    void distribute_threads();
    inline void convert_rgb2yuv(unsigned long start, unsigned long end);
};