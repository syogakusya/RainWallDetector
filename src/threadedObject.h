#pragma once

#include "ofMain.h"
#include <atomic>
#include "ofxCv.h"
#include "ofApp.h"

class ThreadedObject : public ofThread
{
public:

	~ThreadedObject() {
		stop();
		waitForThread(false);
	}

	void setup() {
		reset_Circle();
		start();
	}

	void start() {
		startThread();
	}

	void stop() {
		std::unique_lock<std::mutex> lock(mutex);
		stopThread();
		condition.notify_all();
	}

	void threadedFunction() {
		while (isThreadRunning()) {
			threadFrameNum++;
			std::unique_lock<std::mutex> lock(mutex);
			
			image = ofxCv::toCv(*undistortedImage);
			cv::cvtColor(image.clone(), gray, cv::COLOR_RGB2GRAY);
			cv::warpPerspective(gray, gray, perspectiveMat, gray.size(), cv::INTER_NEAREST);
			cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0, 0);
			adjustGamma(gray, gamma);
			resultMat = (isCalibMode) ? image.clone() : gray.clone();
			unWarpPersMat = gray.clone();
			contourFinder.findContours(gray);

			condition.wait(lock);
		}
	}

	void update() {
		std::unique_lock <std::mutex> lock(mutex);
		setCalibMode(isCalibMode);
		condition.notify_all();
	}

	void draw() {
		ofPushStyle();
		ofSetColor(0, 255, 255);
		ofSetLineWidth(2);
		contourFinder.draw();
		ofPopStyle();
		drawSrcCircle();
	}

	int getThreadFrameNum() {
		return threadFrameNum;
	}

	void setImage(ofImage* img) {
		undistortedImage = img;
	}

	void adjustGamma(cv::Mat& img, float gamma) {
		uchar LUT[256];
		for (int i = 0; i < 256; i++) {
			LUT[i] = (int)(pow((double)i / 255.0, gamma) * 255.0);
		}
		cv::Mat lookUpTable = cv::Mat(1, 256, CV_8UC1, LUT);
		cv::LUT(img, lookUpTable, img);
	}

	void setParameter(
		float threshold_,
		float gamma_,
		float maxAreaRaduis_,
		float minAreaRaduis_,
		bool isCalibMode_,
		bool invertThreshold_) 
	{
		std::unique_lock<std::mutex> lock(mutex);

		threshold = threshold_;
		gamma = gamma_;
		maxAreaRaduis = maxAreaRaduis_;
		minAreaRaduis = minAreaRaduis_;
		isCalibMode = isCalibMode_;

		contourFinder.setThreshold(threshold);
		contourFinder.setMinAreaRadius(minAreaRaduis);
		contourFinder.setMaxAreaRadius(maxAreaRaduis);
		contourFinder.setInvert(invertThreshold_);
	}

	void getContourImage(ofImage& resultImage, ofImage& unWarpPersImage) {
		std::unique_lock<std::mutex> lock(mutex);
		ofxCv::toOf(resultMat, resultImage);
		ofxCv::toOf(unWarpPersMat, unWarpPersImage);
	}


	void setCalibMode(bool calibMode) {
		if (isCalibMode != calibMode) {
			lock();
			isCalibMode = calibMode;
			pickedCircle = -1;
			unlock();
		}
	}

	bool getCalibMode() {
		return isCalibMode;
	}


	void moveClosestPoint(int x, int y) {
		if (pickedCircle > -1) {
			pts_src[pickedCircle] = ofVec2f(x, y) + pickOffset;
		}
	}

	void pickClosestPoint(int x, int y) {
		float cls = INFINITY;
		for (size_t i = 0; i < 4; i++) {
			ofVec2f v = pts_src[i] - ofVec2f(x, y);
			float d = v.length();
			if (cls > d) {
				pickedCircle = i;
				cls = d;
				pickOffset = v;
			}
		}
	}

	void setPerspective(std::vector<ofVec2f> circles) {
		if (circles.size() != 4) return;

		pts_src.clear();
		std::vector<cv::Point2f> src;
		for (size_t i = 0; i < 4; i++) {
			pts_src.push_back(circles[i]);
			src.push_back(cv::Point2f(circles[i].x, circles[i].y));
		}

		std::vector<cv::Point2f> dst;
		dst.push_back(cv::Point2f(0, 0));
		dst.push_back(cv::Point2f(640 - 1, 0));
		dst.push_back(cv::Point2f(640 - 1, 480 - 1));
		dst.push_back(cv::Point2f(0, 480 - 1));

		lock();
		perspectiveMat = cv::getPerspectiveTransform(src, dst);
		unlock();
	}

	void setCalib() {
		setPerspective(pts_src);
	}

	std::vector<ofVec2f>pts_src;

protected:
	int threadFrameNum = 0;
	std::condition_variable condition;

private:


	void drawSrcCircle() {
		if (isCalibMode) {
			ofPushStyle();
			ofNoFill();
			ofSetColor(255, 0, 0);
			ofDrawCircle(pts_src[0], 10);
			ofDrawBitmapString("0", pts_src[0]);
			ofDrawLine(pts_src[0], pts_src[1]);

			ofSetColor(0, 255, 0);
			ofDrawCircle(pts_src[1], 10);
			ofDrawBitmapString("1", pts_src[1]);
			ofDrawLine(pts_src[1], pts_src[2]);

			ofSetColor(0, 0, 255);
			ofDrawCircle(pts_src[2], 10);
			ofDrawBitmapString("2", pts_src[2]);
			ofDrawLine(pts_src[2], pts_src[3]);

			ofSetColor(255, 255, 0);
			ofDrawCircle(pts_src[3], 10);
			ofDrawBitmapString("3", pts_src[3]);
			ofDrawLine(pts_src[3], pts_src[0]);
			ofFill();
			ofPopStyle();
		}
	}

	void reset_Circle() {
		pts_src.clear();
		pts_src.push_back(ofVec2f(0, 0));
		pts_src.push_back(ofVec2f(640 - 1, 0));
		pts_src.push_back(ofVec2f(640 - 1, 480 - 1));
		pts_src.push_back(ofVec2f(0, 480 - 1));
		setPerspective(pts_src);
	}

	ofImage* undistortedImage = nullptr;
	cv::Mat image, gray, resultMat, unWarpPersMat;
	ofxCv::ContourFinder contourFinder;

	bool isCalibMode;
	float threshold;
	float gamma;
	float maxAreaRaduis;
	float minAreaRaduis;

	int pickedCircle;
	ofVec2f pickOffset;
	cv::Mat perspectiveMat;
};