#include "includes.hpp"
#include "image_container.hpp"

int main(int argc, char *argv[])
{
	if (argc == 3)
	{
		Image_Container img_con(argv[1], argv[2]);
		img_con.write(argv[2]);
	}
	if (argc == 4)
	{
		Image_Container img_con(argv[1], argv[2]);
		img_con.write(argv[2]);
		img_con.write_yuvideo(argv[3]);
	}
	else
	{
		std::cout << "Incorrect input data.\nExample: bmp2yuv.exe img.bmp img.yuv\nOr: bmp2yuv.exe img.bmp img.yuv video.yuv";
	}
	return 0;
}
