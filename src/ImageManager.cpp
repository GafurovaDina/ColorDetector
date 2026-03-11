//
//  ImageManager.cpp
//  ColorDetector
//
//  Created by Dina Gafurova on 09/03/2026.
//
#include "ImageManager.h"

bool ImageManager::LoadFromDialog() {
	ofFileDialogResult r = ofSystemLoadDialog("Select an image");
	if (!r.bSuccess) return false;
		return LoadFromPath(r.getPath());
	}

	bool ImageManager::LoadFromPath(const std::string& path) {
		loaded = img.load(path);
		return loaded;
	}

	void ImageManager::draw(float x, float y) const {
		if (!loaded) return;
		img.draw(x, y);
	}

