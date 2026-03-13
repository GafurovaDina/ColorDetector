#include "ImageManager.h"

bool ImageManager::setupCamera(int width, int height) {
	initialized = cam.setup(width, height);
	return initialized;
}

void ImageManager::update() {
	if (initialized) {
		cam.update();
	}
}

void ImageManager::draw(float x, float y) const {
	if (!initialized) return;
	cam.draw(x, y);
}
