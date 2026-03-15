//
//  ColorClusterer.cpp
//  ColorDetector
//
//  Created by Dina Gafurova on 09/03/2026.
#include "ColorClusterer.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <random>
#include <algorithm>
#include <limits>

static cv::Mat ofToCvBGR(const ofImage& img) {
	
	cv::Mat rgb(img.getHeight(), img.getWidth(), CV_8UC3, (void*)img.getPixels().getData());
	cv::Mat bgr;
	cv::cvtColor(rgb, bgr, cv::COLOR_RGB2BGR);
	return bgr.clone();
}

bool ColorClusterer::compute(const ofImage& img, const ofRectangle& roiIn) {
	palette.clear();
	overlay.clear();
	usedROI.set(0, 0, 0, 0);

	if (!img.isAllocated()) return false;

	const int W = img.getWidth();
	const int H = img.getHeight();
	if (W <= 0 || H <= 0) return false;


	ofRectangle roi = roiIn;
	if (roi.getWidth() <= 2 || roi.getHeight() <= 2) {
		roi.set(0, 0, W, H);
	} else {
		int x = clampInt((int)roi.getX(), 0, W - 1);
		int y = clampInt((int)roi.getY(), 0, H - 1);
		int rw = clampInt((int)roi.getWidth(), 1, W - x);
		int rh = clampInt((int)roi.getHeight(), 1, H - y);
		roi.set(x, y, rw, rh);
	}
	usedROI = roi;

	cv::Mat bgr = ofToCvBGR(img);
	cv::Rect r((int)roi.getX(), (int)roi.getY(), (int)roi.getWidth(), (int)roi.getHeight());
	cv::Mat bgrROI = bgr(r).clone();

	const int totalPx = bgrROI.rows * bgrROI.cols;
	if (totalPx <= 0) return false;

	const int N = std::min(sampleN, totalPx);

	cv::Mat samples(N, 3, CV_32F);

	std::mt19937 rng(12345);
	std::uniform_int_distribution<int> dist(0, totalPx - 1);

	for (int i = 0; i < N; i++) {
		int idx = dist(rng);
		int y = idx / bgrROI.cols;
		int x = idx % bgrROI.cols;

		cv::Vec3b c = bgrROI.at<cv::Vec3b>(y, x);
		samples.at<float>(i, 0) = (float)c[0];
		samples.at<float>(i, 1) = (float)c[1];
		samples.at<float>(i, 2) = (float)c[2];
	}

	
	cv::Mat labels, centers;
	cv::kmeans(
		samples,
		K,
		labels,
		cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 1.0),
		3,
		cv::KMEANS_PP_CENTERS,
		centers
	);

	
	std::vector<cv::Vec3f> ctr(K);
	for (int k = 0; k < K; k++) {
		ctr[k] = cv::Vec3f(
			centers.at<float>(k, 0),
			centers.at<float>(k, 1),
			centers.at<float>(k, 2)
		);
	}

	std::vector<int> counts(K, 0);

	overlay.allocate((int)roi.getWidth(), (int)roi.getHeight(), OF_IMAGE_COLOR_ALPHA);
	ofPixels& opix = overlay.getPixels();

	std::vector<ofColor> displayColors = {
		    ofColor(230, 140, 140, 160),  // soft coral
			ofColor(140, 210, 140, 160),  // soft sage
			ofColor(140, 170, 230, 160),  // soft periwinkle
			ofColor(230, 210, 120, 160),  // soft honey
			ofColor(200, 140, 210, 160),  // soft orchid
			ofColor(120, 200, 210, 160),  // soft turquoise
			ofColor(230, 170, 120, 160),  // soft apricot
			ofColor(180, 140, 210, 160),  // soft lavender
			ofColor(140, 190, 210, 160),  // soft sky
			ofColor(170, 200, 140, 160)   // soft celery
			//ofColor(255,   0,   0, 80), // red
			//ofColor(  0, 255,   0, 80), // green
			//ofColor(  0,   0, 255, 80), // blue
			//ofColor(255, 255,   0, 80), // yellow
			//ofColor(255,   0, 255, 80), // magenta
			//ofColor(  0, 255, 255, 80), // cyan
			//ofColor(255, 128,   0, 80), // orange
			//ofColor(128,   0, 255, 80), // purple
			//ofColor(  0, 128, 255, 80), // sky
			//ofColor(128, 255,   0, 80)  // lime
		};
	
	for (int y = 0; y < bgrROI.rows; y++) {
		for (int x = 0; x < bgrROI.cols; x++) {
			cv::Vec3b c = bgrROI.at<cv::Vec3b>(y, x);
			cv::Vec3f cf((float)c[0], (float)c[1], (float)c[2]);

			int bestK = 0;
			float bestD = std::numeric_limits<float>::max();
			for (int k = 0; k < K; k++) {
				cv::Vec3f d = cf - ctr[k];
				float d2 = d.dot(d);
				if (d2 < bestD) {
					bestD = d2;
					bestK = k;
				}
			}

			counts[bestK]++;
			
			ofColor oc = displayColors[bestK % displayColors.size()];
					 opix.setColor(x, y, oc);
		}
	}

	overlay.update();


	palette.resize(K);
	for (int k = 0; k < K; k++) {
		float pct = 100.0f * (float)counts[k] / (float)totalPx;

		ofColor oc;
		oc.r = (unsigned char)clampInt((int)std::lround(ctr[k][2]), 0, 255);
		oc.g = (unsigned char)clampInt((int)std::lround(ctr[k][1]), 0, 255);
		oc.b = (unsigned char)clampInt((int)std::lround(ctr[k][0]), 0, 255);
		oc.a = 255;

		palette[k] = { oc, pct };
	}

	std::sort(palette.begin(), palette.end(),
			  [](const ClusterColor& a, const ClusterColor& b) {
				  return a.percent > b.percent;
			  });

	return true;
}
