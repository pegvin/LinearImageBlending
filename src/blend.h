#ifndef APP_SRC_BLEND_H_INCLUDED_
#define APP_SRC_BLEND_H_INCLUDED_ 1
#pragma once

#include <stdint.h>

// sRGB: 0 - 1
struct Norm_sRGB {
	double r, g, b, a;
};

// sRGB: 0 - 255
struct sRGB {
	uint8_t r, g, b, a;
};

// Linear RGB
struct lRGB {
	double r, g, b, a;
};

enum BlendOP {
	BlendOP_Alpha,
	BlendOP_Addition,
	BlendOP_Subtraction,
	BlendOP_Multiply,
	BlendOP_Screen,
	BlendOP_Darken,
	BlendOP_Lighten
};

struct lRGB sRGB_To_lRGB(struct sRGB col);
struct sRGB lRGB_To_sRGB(struct lRGB col);

struct sRGB AlphaBlend(struct sRGB _src, struct sRGB _backdrop);
struct lRGB AlphaBlend_Linear(struct lRGB t, struct lRGB b);

void LinearBlend(struct sRGB _src, struct sRGB _backdrop, struct sRGB* _out, enum BlendOP op);
void NormalBlend(struct sRGB src, struct sRGB backdrop, struct sRGB* _out, enum BlendOP op);

#endif // APP_SRC_BLEND_H_INCLUDED_

