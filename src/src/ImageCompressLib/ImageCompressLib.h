#pragma once

typedef enum _IMAGE_TYPE{
	IMAGE_TYPE_UNKNOWN,
	IMAGE_TYPE_NOT_EXIST,
	IMAGE_TYPE_JPEG,
	IMAGE_TYPE_NORMAL_PNG,
	IMAGE_TYPE_APNG,
	IMAGE_TYPE_COMPLIED_9PNG,
	IMAGE_TYPE_UNCOMPLIED_9PNG,
	IMAGE_TYPE_GFT
}IMAGE_TYPE;

IMAGE_TYPE getImageType(const char *fileName);



bool ApngCompress(const char* inputFile, const char* outputFile);

bool ApngDissemble(const char* inputFile, const char* outputDir);

bool ApngAssemble(const char *inputDir, const char* outputFile);

bool NinePatchOpt(const char *inputFile, const char *outputFile, int outputKind, int(&ninePatch)[4], int quality = 100);

bool PngZopfliCompress(const char *inputFile, const char*outputFile);

extern "C"{
    int PngQuantFile(const char *fileName, const char *outName, int quality);
    int CJpegFile(const char *fileName, const char *outNamem, int quality);
}
