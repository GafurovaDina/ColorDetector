//
//  ROISelector.cpp
//  ColorDetector
//
//  Created by Dina Gafurova on 09/03/2026.
//
#include "ROISelector.h"
#include <algorithm>

void ROISelector::clampROI() {
	if (imgW <= 0 || imgH <= 0) return;

	float x = std::max(0.0f, roi.getX());
	float y = std::max(0.0f, roi.getY());

	float right  = std::min((float)imgW, x + roi.getWidth());
	float bottom = std::min((float)imgH, y + roi.getHeight());

	roi.set(x, y, std::max(0.0f, right - x), std::max(0.0f, bottom - y));
}

void ROISelector::draw() const {
	if (!enabled) return;

	ofPushStyle();
	ofNoFill();
	ofSetColor(255);
	ofDrawRectangle(roi);
	ofPopStyle();
}

void ROISelector::mousePressed(int x, int y) {
	if (!enabled) return;

	if (imgW > 0 && imgH > 0) {
		if (x < 0 || y < 0 || x >= imgW || y >= imgH) return;
	}

	dragging = true;
	start.set(x, y);

	roi.set(x, y, 1, 1);
	clampROI();
}

void ROISelector::mouseDragged(int x, int y) {
	if (!enabled || !dragging) return;

	float x0 = start.x, y0 = start.y;
	roi.set(std::min(x0, (float)x),
			std::min(y0, (float)y),
			std::abs(x - x0),
			std::abs(y - y0));
	clampROI();
}

void ROISelector::mouseReleased() {
	dragging = false;
	clampROI();
}
