#include "RainApp.h"

void RainApp::setup() {

}

void RainApp::update() {

}

void RainApp::draw() {

}

void RainApp::keyPressed(int key) {
	if (key == 'f') {

		bFullScreen = !bFullScreen;

		if (!bFullScreen) {
			ofSetFullscreen(false);
		}
		else if (bFullScreen == 1) {
			ofSetFullscreen(true);
		}
	}
}