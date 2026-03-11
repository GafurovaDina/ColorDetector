
#include "ofApp.h"
#include <algorithm>

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Color detector");

	gui.setup("controls");
	gui.setPosition(15, 15);

	kClusters.set("k clusters", 5, 2, 10);
	useROI.set("use ROI", false);
	sampleN.set("Sample N", 3000, 5000, 100000);
	showOverlay.set("Show overlay", true);

	gui.add(kClusters);
	gui.add(sampleN);
	gui.add(useROI);
	gui.add(showOverlay);

	btnExtract.addListener(this, &ofApp::onExtractPalette);
	gui.add(btnExtract.setup("extract palette"));

	
	roiSel.setDefault(100, 100, 300, 200);
}

//--------------------------------------------------------------
void ofApp::update() {}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(20);

	if (!imgMgr.isLoaded()) {
		ofSetColor(220);
		ofDrawBitmapString("Press L to load an image.", 20, 40);
		ofSetColor(255);
		gui.draw();
		return;
	}

	
	imgMgr.draw(0, 0);


	roiSel.setEnabled(useROI);
	roiSel.draw();
	
	if (showOverlay && clusterer.hasOverlay()) {
		ofRectangle roiUsed = clusterer.getUsedROI();
		ofSetColor(255);
		clusterer.getOverlay().draw(roiUsed.getX(), roiUsed.getY());
	}

	
	if (showOverlay && clusterer.hasOverlay()) {
		ofRectangle roiUsed = clusterer.getUsedROI(); //

	
		ofSetColor(255);
		clusterer.getOverlay().draw(roiUsed.getX(), roiUsed.getY());
	}

	
	if (sampler.hasHover()) {
		ofPushStyle();

		ofSetColor(0, 180);
		ofDrawRectangle(15, ofGetHeight() - 100, 420, 85);

		ofColor c = sampler.getRGB();
		std::string rgbText =
			"RGB: " + ofToString((int)c.r) + ", " + ofToString((int)c.g) + ", " + ofToString((int)c.b);

		std::string hsvText =
			"HSV: " + ofToString(sampler.getHue()) + ", " +
			ofToString(sampler.getSaturation()) + ", " +
			ofToString(sampler.getBrightness());

		std::string hexText = "HEX: " + sampler.getHEX();

		ofSetColor(255);
		ofDrawBitmapString(rgbText, 25, ofGetHeight() - 75);
		ofDrawBitmapString(hsvText, 25, ofGetHeight() - 55);
		ofDrawBitmapString(hexText, 25, ofGetHeight() - 35);

		ofSetColor(c);
		ofDrawRectangle(360, ofGetHeight() - 92, 60, 60);

		ofPopStyle();
	}


	const auto& pal = clusterer.getPalette();
	if (!pal.empty()) {
		float x0 = ofGetWidth() - 230;
		float y0 = 15;
		float sw = 40;
		float sh = 22;

		ofPushStyle();
		ofSetColor(255);
		ofDrawBitmapString("palette (%):", x0, y0 + 10);

		for (size_t i = 0; i < pal.size(); i++) {
			float y = y0 + 20 + (float)i * (sh + 6);

			ofSetColor(pal[i].color);
			ofDrawRectangle(x0, y, sw, sh);

			ofSetColor(255);
			ofDrawBitmapString(ofToString(pal[i].percent, 1) + "%", x0 + sw + 10, y + 16);
		}
		ofPopStyle();
	}


	ofSetColor(255);
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'l' || key == 'L') {
		if (imgMgr.LoadFromDialog()) {
			
			roiSel.setImageBounds(imgMgr.width(), imgMgr.height());
		}
	}

		if (key == 'l' || key == 'L') {
			if (imgMgr.LoadFromDialog()) {
				roiSel.setImageBounds(imgMgr.width(), imgMgr.height());
			}
		}
	if (key == 'o' || key == 'O') {
		showOverlay = !showOverlay;
	}
	}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	if (!imgMgr.isLoaded()) return;
	sampler.updateHover(imgMgr.getImage(), x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	if (!imgMgr.isLoaded()) return;

	roiSel.setEnabled(useROI);
	roiSel.setImageBounds(imgMgr.width(), imgMgr.height());
	roiSel.mousePressed(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	roiSel.mouseDragged(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	roiSel.mouseReleased();
}

//--------------------------------------------------------------
void ofApp::onExtractPalette() {
	if (!imgMgr.isLoaded()) return;

	clusterer.setK(kClusters);
	clusterer.setSampleN(sampleN);


	ofRectangle roi = (useROI && roiSel.hasValidROI())
						? roiSel.getROI()
						: ofRectangle(0, 0, imgMgr.width(), imgMgr.height());

	clusterer.compute(imgMgr.getImage(), roi);
}
