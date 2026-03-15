#include "ImageManager_live.h"

bool ImageManager_live::setupCamera(int width, int height) {
	initialized = cam.setup(width, height);
	return initialized;
}

void ImageManager_live::update() {
	if (initialized) {
		cam.update();
	}
}

void ImageManager_live::draw(float x, float y) const {
	if (!initialized) return;
	cam.draw(x, y);
}
