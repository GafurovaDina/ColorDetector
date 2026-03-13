#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Color Detector - Live Camera");
	ofSetFrameRate(30);
	ofBackground(20);

	imgMgr.setupCamera(camWidth, camHeight);

	gui.setup("controls");
	gui.setPosition(15, 15);

	hueRange.set("Hue Range", 10, 1, 40);
	satRange.set("Sat Range", 60, 1, 120);
	valRange.set("Val Range", 60, 1, 120);
	minArea.set("Min Area", 500, 50, 10000);
	maxArea.set("Max Area", 50000, 1000, 200000);

	gui.add(hueRange);
	gui.add(satRange);
	gui.add(valRange);
	gui.add(minArea);
	gui.add(maxArea);

	grayMask.allocate(camWidth, camHeight);
	maskPreview.allocate(camWidth, camHeight, OF_IMAGE_GRAYSCALE);
}

//--------------------------------------------------------------
void ofApp::update() {
	imgMgr.update();

	if (imgMgr.isFrameNew()) {
		processFrame();
	}
}

//--------------------------------------------------------------
void ofApp::processFrame() {

	const ofPixels & pixels = imgMgr.getPixels();

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

	// Opening: remove small noise
	cv::morphologyEx(mask, maskFiltered, cv::MORPH_OPEN, kernel);

	// Closing: fill small holes inside detected object
	cv::morphologyEx(maskFiltered, maskFiltered, cv::MORPH_CLOSE, kernel);

	// Optional light blur for smoother mask edges
	cv::GaussianBlur(maskFiltered, maskFiltered, cv::Size(5, 5), 0);

	// Convert back to binary after blur
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
void ofApp::draw() {

	ofBackground(20);
	ofSetColor(255);

	if (!imgMgr.isLoaded()) {
		ofDrawBitmapString("Camera not initialized.", 20, 40);
		return;
	}

	imgMgr.draw(20, 20);

	// Draw bounding boxes and labels
	ofNoFill();
	ofSetColor(0, 255, 0);

	for (int i = 0; i < contourFinder.nBlobs; i++) {

		ofRectangle r = contourFinder.blobs[i].boundingRect;

		ofDrawRectangle(
			r.x + 20,
			r.y + 20,
			r.width,
			r.height);

		// Label
		ofFill();
		ofSetColor(0, 255, 0);

		std::string label = "Object " + ofToString(i + 1);

		ofDrawBitmapString(
			label,
			r.x + 20,
			r.y + 15);

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
		ofDrawBitmapString(
			"Objects detected: " + ofToString(contourFinder.nBlobs),
			800,
			570);
	}

	else {

		ofSetColor(255);
		ofDrawBitmapString(
			"Click on the camera stream to select a color",
			700,
			570);
	}

	gui.draw();

	ofSetColor(255);
	ofDrawBitmapString(
		"Press R to reset selected color",
		20,
		700);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	if (x >= 20 && x < 20 + camWidth && y >= 20 && y < 20 + camHeight) {

		int camX = x - 20;
		int camY = y - 20;

		selectedColor = imgMgr.getPixels().getColor(camX, camY);
		hasSelectedColor = true;
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == 'r' || key == 'R') {
		hasSelectedColor = false;
	}
}
