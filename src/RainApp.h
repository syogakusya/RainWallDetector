#pragma once

#include "ofMain.h"

class RainApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);

	bool bFullScreen;
};