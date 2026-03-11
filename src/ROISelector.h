//
//  ROISelector.h
//  ColorDetector
//
//  Created by Dina Gafurova on 09/03/2026.
//
#pragma once
#include "ofMain.h"

class ROISelector {
public:
	void setEnabled (bool e) { enabled = e; }
	bool isEnabled() const { return enabled; }
	
	void setImageBounds (int w, int h) { imgW = w; imgH = h; clampROI(); }
	
	const ofRectangle& getROI() const {return roi; }
	
	bool hasValidROI() const { return enabled && roi.getWidth() > 2 && roi.getHeight() >2; }
	
	void setDefault(float x, float y, float w, float h) { roi.set(x, y, w, h); clampROI(); }
	void draw() const;
	
	void mousePressed(int x, int y);
	void mouseDragged(int x, int y);
	void mouseReleased();
	
private:
	void clampROI();

	bool enabled = false;
	bool dragging = false;
	ofPoint start;         
	ofRectangle roi{100, 100, 300, 200};

	int imgW = 0;
	int imgH = 0;
};

