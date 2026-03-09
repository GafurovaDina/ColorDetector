#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		

		void keyPressed(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		
	    void extractPalette();
		
		ofImage img;
		bool hasImage = false;
	
		ofxPanel gui;
		ofParameter<int> kClusters;
		ofParameter<int> sampleN;
		ofParameter<bool> useROI;
		ofParameter<bool> ignoreNearWhite;
		ofParameter<bool> ignoreNearBlack;
		ofxButton btnExtract;
	
		ofRectangle roi;
		bool draggingROI = false;
		ofPoint roiStart;
	
		ofColor hoverColor;
		bool hasHover = false;

	
};
