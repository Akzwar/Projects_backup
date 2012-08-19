#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"


//========================================================================
int main( ){

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1280,1024, OF_FULLSCREEN);
    ofSetBackgroundAuto(false);
	testApp* myapp = new testApp;
	ofRunApp(myapp);
	myapp->~testApp();
	return 0;
}
