#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"
#include "RainApp.h"

//========================================================================
int main( ){

	ofGLFWWindowSettings settings;
	settings.setSize(640 * 2, 480 * 2);
	settings.resizable = false;
	auto mainWindow = ofCreateWindow(settings);
	
	settings.setSize(300, 300);
	settings.resizable = false;
	auto guiWindow = ofCreateWindow(settings);

	settings.setSize(1280, 960);
	settings.resizable = true;
	settings.windowMode = OF_WINDOW;
	auto rainWindow = ofCreateWindow(settings);

	guiWindow->setVerticalSync(false);
	rainWindow->setVerticalSync(false);

	auto mainApp = make_shared<ofApp>();
	mainApp->setupGui();
	ofAddListener(guiWindow->events().draw, mainApp.get(), &ofApp::drawGui);//(発火する原因、行いたいイベントの参照元（インスタンス）、行うインスタンスメソッド)
																			//ここではguiWindowのdraw（mainWindowとは別に毎フレーム呼び出される？）、ofApp、ofApp::drawGuiである
	auto rainApp = make_shared<RainApp>();
	


	ofRunApp(mainWindow, mainApp);
	ofRunApp(rainWindow, rainApp);
	ofRunMainLoop();

}
