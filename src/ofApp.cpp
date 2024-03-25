#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
	ofSetVerticalSync(true);

	cam.setup(640, 480);
	
	calibration.setFillFrame(true);
	calibration.load("calibration.yml");
	imitate(undistorted, cam);

	threadedContourFinder.setup();
	threadedContourFinder.setImage(&undistorted);
	loadParam();

}

void ofApp::setupGui() {
	parameters.setName("parameters");
	parameters.add(threshold.set("Threshold", 128, 0, 255));
	parameters.add(gamma.set("Gamma", 2, 0, 10));
	parameters.add(maxAreaRadius.set("MaxAreaRadius", 100, 0, 500));
	parameters.add(minAreaRadius.set("MinAreaRadius", 10.0, 0, 30.0));
	parameters.add(isCalibMode.set("Calibaration", false));
	parameters.add(invertThreshold.set("Invert", false));
	gui.setup(parameters);
	ofSetBackgroundColor(0);
}

void ofApp::update() {
	cam.update();
	if (cam.isFrameNew()) {
		calibration.undistort(toCv(cam), toCv(undistorted));
		undistorted.update();
	}

	threadedContourFinder.setParameter(threshold, gamma, maxAreaRadius, minAreaRadius, isCalibMode, invertThreshold);
	threadedContourFinder.update();
	threadedContourFinder.getContourImage(resultImg, unWarpPersImg);
	resultImg.update();
	unWarpPersImg.update();
}

void ofApp::draw() {
	ofSetColor(255);
	cam.draw(640, 0);
	undistorted.draw(640, 480);
	resultImg.draw(0, 0);
	unWarpPersImg.draw(0, 480);
	threadedContourFinder.draw();

	ofDrawBitmapStringHighlight("MainImage", 4, 14, ofColor(128,0,0));
	ofDrawBitmapStringHighlight("ProcessingImage", 4, 480 + 14, ofColor(0, 128, 0));
	ofDrawBitmapStringHighlight("RawImage", 640 + 4, 14, ofColor(0, 0, 128));
	ofDrawBitmapStringHighlight("UndistortedImage", 640 + 4, 480 + 14, ofColor(128, 128, 0));
}

void ofApp::drawGui(ofEventArgs& args) {
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'g') {
		saveParam();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (threadedContourFinder.getCalibMode()) {
		threadedContourFinder.moveClosestPoint(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (threadedContourFinder.getCalibMode()) {
		threadedContourFinder.pickClosestPoint(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if (threadedContourFinder.getCalibMode()) {
		threadedContourFinder.setCalib();
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::loadParam() {
	nlohmann::json j;
	std::ifstream ifs("data.json");
	if (!ifs)
		return;
	ifs >> j;
	ifs.close();

	std::vector<ofVec2f> circles;
	for (size_t i = 0; i < 4; i++) {
		circles.push_back(ofVec2f(j["circles"][i * 2], j["circles"][i * 2 + 1]));
	}
	threadedContourFinder.setPerspective(circles);

	threshold = j["contourFinder"]["threshold"];
	minAreaRadius = j["contourFinder"]["minAreaRadius"];
	maxAreaRadius = j["contourFinder"]["maxAreaRadius"];
	gamma = j["contourFinder"]["gamma"];
	invertThreshold = j["contourFinder"]["invert"];
}

void ofApp::saveParam() {
	nlohmann::json j;
	std::array<float, 8> circles;
	for (size_t i = 0; i < 4; i++) {
		ofVec2f p = threadedContourFinder.pts_src[i];
		circles[i * 2] = p.x;
		circles[i * 2 + 1] = p.y;
	}
	j["circles"] = circles;
	j["contourFinder"]["threshold"] = (float)this->threshold;
	j["contourFinder"]["minAreaRadius"] = (float)this->minAreaRadius;
	j["contourFinder"]["maxAreaRadius"] = (float)this->maxAreaRadius;
	j["contourFinder"]["gamma"] = (float)this->gamma;
	j["contourFinder"]["invert"] = (bool)this->invertThreshold;

	std::ofstream ofs("data.json");
	ofs << j.dump(4) << std::endl;
	ofs.close();
}
