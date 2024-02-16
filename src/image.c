#include "image.h"
#include "stb_image.h"
#include "stb_image_write.h"

const char* GetImageFailureReason() {
	return stbi_failure_reason();
}

int CreateImage(struct Image* img, int32_t w, int32_t h, int32_t c) {
	img->pixels = malloc(w * h * c);
	if (img->pixels == NULL) {
		return 1;
	}
	img->w = w;
	img->h = h;
	img->c = c;
	return 0;
}

int LoadImage(struct Image* img, const char* filePath) {
	int32_t w = 0, h = 0, c = 0;
	uint8_t* pixels = stbi_load(filePath, &w, &h, &c, 4);

	if (pixels == NULL || w < 1 || h < 1) {
		return 1;
	}

	img->w = w;
	img->h = h;
	img->c = c;
	img->pixels = pixels;

	return 0;
}

void FreeImage(struct Image* img) {
	free(img->pixels);
	img->w = 0;
	img->h = 0;
	img->c = 0;
	img->pixels = NULL;
}

int WriteImagePng(const struct Image* img, const char* filePath) {
	return stbi_write_png(filePath, img->w, img->h, img->c, img->pixels, img->w * img->c) == 0;
}

