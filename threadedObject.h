#pragma once

#include "ofMain.h"
#include <atomic>

class ThreadedObject : public ofThread
{
public:
	~ThreadedObject() {
		stop();
		waitForThread(false);
	}

	void setup() {

	}

	void start() {

	}

	void stop() {

	}

	void threadedFunction() {
		while (isThreadRunning()) {

		}
	}

	void update() {

	}

	void draw() {

	}

protected:

};