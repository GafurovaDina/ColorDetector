
#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "ImageManager.h"
#include "ROISelector.h"
#include "ColorSampler.h"
#include "ColorClusterer.h"

class ofApp : public ofBaseApp {
public:
	void setup() override;
	void update() override;
	void draw() override;

	void keyPressed(int key) override;
	void mouseMoved(int x, int y) override;
	void mousePressed(int x, int y, int button) override;
	void mouseDragged(int x, int y, int button) override;
	void mouseReleased(int x, int y, int button) override;


private:
	void onExtractPalette();

	
	ImageManager imgMgr;
	ROISelector roiSel;
	ColorSampler sampler;
	ColorClusterer clusterer;

	
	ofxPanel gui;
	ofParameter<int> kClusters;
	ofParameter<int> sampleN;
	ofParameter<bool> useROI;
	ofParameter<bool> showOverlay;
	
	ofxButton btnExtract;
};
