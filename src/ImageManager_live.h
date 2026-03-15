#pragma once
#include "ofMain.h"

class ImageManager_live {
public:
	bool setupCamera(int width = 640, int height = 480);
	void update();
	void draw(float x = 0, float y = 0) const;

	bool isLoaded() const { return initialized; }
	bool isFrameNew() const { return initialized && cam.isFrameNew(); }

	int width() const { return initialized ? cam.getWidth() : 0; }
	int height() const { return initialized ? cam.getHeight() : 0; }

	const ofPixels & getPixels() const { return cam.getPixels(); }

private:
	ofVideoGrabber cam;
	bool initialized = false;
};
