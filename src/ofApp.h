#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include <opencv2/opencv.hpp>

#include "ImageManager.h"      // For image loading
#include "ImageManager_live.h"  // For camera (renamed from ImageManager_live.h)
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
	
	enum class Mode {
		CAMERA_LIVE,
		IMAGE_STATIC
	};
	Mode currentMode = Mode::CAMERA_LIVE;
	
	
	void switchToCameraMode();
	void switchToImageMode();
	void drawModeIndicator();
	

	ImageManager imgMgr;
	ROISelector roiSel;
	ColorSampler sampler;
	ColorClusterer clusterer;
	
	ofxPanel guiImage;
	ofParameter<int> kClusters;
	ofParameter<int> sampleN;
	ofParameter<bool> useROI;
	ofParameter<bool> showOverlay;
	ofxButton btnExtract;
	
	void onExtractPalette();
	
	// ===== CAMERA MODE COMPONENTS (from live version) =====
	ImageManager_live camMgr;  // Renamed from ImageManager to avoid conflict
	
	ofxPanel guiCamera;
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
	
	void processCameraFrame();
	cv::Vec3b getSelectedHSV() const;
	
	// ===== COMMON SETTINGS =====
	void drawInstructions();
	void drawColorInfo();
};
