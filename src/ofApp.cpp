#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowTitle("Color detector");
	gui.setup("controls");
	gui.setPosition(15,15);
	
	kClusters.set("k cluster", 5,2,10);
	sampleN.set("sample N", 3000,5000,100000);
	useROI.set("use ROI", false);

		gui.add(kClusters);
		gui.add(sampleN);
		gui.add(useROI);
	

		btnExtract.addListener(this, &ofApp::extractPalette);
		gui.add(btnExtract.setup("extract palette"));

		roi.set(100, 100, 300, 200);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(20);

		if (hasImage) {
			img.draw(0, 0);

			if (useROI) {
				ofNoFill();
				ofSetColor(255);
				ofDrawRectangle(roi);
				ofFill();
			}
			if (hasHover) {
						ofSetColor(0, 180);
						ofDrawRectangle(15, ofGetHeight() - 100, 320, 85);

						ofSetColor(255);
				
						std::string rgbText =
							"RGB: " + ofToString((int)hoverColor.r) + ", " + ofToString((int)hoverColor.g) + ", " + ofToString((int)hoverColor.b);
						ofDrawBitmapString(rgbText, 25, ofGetHeight() - 75);
				
				ofSetColor(hoverColor);
				ofDrawRectangle(280, ofGetHeight() - 62, 45, 40);
				
				ofSetColor(255);
				ofColor hsvColor = hoverColor;
				float hue = hsvColor.getHue();
				float saturation = hsvColor.getSaturation();
				float brightness = hsvColor.getBrightness();
						
						std:: string hsvText =
							"HSV: " + ofToString((int)hue) + ", " + ofToString((int)saturation) + ", " + ofToString((int)brightness);
						ofDrawBitmapString(hsvText, 25, ofGetHeight() - 55);
				
				ofSetColor(hoverColor);
				ofDrawRectangle(280, ofGetHeight() - 92, 45, 60);
				
					}
				} else {
					ofSetColor(220);
					ofDrawBitmapString("Press L to load an image (or implement drag & drop).", 300, 30);
				}

				ofSetColor(255);
				gui.draw();
			}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'l' || key == 'L') {
			ofFileDialogResult r = ofSystemLoadDialog("Select an image");
			if (r.bSuccess) {
				hasImage = img.load(r.getPath());
				hasHover = false;
		}
		}
}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	if (!hasImage) return;
	   if (x < 0 || y < 0 || x >= img.getWidth() || y >= img.getHeight()) {
		   hasHover = false;
		   return;
	   }
	   hoverColor = img.getColor(x, y);
	   hasHover = true;
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (!useROI) return;
		if (roi.inside(x, y)) {
			draggingROI = true;
			roiStart.set(x, y);
		} else {
			// start a new ROI from click
			draggingROI = true;
			roiStart.set(x, y);
			roi.set(x, y, 1, 1);
		}
}

void ofApp::mouseDragged(int x, int y, int button) {
	if (!useROI || !draggingROI) return;
	float x0 = roiStart.x, y0 = roiStart.y;
	roi.set(std::min(x0, (float)x), std::min(y0, (float)y), std::abs(x - x0), std::abs(y - y0));
}

void ofApp::extractPalette() {
	if (!hasImage) return;

	// placeholder for now
	ofLogNotice() << "Extract palette with k=" << kClusters
				  << " sampleN=" << sampleN
				  << " useROI=" << (useROI ? "true" : "false");
}

