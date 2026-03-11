# ColorDetector (openFrameworks + OpenCV)

A small computer-vision tool built in openFrameworks (C++) that:
- reads pixel color under the mouse (RGB + HSV + HEX)
- clusters colors into `k` groups using k-means
- visualizes clusters as a colored overlay
- supports ROI (rectangle) to run clustering only on a selected region

Current version uses a still image source. Next step is to switch to video streaming (webcam or video file) while keeping the same analysis pipeline.

---

## What remains (no rewrite needed)

These parts stay and should continue working when switching to video:
- `ROISelector`: rectangle ROI selection
- `ColorSampler`: hover pixel color readout (RGB + HSV + HEX)
- `ColorClusterer`: k-means color clustering + overlay + palette percentages
- GUI controls: `k`, `sampleN`, ROI on/off, overlay/original toggle

The algorithm pipeline stays:
source → ROI → sample/cluster → overlay → display

---

## What changes for video streaming

Only the “source” layer changes:
- replace image loading (`L` key + `ofImage`) with a live frame source (`ofVideoGrabber` or `ofVideoPlayer`)
- implement `ofApp::update()` to continuously grab frames
- feed the current frame into `ColorSampler` and `ColorClusterer`

### Recommended small refactor (best for video)
Change `ColorSampler` and `ColorClusterer` to accept `ofPixels` (frames naturally provide pixels):

Suggested signatures:
- `ColorSampler`
  - `updateHover(const ofPixels& pix, int x, int y)`
- `ColorClusterer`
  - `compute(const ofPixels& pix, int width, int height, const ofRectangle& roi)`

This supports both sources:
- still image: `img.getPixels()`
- live video: `cam.getPixels()`

---

## Class responsibilities (short)

### ofApp
Orchestrator:
- owns GUI + event handlers
- calls module functions
- in video version: owns video grabber/player and provides latest frame to modules

### ImageManager (current)
Still-image source:
- loads image from disk (triggered by `L` key in `ofApp`)
- stores an `ofImage` and draws it
Note: in video version this can remain for testing, but will be unused or replaced.

### ROISelector
ROI (rectangle) tool:
- stores ROI rectangle
- handles mouse drag to create ROI
- clamps ROI to image/frame bounds
- provides `getROI()`

### ColorSampler
Color “ruler”:
- reads pixel color under the mouse
- stores hover state
- provides RGB, HSV (openFrameworks style), HEX for UI display

### ColorClusterer
Color clustering:
- runs k-means clustering on pixels inside ROI
- outputs:
  - `palette`: list of cluster colors + % of pixels
  - `overlay`: RGBA overlay showing cluster labels (vivid colors)

---

## Controls

### Current (still image mode)
- `L` — load image from disk
- `O` — toggle overlay/original (depends on current implementation)

### GUI
- `k clusters` — number of clusters for k-means
- `sample N` — number of pixel samples used for k-means (speed vs accuracy)
- `use ROI` — enable/disable ROI rectangle
- `show overlay` / `original image` — visualization toggle

---

## Video streaming implementation notes 

Suggested approach:
- Use `ofVideoGrabber` for webcam:
  - `cam.setup(w, h)` in `setup()`
  - `cam.update()` each frame in `update()`
  - `if (cam.isFrameNew()) { pixels = cam.getPixels(); }`

Display:
- draw the current frame
- draw ROI rectangle on top
- draw overlay on top when enabled

Performance:
- avoid running k-means every frame by default (can be heavy)
- recommended:
  - run clustering only when user presses the GUI button “extract”
  - optionally add an “auto” toggle and run every N frames (e.g., every 10 frames)

---

## Dependencies

- openFrameworks
- `ofxGui`
- OpenCV (required by `ColorClusterer` for `cv::kmeans`, etc.)

---

## Suggested next steps checklist

- [ ] Replace still-image source with `ofVideoGrabber` or `ofVideoPlayer`
- [ ] Switch `ColorSampler` and `ColorClusterer` input to `ofPixels`
- [ ] Keep ROI + hover sampling working on live frames
- [ ] Add optional “auto cluster” mode with reduced frequency
