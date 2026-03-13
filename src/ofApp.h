#pragma once

#include "ImageManager.h"
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include <opencv2/opencv.hpp>

class ofApp : public ofBaseApp {
public:
	void setup() override;
	void update() override;
	void draw() override;

	void keyPressed(int key) override;
	void mousePressed(int x, int y, int button) override;

private:
	void processFrame();
	cv::Vec3b getSelectedHSV() const;

	ImageManager imgMgr;

	ofxPanel gui;
	ofParameter<int> hueRange;
	ofParameter<int> satRange;
	ofParameter<int> valRange;
	ofParameter<int> minArea;
	ofParameter<int> maxArea;

	ofColor selectedColor;
	bool hasSelectedColor = false;

	int camWidth = 640;
	int camHeight = 480;

	cv::Mat frameRgb;
	cv::Mat frameHsv;
	cv::Mat mask;
	cv::Mat maskFiltered;

	ofxCvGrayscaleImage grayMask;
	ofxCvContourFinder contourFinder;
	ofImage maskPreview;
};
