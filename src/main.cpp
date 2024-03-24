#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){

	ofGLFWWindowSettings settings;
	settings.setSize(640 * 2, 480 * 2);
	settings.resizable = false;
	auto mainWindow = ofCreateWindow(settings);
	
	settings.setSize(300, 300);
	settings.resizable = false;
	auto guiWindow = ofCreateWindow(settings);

	guiWindow->setVerticalSync(false);

	auto mainApp = make_shared<ofApp>();
	mainApp->setupGui();
	ofAddListener(guiWindow->events().draw, mainApp.get(), &ofApp::drawGui);//(���΂��錴���A�s�������C�x���g�̎Q�ƌ��i�C���X�^���X�j�A�s���C���X�^���X���\�b�h)
																			//�����ł�guiWindow��draw�imainWindow�Ƃ͕ʂɖ��t���[���Ăяo�����H�j�AofApp�AofApp::drawGui�ł���

	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();

}
