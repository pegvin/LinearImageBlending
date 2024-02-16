#ifndef APP_SRC_IMAGE_H_INCLUDED_
#define APP_SRC_IMAGE_H_INCLUDED_ 1
#pragma once

#include <stdint.h>

struct Image {
	int32_t w, h, c;
	uint8_t* pixels;
};

int  LoadImage(struct Image* img, const char* filePath);
int  CreateImage(struct Image* img, int32_t w, int32_t h, int32_t c);
int  WriteImagePng(const struct Image* img, const char* filePath);
void FreeImage(struct Image* img);

const char* GetImageFailureReason();

#endif // APP_SRC_IMAGE_H_INCLUDED_

