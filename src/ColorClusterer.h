//
//  ColorClusterer.h
//  ColorDetector
//
//  Created by Dina Gafurova on 09/03/2026.
//

#pragma once
#include "ofMain.h"
#include <opencv2/core.hpp>
#include <vector>

struct ClusterColor {
	ofColor color;
	float percent;
};

class ColorClusterer {
public:
	void setK(int k) { K = std::max(2, k); }
	void setSampleN(int n) { sampleN = std::max(1000, n); }

	bool compute(const ofImage& img, const ofRectangle& roi);

	const std::vector<ClusterColor>& getPalette() const { return palette; }

	bool hasOverlay() const { return overlay.isAllocated(); }
	const ofImage& getOverlay() const { return overlay; }

	ofRectangle getUsedROI() const { return usedROI; }

private:
	int K = 5;
	int sampleN = 30000;

	std::vector<ClusterColor> palette;
	ofImage overlay;        
	ofRectangle usedROI;

	static int clampInt(int v, int lo, int hi) {
		return std::max(lo, std::min(v, hi));
	}
};
