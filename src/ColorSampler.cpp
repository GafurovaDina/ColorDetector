//
//  ColorSampler.cpp
//  ColorDetector
//
//  Created by Dina Gafurova on 09/03/2026.

#include "ColorSampler.h"
#include <cstdio>

void ColorSampler::updateHover(const ofImage& img, int x, int y) {
	if (!img.isAllocated()) {
		hoverValid = false;
		return;
	}

	if (x < 0 || y < 0 || x >= img.getWidth() || y >= img.getHeight()) {
		hoverValid = false;
		return;
	}

	rgb = img.getColor(x, y);
	hoverValid = true;

	hue = (int)rgb.getHue();
	saturation = (int)rgb.getSaturation();
	brightness = (int)rgb.getBrightness();

	hex = rgbToHEX(rgb);
}

std::string ColorSampler::rgbToHEX(const ofColor& c) const {
	char buf[8];
	std::snprintf(buf, sizeof(buf), "#%02X%02X%02X",
				  (unsigned)c.r, (unsigned)c.g, (unsigned)c.b);
	return std::string(buf);
}
