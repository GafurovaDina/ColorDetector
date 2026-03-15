//
//  ImageManager.h
//  ColorDetector
//
//  Created by Dina Gafurova on 09/03/2026.
//

#pragma once
#include "ofMain.h"
class ImageManager {
public:
	bool LoadFromDialog();
	bool LoadFromPath(const std::string& path);
	
	bool isLoaded() const { return loaded; }
	int width() const { return loaded ? img.getWidth() :0; }
	int height() const { return loaded ? img.getHeight() :0; }
	void draw(float x = 0, float y = 0) const;
	
	const ofImage& getImage() const {return img; }
	
private:
	ofImage img;
	bool loaded = false;
};
