#include "pano.h"
#include "ofApp.h"

void Pano::setup()
{ 
	// defaults
	width = 1024;
	height = 1024;

	displayIndex = 0;

	// sphere holds pano
    sphere.setScale(.5);
    sphere.setResolution(50);
    sphere.setPosition(0, 0, 0);
    sphere.setOrientation(ofVec3f(90,0,0));
    sphere.mapTexCoords(1, 0, 0, 1);
    
	// domemaster render
    domemaster.setup();
    domemaster.setCameraPosition(0,0,10);
    
	// image buffer 
    bufferCnt = 3;
    for (int i=0; i<bufferCnt; i++)
	{
        ofImage img;
        images.push_back(img);
        display.push_back(i);
        fIndex.push_back(i);
    }

	objectHeightCorrection = 0.7f;
	position = glm::vec2(0);
	deviation = glm::vec2(0);
	flip = true;
}

void Pano::update()
{
    if (startup && images[display[0]].getTextureReference().isAllocated()){
        startup = false;
	}
    
    if (files.size() == 0){
        loading = false;
    }
    else if (files.size() == 1){
        if (images[display[0]].getTextureReference().isAllocated())
            loading = false;
        else
            loading = true;
    }
    else if (files.size() == 2){
        if (images[display[0]].getTextureReference().isAllocated() && images[display[1]].getTextureReference().isAllocated())
            loading = false;
        else
            loading = true;
    }
    else {
        if (images[display[0]].getTextureReference().isAllocated() && images[display[1]].getTextureReference().isAllocated() && images[display[2]].getTextureReference().isAllocated())
            loading = false;
        else
            loading = true;
    }
}

void Pano::updatePosition(glm::vec2 pos)
{
	position = pos;
}

void Pano::updateObjectDistance(float dist)
{
	objectDistance = dist;
}

void Pano::updateXCorrection(float x, bool released)
{
	tempXRotCorrection = xRotCorrection + x;
	if (released)
	{
		xRotCorrection = tempXRotCorrection;
		mouseLastReleased = true;
	}
}

void Pano::setFullScreen(bool b)
{
	bFullScreen = b;
	domemaster.setFullScreen(b);
}

void Pano::setFlip(bool b)
{
	flip = b;
}

void Pano::setTexture(ofTexture* tex)
{
	videoTexture = tex;
}

void Pano::draw()
{
    ofBackground(0);
    
    if (startup)
        ofSetColor(45);
    else
        ofSetColor(255);
    
	// draw sphere
    ofEnableNormalizedTexCoords();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

	// z = eye of camera, y = up, x = right
	glm::mat4 rot = glm::mat4();
	rot = glm::rotate(rot, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	rot = glm::rotate(rot, glm::radians(position.x - deviation.x), glm::vec3(0.0f, -1.0f, 0.0f));
	rot = glm::rotate(rot, glm::radians(position.y - deviation.y), glm::vec3(-1.0f, 0.0f, 0.0f));
	rot = glm::rotate(rot, glm::radians(tempXRotCorrection + (flip) ? 180.0f : 0), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat quatrot = glm::toQuat(rot);

	objectDistance *= ((bFullScreen) ? objectHeightCorrection : 1.0f);

	if (ofApp::bUseVideo) videoTexture->bind();
	else images[0].bind();

	for (int i=0; i<domemaster.renderCount; i++)
	{
		domemaster.begin(i);
		ofPushMatrix();
		sphere.setScale(glm::vec3(objectDistance));
		sphere.setGlobalOrientation(quatrot);
		sphere.draw();
		ofPopMatrix();
		domemaster.end(i);
	}

	if (ofApp::bUseVideo) videoTexture->unbind();
	else images[0].unbind();

    glDisable(GL_CULL_FACE);
    ofDisableNormalizedTexCoords();

	// draw domemaster
    domemaster.draw();
    
	if (ofApp::bShowPreview)
	{
		ofSetColor(0, 0, 0, 128);
		ofFill();
		ofRect(0, 0, 200, 200);

		ofSetColor(255);
		ofNoFill();
		ofDrawBitmapString("WORMCHAT v0.3.1", 20, 20);
		ofDrawBitmapString("p :toggle preview", 20, 60);
		ofDrawBitmapString("f: toggle fullscreen", 20, 80);
		ofDrawBitmapString("r: reset orientation", 20, 100);
		ofDrawBitmapString("x: exit", 20, 120);

		ostringstream s0, s1, s2;
		s0 << "pos: " << position.x << ", " << position.y;
		s1 << "height: " << objectDistance;
		s2 << "correction: " << tempXRotCorrection;
		ofDrawBitmapString(s0.str(), 20, 140);
		ofDrawBitmapString(s1.str(), 20, 160);
		ofDrawBitmapString(s2.str(), 20, 180);
	}
	ofSetColor(0, 0, 0, 128);
	ofFill();
	ofRect(200, ofGetWindowSize().y-100, 640, 60);

	ofSetColor(255);
	ofDrawBitmapString("Wormhole opened successfully.", 220, ofGetWindowSize().y-80);
	ofDrawBitmapString("Press SPACE to return to idle for the next passerby.", 220, ofGetWindowSize().y-60);
	if (loading)
	{
		ofDrawBitmapString("loading...", 20, ofGetWindowHeight() - 40);
	}
}

void Pano::move(float x, float y, float z)
{
	sphere.move(x, y, z);
}

void Pano::rotate(float degrees, float vx, float vy, float vz)
{
	sphere.rotate(degrees, vx, vy, vz);
}

void Pano::reset()
{
	// calibrate
	deviation = position;
}

void Pano::windowResized(int w, int h)
{
    height = ofGetWindowHeight();
    width = height;
    domemaster.resize(width, height);
}

void Pano::loadImage(string fname)
{
	loader.loadFromDisk(images[0], fname);
}

void Pano::exit()
{
    loader.waitForThread();
}
