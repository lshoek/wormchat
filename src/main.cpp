#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

int main()
{
	ofAppGLFWWindow win;
    ofSetupOpenGL(&win, 1024, 1024, OF_WINDOW);
	ofRunApp(new ofApp());
}
