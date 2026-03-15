#include "ofApp.h"
#include <algorithm>

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Color Detector - Camera/Image Mode");
	ofSetFrameRate(30);
	ofBackground(20);
	
	// ===== SETUP CAMERA MODE =====
	camMgr.setupCamera(camWidth, camHeight);
	
	guiCamera.setup("Camera Controls");
	guiCamera.setPosition(15, 60);
	
	hueRange.set("Hue Range", 10, 1, 40);
	satRange.set("Sat Range", 60, 1, 120);
	valRange.set("Val Range", 60, 1, 120);
	minArea.set("Min Area", 500, 50, 10000);
	maxArea.set("Max Area", 50000, 1000, 200000);
	
	guiCamera.add(hueRange);
	guiCamera.add(satRange);
	guiCamera.add(valRange);
	guiCamera.add(minArea);
	guiCamera.add(maxArea);
	
	grayMask.allocate(camWidth, camHeight);
	maskPreview.allocate(camWidth, camHeight, OF_IMAGE_GRAYSCALE);
	
	// ===== SETUP IMAGE MODE =====
	guiImage.setup("Image Controls");
	guiImage.setPosition(15, 60);
	
	kClusters.set("k clusters", 5, 2, 10);
	useROI.set("use ROI", false);
	sampleN.set("Sample N", 30000, 5000, 100000);
	showOverlay.set("Show overlay", true);
	
	guiImage.add(kClusters);
	guiImage.add(sampleN);
	guiImage.add(useROI);
	guiImage.add(showOverlay);
	
	btnExtract.addListener(this, &ofApp::onExtractPalette);
	guiImage.add(btnExtract.setup("extract palette"));
	
	roiSel.setDefault(100, 100, 300, 200);
	
	// Start in camera mode by default
	switchToCameraMode();
}

//--------------------------------------------------------------
void ofApp::update() {
	if (currentMode == Mode::CAMERA_LIVE) {
		camMgr.update();
		
		if (camMgr.isFrameNew()) {
			processCameraFrame();
		}
	}
	// Image mode doesn't need continuous updates
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(20);
	
	// Draw based on current mode
	if (currentMode == Mode::CAMERA_LIVE) {
		if (!camMgr.isLoaded()) {
			ofDrawBitmapString("Camera not initialized.", 20, 40);
			return;
		}
		
		// Draw camera feed
		camMgr.draw(20, 20);
		
		// Draw bounding boxes
		ofNoFill();
		ofSetColor(0, 255, 0);
		
		for (int i = 0; i < contourFinder.nBlobs; i++) {
			ofRectangle r = contourFinder.blobs[i].boundingRect;
			ofDrawRectangle(r.x + 20, r.y + 20, r.width, r.height);
			
			ofFill();
			ofSetColor(0, 255, 0);
			std::string label = "Object " + ofToString(i + 1);
			ofDrawBitmapString(label, r.x + 20, r.y + 15);
			ofNoFill();
		}
		
		ofFill();
		ofSetColor(255);
		maskPreview.draw(700, 20);
		
		ofDrawBitmapString("Live Camera", 20, 15);
		ofDrawBitmapString("Mask Preview", 700, 15);
		
		if (hasSelectedColor) {
			ofDrawBitmapString("Selected Color", 700, 530);
			ofSetColor(selectedColor);
			ofDrawRectangle(700, 545, 80, 40);
			ofSetColor(255);
			ofDrawBitmapString("Objects detected: " + ofToString(contourFinder.nBlobs), 800, 570);
		} else {
			ofSetColor(255);
			ofDrawBitmapString("Click on camera to select a color", 700, 570);
		}
		
		guiCamera.draw();
	}
	
	else if (currentMode == Mode::IMAGE_STATIC) {
		if (!imgMgr.isLoaded()) {
			ofSetColor(220);
			ofDrawBitmapString("Press L to load an image.", 20, 40);
			ofSetColor(255);
			guiImage.draw();
			drawModeIndicator();
			drawInstructions();
			return;
		}
		
		// Draw image
		imgMgr.draw(0, 0);
		
		// Draw ROI selector
		roiSel.setEnabled(useROI);
		roiSel.draw();
		
		// Draw overlay if available
		if (showOverlay && clusterer.hasOverlay()) {
			ofRectangle roiUsed = clusterer.getUsedROI();
			ofSetColor(255);
			clusterer.getOverlay().draw(roiUsed.getX(), roiUsed.getY());
		}
		
		// Draw hover color info
		if (sampler.hasHover()) {
			ofPushStyle();
			ofSetColor(0, 180);
			ofDrawRectangle(15, ofGetHeight() - 100, 490, 85);
			
			ofColor c = sampler.getRGB();
			std::string rgbText = "RGB: " + ofToString((int)c.r) + ", " + ofToString((int)c.g) + ", " + ofToString((int)c.b);
			std::string hsvText = "HSV: " + ofToString(sampler.getHue()) + ", " +
								 ofToString(sampler.getSaturation()) + ", " +
								 ofToString(sampler.getBrightness());
			std::string hexText = "HEX: " + sampler.getHEX();
			
			ofSetColor(255);
			ofDrawBitmapString(rgbText, 25, ofGetHeight() - 75);
			ofDrawBitmapString(hsvText, 25, ofGetHeight() - 55);
			ofDrawBitmapString(hexText, 25, ofGetHeight() - 35);
			
			ofSetColor(c);
			ofDrawRectangle(420, ofGetHeight() - 92, 60, 60);
			ofPopStyle();
		}
		
		// Draw palette
		const auto& pal = clusterer.getPalette();
		if (!pal.empty()) {
			float x0 = ofGetWidth() - 230;
			float y0 = 15;
			float sw = 40;
			float sh = 22;
			
			ofPushStyle();
			ofSetColor(0);
			ofDrawBitmapString("palette (%):", x0, y0 + 10);
			
			for (size_t i = 0; i < pal.size(); i++) {
				float y = y0 + 20 + (float)i * (sh + 6);
				ofSetColor(pal[i].color);
				ofDrawRectangle(x0, y, sw, sh);
				ofSetColor(0);
				ofDrawBitmapString(ofToString(pal[i].percent, 1) + "%", x0 + sw + 10, y + 16);
			}
			ofPopStyle();
		}
		
		guiImage.draw();
	}
	
	// Draw common UI elements
	drawModeIndicator();
	drawInstructions();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	// Global mode switching
	if (key == '1') {
		switchToCameraMode();
	} else if (key == '2') {
		switchToImageMode();
	}
	
	// Mode-specific keys
	if (currentMode == Mode::CAMERA_LIVE) {
		if (key == 'r' || key == 'R') {
			hasSelectedColor = false;
		}
	}
	
	else if (currentMode == Mode::IMAGE_STATIC) {
		if (key == 'l' || key == 'L') {
			if (imgMgr.LoadFromDialog()) {
				roiSel.setImageBounds(imgMgr.width(), imgMgr.height());
			}
		}
		if (key == 'o' || key == 'O') {
			showOverlay = !showOverlay;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	if (currentMode == Mode::IMAGE_STATIC && imgMgr.isLoaded()) {
		sampler.updateHover(imgMgr.getImage(), x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	if (currentMode == Mode::CAMERA_LIVE) {
		// Color selection in camera mode
		if (x >= 20 && x < 20 + camWidth && y >= 20 && y < 20 + camHeight) {
			int camX = x - 20;
			int camY = y - 20;
			selectedColor = camMgr.getPixels().getColor(camX, camY);
			hasSelectedColor = true;
		}
	}
	
	else if (currentMode == Mode::IMAGE_STATIC && imgMgr.isLoaded()) {
		roiSel.setEnabled(useROI);
		roiSel.setImageBounds(imgMgr.width(), imgMgr.height());
		roiSel.mousePressed(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	if (currentMode == Mode::IMAGE_STATIC) {
		roiSel.mouseDragged(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	if (currentMode == Mode::IMAGE_STATIC) {
		roiSel.mouseReleased();
	}
}

//--------------------------------------------------------------
void ofApp::switchToCameraMode() {
	currentMode = Mode::CAMERA_LIVE;
	camMgr.setupCamera(camWidth, camHeight);  // Reinitialize if needed
	ofLogNotice() << "Switched to Camera Mode (Press 1 for camera, 2 for images)";
}

//--------------------------------------------------------------
void ofApp::switchToImageMode() {
	currentMode = Mode::IMAGE_STATIC;
	camMgr = ImageManager_live();  // Reset camera (optional)
	ofLogNotice() << "Switched to Image Mode (Press 1 for camera, 2 for images)";
}

//--------------------------------------------------------------
void ofApp::drawModeIndicator() {
	ofPushStyle();
	ofSetColor(255, 255, 0);
	string modeText = (currentMode == Mode::CAMERA_LIVE) ?
					  "MODE: LIVE CAMERA [Press 2 for Image Mode]" :
					  "MODE: IMAGE ANALYSIS [Press 1 for Camera Mode]";
	ofDrawBitmapStringHighlight(modeText, 20, 35);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawInstructions() {
	ofPushStyle();
	ofSetColor(200);
	int yPos = ofGetHeight() - 20;
	
	if (currentMode == Mode::CAMERA_LIVE) {
		ofDrawBitmapString("Camera: Click to select color | R: reset | 1/2: switch modes", 20, yPos);
	} else {
		ofDrawBitmapString("Image: L: load image | O: toggle overlay | 1/2: switch modes", 20, yPos);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::processCameraFrame() {
	const ofPixels & pixels = camMgr.getPixels();
	
	frameRgb = cv::Mat(camHeight, camWidth, CV_8UC3, (void *)pixels.getData()).clone();
	cv::cvtColor(frameRgb, frameHsv, cv::COLOR_RGB2HSV);
	
	if (!hasSelectedColor) {
		mask = cv::Mat::zeros(camHeight, camWidth, CV_8UC1);
		maskFiltered = mask.clone();
		maskPreview.setFromPixels(mask.data, camWidth, camHeight, OF_IMAGE_GRAYSCALE);
		return;
	}
	
	cv::Vec3b hsv = getSelectedHSV();
	
	int h = hsv[0];
	int s = hsv[1];
	int v = hsv[2];
	
	cv::Scalar lower1, upper1, lower2, upper2;
	cv::Mat mask1, mask2;
	
	int hLow = h - hueRange;
	int hHigh = h + hueRange;
	
	if (hLow < 0) {
		lower1 = cv::Scalar(0, std::max(0, s - (int)satRange), std::max(0, v - (int)valRange));
		upper1 = cv::Scalar(hHigh, std::min(255, s + (int)satRange), std::min(255, v + (int)valRange));
		
		lower2 = cv::Scalar(180 + hLow, std::max(0, s - (int)satRange), std::max(0, v - (int)valRange));
		upper2 = cv::Scalar(179, std::min(255, s + (int)satRange), std::min(255, v + (int)valRange));
		
		cv::inRange(frameHsv, lower1, upper1, mask1);
		cv::inRange(frameHsv, lower2, upper2, mask2);
		cv::bitwise_or(mask1, mask2, mask);
	}
	else if (hHigh > 179) {
		lower1 = cv::Scalar(hLow, std::max(0, s - (int)satRange), std::max(0, v - (int)valRange));
		upper1 = cv::Scalar(179, std::min(255, s + (int)satRange), std::min(255, v + (int)valRange));
		
		lower2 = cv::Scalar(0, std::max(0, s - (int)satRange), std::max(0, v - (int)valRange));
		upper2 = cv::Scalar(hHigh - 179, std::min(255, s + (int)satRange), std::min(255, v + (int)valRange));
		
		cv::inRange(frameHsv, lower1, upper1, mask1);
		cv::inRange(frameHsv, lower2, upper2, mask2);
		cv::bitwise_or(mask1, mask2, mask);
	}
	else {
		cv::Scalar lower(hLow, std::max(0, s - (int)satRange), std::max(0, v - (int)valRange));
		cv::Scalar upper(hHigh, std::min(255, s + (int)satRange), std::min(255, v + (int)valRange));
		cv::inRange(frameHsv, lower, upper, mask);
	}
	
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
	cv::morphologyEx(mask, maskFiltered, cv::MORPH_OPEN, kernel);
	cv::morphologyEx(maskFiltered, maskFiltered, cv::MORPH_CLOSE, kernel);
	cv::GaussianBlur(maskFiltered, maskFiltered, cv::Size(5, 5), 0);
	cv::threshold(maskFiltered, maskFiltered, 127, 255, cv::THRESH_BINARY);
	
	grayMask.setFromPixels(maskFiltered.data, camWidth, camHeight);
	
	contourFinder.findContours(
		grayMask,
		minArea,
		maxArea,
		20,
		false);
	
	maskPreview.setFromPixels(maskFiltered.data, camWidth, camHeight, OF_IMAGE_GRAYSCALE);
}

//--------------------------------------------------------------
cv::Vec3b ofApp::getSelectedHSV() const {
	cv::Mat rgb(1, 1, CV_8UC3);
	rgb.at<cv::Vec3b>(0, 0) = cv::Vec3b(
		selectedColor.r,
		selectedColor.g,
		selectedColor.b);
	
	cv::Mat hsv;
	cv::cvtColor(rgb, hsv, cv::COLOR_RGB2HSV);
	return hsv.at<cv::Vec3b>(0, 0);
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
