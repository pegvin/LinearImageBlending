#include <stdio.h>
#include <stdint.h>

#include "image.h"
#include "blend.h"

int main(void) {
	int result = 0;

	struct Image fgImage = {0};
	struct Image output_linear = {0};
	struct Image output_normal = {0};

	if (LoadImage(&fgImage, "green-gradient.png") != 0) {
		printf("Failed To Load 'green-gradient.png': %s\n", GetImageFailureReason());
		result = 1;
		goto end;
	}

	CreateImage(&output_linear, fgImage.w, fgImage.h, fgImage.c);
	CreateImage(&output_normal, fgImage.w, fgImage.h, fgImage.c);
	for (int32_t y = 0; y < fgImage.h; y++) {
		for (int32_t x = 0; x < fgImage.w; x++) {
			struct sRGB src = {
				*(&fgImage.pixels[((y * fgImage.w) + x) * fgImage.c] + 0),
				*(&fgImage.pixels[((y * fgImage.w) + x) * fgImage.c] + 1),
				*(&fgImage.pixels[((y * fgImage.w) + x) * fgImage.c] + 2),
				*(&fgImage.pixels[((y * fgImage.w) + x) * fgImage.c] + 3)
			};
			struct sRGB backdrop = { 0xF6, 0x07, 0xF6, 255 };
			enum BlendOP op = BlendOP_Alpha;
			{
				struct sRGB out;
				LinearBlend(src, backdrop, &out, op);
				out = lRGB_To_sRGB(AlphaBlend_Linear(sRGB_To_lRGB(out), sRGB_To_lRGB(backdrop)));
				*(&output_linear.pixels[((y * fgImage.w) + x) * fgImage.c] + 0) = out.r;
				*(&output_linear.pixels[((y * fgImage.w) + x) * fgImage.c] + 1) = out.g;
				*(&output_linear.pixels[((y * fgImage.w) + x) * fgImage.c] + 2) = out.b;
				*(&output_linear.pixels[((y * fgImage.w) + x) * fgImage.c] + 3) = out.a;
			}
			{
				struct sRGB out;
				NormalBlend(src, backdrop, &out, op);
				out = AlphaBlend(out, backdrop);
				*(&output_normal.pixels[((y * fgImage.w) + x) * fgImage.c] + 0) = out.r;
				*(&output_normal.pixels[((y * fgImage.w) + x) * fgImage.c] + 1) = out.g;
				*(&output_normal.pixels[((y * fgImage.w) + x) * fgImage.c] + 2) = out.b;
				*(&output_normal.pixels[((y * fgImage.w) + x) * fgImage.c] + 3) = out.a;
			}
		}
	}

	if (WriteImagePng(&output_linear, "output.linear.png") != 0) {
		printf("Failed To Write 'output.linear.png': %s\n", GetImageFailureReason());
		result = 1;
		goto end;
	}

	if (WriteImagePng(&output_normal, "output.normal.png") != 0) {
		printf("Failed To Write 'output.normal.png': %s\n", GetImageFailureReason());
		result = 1;
		goto end;
	}

end:
	if (fgImage.w > 0) FreeImage(&fgImage);
	if (output_linear.w > 0) FreeImage(&output_linear);
	if (output_normal.w > 0) FreeImage(&output_normal);
	return result;
}

