#pragma once

#include "ofxDomemaster.h"
#include "ofxThreadedImageLoader.h"

class Pano 
{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void windowResized(int w, int h);
	void loadImage(string fname);
    
    void reset();
	void move(float x, float y, float z);
	void rotate(float degrees, float vx, float vy, float vz);

	void updatePosition(glm::vec2 pos);
	void updateObjectDistance(float dist);
	void updateXCorrection(float x, bool released);
	void setFullScreen(bool b);
	void setFlip(bool b);

	void setTexture(ofTexture* tex);

private:
    ofxDomemaster domemaster;
    ofSpherePrimitive sphere;

	ofTexture* videoTexture;
    
    ofxThreadedImageLoader loader;
    vector<ofImage> images;
    int total;
    int fileIndex;

	int width; 
	int height;

	vector<string> files;
	vector<int> display;
	vector<int> fIndex;

	glm::vec2 position;
	glm::vec2 deviation;

	float objectDistance;
	float objectHeightCorrection;
	float xRotCorrection;
	float tempXRotCorrection;

	int bufferCnt;
	int displayIndex;

	bool mouseLastReleased;
	bool bFullScreen;
	bool loading;
	bool startup;
	bool flip;
};
