#include <math.h>
#include "blend.h"

#define MIN(a, min) (a <= min ? min : a)
#define MAX(a, max) (a >= max ? max : a)
#define MIN_MAX(a, min, max) MAX(MIN(a, min), max)

double Gamma(double x) {
	if (x >= 0.0031308) {
		return (1.055) * pow(x, (1.0/2.4)) - 0.055;
	} else {
		return 12.92 * x;
	}
}

double Gamma_Inv(double x) {
	if (x >= 0.04045) {
		return pow((x + 0.055)/(1 + 0.055), 2.4);
	} else {
		return x / 12.92;
	}
}

void LinearBlend(struct sRGB _src, struct sRGB _backdrop, struct sRGB* _out, enum BlendOP op) {
	struct lRGB src = sRGB_To_lRGB(_src);
	struct lRGB backdrop = sRGB_To_lRGB(_backdrop);
	struct lRGB out;

	switch (op) {
		case BlendOP_Alpha: {
			out.r = src.r;
			out.g = src.g;
			out.b = src.b;
			break;
		}
		case BlendOP_Addition: {
			out.r = src.r + backdrop.r;
			out.g = src.g + backdrop.g;
			out.b = src.b + backdrop.b;
			break;
		}
		case BlendOP_Subtraction: {
			out.r = backdrop.r - src.r;
			out.g = backdrop.g - src.g;
			out.b = backdrop.b - src.b;
			break;
		}
		case BlendOP_Multiply: {
			out.r = src.r * backdrop.r;
			out.g = src.g * backdrop.g;
			out.b = src.b * backdrop.b;
			break;
		}
		case BlendOP_Screen: {
			out.r = backdrop.r + src.r - (src.r * backdrop.r);
			out.g = backdrop.g + src.g - (src.g * backdrop.g);
			out.b = backdrop.b + src.b - (src.b * backdrop.b);
			break;
		}
		case BlendOP_Darken: {
			out.r = MAX(backdrop.r, src.r);
			out.g = MAX(backdrop.g, src.g);
			out.b = MAX(backdrop.b, src.b);
			break;
		}
		case BlendOP_Lighten: {
			out.r = MIN(backdrop.r, src.r);
			out.g = MIN(backdrop.g, src.g);
			out.b = MIN(backdrop.b, src.b);
			break;
		}
	}

	out.r = MIN(out.r, 0);
	out.g = MIN(out.g, 0);
	out.b = MIN(out.b, 0);
	out.a = src.a;
	*_out = lRGB_To_sRGB(out);
}

// Same as ((a/255) * (b/255)) * 255 But FASSST
// From Pixman: https://gitlab.freedesktop.org/pixman/pixman/-/blob/fdd716109787ef825f9eb88f73447297c43e5c10/pixman/pixman-combine32.h
#define MUL_UN8(a, b, t) ((t) = (a) * (uint16_t)(b) + 0x80, ((((t) >> 8) + (t)) >> 8))

void NormalBlend(struct sRGB src, struct sRGB backdrop, struct sRGB* _out, enum BlendOP op) {
	struct sRGB out;

	int32_t r = 0, g = 0, b = 0, t;
	switch (op) {
		case BlendOP_Alpha: {
			r = src.r;
			g = src.g;
			b = src.b;
			break;
		}
		case BlendOP_Addition: {
			r = src.r + backdrop.r;
			g = src.g + backdrop.g;
			b = src.b + backdrop.b;
			break;
		}
		case BlendOP_Subtraction: {
			r = backdrop.r - src.r;
			g = backdrop.g - src.g;
			b = backdrop.b - src.b;
			break;
		}
		case BlendOP_Multiply: {
			r = MUL_UN8(src.r, backdrop.r, t);
			g = MUL_UN8(src.g, backdrop.g, t);
			b = MUL_UN8(src.b, backdrop.b, t);
			break;
		}
		case BlendOP_Screen: {
			r = backdrop.r + src.r - (src.r * backdrop.r);
			g = backdrop.g + src.g - (src.g * backdrop.g);
			b = backdrop.b + src.b - (src.b * backdrop.b);
			break;
		}
		case BlendOP_Darken: {
			r = MAX(backdrop.r, src.r);
			g = MAX(backdrop.g, src.g);
			b = MAX(backdrop.b, src.b);
			break;
		}
		case BlendOP_Lighten: {
			r = MIN(backdrop.r, src.r);
			g = MIN(backdrop.g, src.g);
			b = MIN(backdrop.b, src.b);
			break;
		}
	}

	out.r = MIN_MAX(r, 0, 255);
	out.g = MIN_MAX(g, 0, 255);
	out.b = MIN_MAX(b, 0, 255);
	out.a = src.a;
	*_out = out;
}

struct lRGB sRGB_To_lRGB(struct sRGB col) {
	struct lRGB out;

	out.r = Gamma_Inv(col.r / 255.0f);
	out.g = Gamma_Inv(col.g / 255.0f);
	out.b = Gamma_Inv(col.b / 255.0f);
	out.a = col.a / 255.0f;

	return out;
}

struct sRGB lRGB_To_sRGB(struct lRGB col) {
	struct sRGB out;

	out.r = MIN_MAX(Gamma(col.r) * 255.0f, 0, 255);
	out.g = MIN_MAX(Gamma(col.g) * 255.0f, 0, 255);
	out.b = MIN_MAX(Gamma(col.b) * 255.0f, 0, 255);
	out.a = MIN_MAX(col.a * 255.0f, 0, 255);

	return out;
}

struct sRGB AlphaBlend(struct sRGB _src, struct sRGB _backdrop) {
	struct Norm_sRGB out;
	struct Norm_sRGB t = { _src.r / 255.0f, _src.g / 255.0f, _src.b / 255.0f, _src.a / 255.0f };
	struct Norm_sRGB b = { _backdrop.r / 255.0f, _backdrop.g / 255.0f, _backdrop.b / 255.0f, _backdrop.a / 255.0f };

	if (t.a >= 1.0f)
		return _src;

	out.a = ((1.0f - t.a) * b.a + t.a);

	if (out.a == 0.0f)
		return (struct sRGB){ 0, 0, 0, 0 };

	out.r = ((1.0f - t.a) * b.a * b.r + t.a * t.r) / out.a;
	out.g = ((1.0f - t.a) * b.a * b.g + t.a * t.g) / out.a;
	out.b = ((1.0f - t.a) * b.a * b.b + t.a * t.b) / out.a;

	struct sRGB outFinal = { out.r * 255, out.g * 255, out.b * 255, out.a * 255 };

	return outFinal;
}

struct lRGB AlphaBlend_Linear(struct lRGB t, struct lRGB b) {
	struct lRGB out;

	if (t.a >= 1.0f)
		return t;

	out.a = ((1.0f - t.a) * b.a + t.a);

	if (out.a == 0.0f)
		return (struct lRGB){ 0, 0, 0, 0 };

	out.r = ((1.0f - t.a) * b.a * b.r + t.a * t.r) / out.a;
	out.g = ((1.0f - t.a) * b.a * b.g + t.a * t.g) / out.a;
	out.b = ((1.0f - t.a) * b.a * b.b + t.a * t.b) / out.a;

	return out;
}

