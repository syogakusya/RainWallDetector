#pragma once

#include "ofMain.h"
#include "RainApp.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "threadedObject.h"
#include "nlohmann/json.hpp"

class ofApp : public ofBaseApp {
public:

	void setup();
	void update();
	void draw();

	void setupGui();
	void drawGui(ofEventArgs & args);
	

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

		

	ofxPanel gui;
	ofParameterGroup parameters;
	ofParameter<float> threshold;
	ofParameter<float> gamma;
	ofParameter<float> maxAreaRadius;
	ofParameter<float> minAreaRadius;
	ofParameter<bool> isCalibMode;
	ofParameter<bool> invertThreshold;

	ThreadedObject threadedContourFinder;
	ofVideoGrabber cam;
	ofImage resultImg, unWarpPersImg;

	void saveParam();
	void loadParam();
private:
	ofImage undistorted;
	ofxCv::Calibration calibration;
};
