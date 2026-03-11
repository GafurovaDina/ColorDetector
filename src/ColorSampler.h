//
//  ColorSampler.h
//  ColorDetector
//
//  Created by Dina Gafurova on 09/03/2026.
//
#pragma once
#include "ofMain.h"
#include <string>

class ColorSampler {
public:

	void updateHover(const ofImage& img, int x, int y);


	bool hasHover() const { return hoverValid; }
	ofColor getRGB() const { return rgb; }

	
	int getHue() const { return hue; }
	int getSaturation() const { return saturation; }
	int getBrightness() const { return brightness; }

	std::string getHEX() const { return hex; }

private:
	std::string rgbToHEX(const ofColor& c) const;

	bool hoverValid = false;
	ofColor rgb{};

	int hue = 0;
	int saturation = 0;
	int brightness = 0;

	std::string hex{};
};
