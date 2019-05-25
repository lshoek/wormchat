#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofAppGLFWWindow.h"
#include "ofxBiquadFilter.h"
#include "pano.h"

class ofApp : public ofBaseApp
{  
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void windowResized(int w, int h);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	void drawFinderFlipped();
	void loadVideo();
	void toggleFullScreen();
	void setIdle(bool b);

	Pano pano;
	ofAppGLFWWindow *glfw;

	ofxBiquadFilter2f lpFilterFacePosition;
	ofxBiquadFilter2f lpFilterFaceSize;

	ofxCv::ObjectFinder finder;
	ofVideoGrabber videoGrabber;
	ofDirectShowPlayer videoPlayer;

	ofTexture videoTexture;
	ofTexture* videoTexturePtr;
	ofImage wormholeImage;
	ofDirectory videoDir;

	glm::vec2 previewSize;
	ofRectangle faceRect;
	float calibRectSize;

	float turningRate;
	float minRelativeFaceHeight;
	float maxRelativeFaceHeight;

	float fc;
	static bool bFullscreen;
	static bool bShowPreview;
	static bool bUseVideo;

	glm::vec2 mousePressedLocation;
	bool mouseLastReleased;
	bool idle;
};
