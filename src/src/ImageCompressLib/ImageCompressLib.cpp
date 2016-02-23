#include "ImageCompressLib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "libpng/png.h"
#include "PNG_Image.h"

IMAGE_TYPE getImageType(const char *fileName){
	std::wstring name;// CompressTools::WideCharFromMultiBytes(fileName);
	FILE *file = nullptr;
	IMAGE_TYPE result = IMAGE_TYPE_UNKNOWN;
	if ((file = _wfopen(name.c_str(), L"rb")) == nullptr)
		return IMAGE_TYPE_NOT_EXIST;
	unsigned char header[8] = { 0 };
	if (fread(header, 8, 1, file)){
		if (header[0] == 0xFF && header[1] == 0xD8 && header[2] == 0xFF)
			result = IMAGE_TYPE_JPEG;
		else if (header[0] == 0x89 && header[1] == 0x50 &&
			header[2] == 0x4E && header[3] == 0x47 &&
			header[4] == 0x0D && header[5] == 0x0A &&
			header[6] == 0x1A && header[7] == 0x0A){
			fclose(file);
			PNG_Image pngImage;
			pngImage.read_png(std::string(fileName));
			switch (pngImage.getPngType()){
			case PNG_Image::PNG_IMAGE_TYPE_APNG:
				return IMAGE_TYPE_APNG;
			case PNG_Image::PNG_IMAGE_TYPE_COMPLIED_9PNG:
				return IMAGE_TYPE_COMPLIED_9PNG;
			case PNG_Image::PNG_IMAGE_TYPE_NORMAL:
				return IMAGE_TYPE_NORMAL_PNG;
			case PNG_Image::PNG_IMAGE_TYPE_NOT_EXIST:
				return IMAGE_TYPE_NOT_EXIST;
			case PNG_Image::PNG_IMAGE_TYPE_UNCOMPLIED_9PNG:
				return IMAGE_TYPE_UNCOMPLIED_9PNG;
			case PNG_Image::PNG_IMAGE_TYPE_UNKNOWN:
				return IMAGE_TYPE_UNKNOWN;
			default:
				return IMAGE_TYPE_UNKNOWN;
			}
		}
		else if (header[0] == 0x54 && header[1] == 0x47 && header[2] == 0x46)
			result = IMAGE_TYPE_GFT;
		else
			result = IMAGE_TYPE_UNKNOWN;
	}
	else
		result = IMAGE_TYPE_UNKNOWN;
	fclose(file);
	return result;
}
