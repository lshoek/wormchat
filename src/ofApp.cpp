#include "ofApp.h"

bool ofApp::bFullscreen = false;
bool ofApp::bShowPreview = false;
bool ofApp::bUseVideo = true;

void ofApp::setup()
{
	ofSetVerticalSync(true);
    glfw = (ofAppGLFWWindow*)ofGetWindowPtr();

	finder.setFindBiggestObject(true);
	finder.setMultiScaleFactor(1.2);
	finder.setMinSizeScale(0.25);
	finder.setMaxSizeScale(0.75);
	finder.setRescale(0.75);
	finder.setCannyPruning(true);
	finder.setup("haarcascades/haarcascade_frontalface_default.xml");

	videoGrabber.setPixelFormat(OF_PIXELS_NATIVE);
	videoGrabber.setup(640, 360);
	previewSize = glm::vec2(160, 90);

	pano.setup();
	toggleFullScreen();

	if (bUseVideo)
	{
		//videoPlayer.setPixelFormat(OF_PIXELS_RGB);
		//videoPlayer.setLoopState(ofLoopType::OF_LOOP_NORMAL);
	}
	else
	{
		pano.loadImage("images/room.jpg");
	}
	wormholeImage.load("images/wormhole.png");
	videoDir.open("videos");

	fc = 0.1f;
	lpFilterFacePosition.setType(OFX_BIQUAD_TYPE_LOWPASS);
	lpFilterFaceSize.setType(OFX_BIQUAD_TYPE_LOWPASS);
	lpFilterFacePosition.setFc(fc);
	lpFilterFaceSize.setFc(fc);

	turningRate = 0.325f;
	minRelativeFaceHeight = 0.1f;
	maxRelativeFaceHeight = 0.75f;
	calibRectSize = 128.0f;

	mouseLastReleased = true;
    bFullscreen = false;
	idle = true;
}

void ofApp::update()
{
	videoGrabber.update();
	if (bUseVideo && idle == false)
	{
		videoPlayer.update();
	}
	if (videoGrabber.isFrameNew())
	{
		videoGrabber.update();
		finder.update(videoGrabber);

		if (finder.size() > 0)
		{
			faceRect = finder.getObjectSmoothed(0);
			lpFilterFacePosition.update(faceRect.position);
			lpFilterFaceSize.update(glm::vec2(faceRect.height, 0));
		}
		if (idle && faceRect.position.x != 0)
		{
			glm::vec2 fpos = glm::vec2(faceRect.position.x + faceRect.width / 2, faceRect.position.y + faceRect.height / 2);
			glm::vec2 vidcenter = glm::vec2(videoGrabber.getWidth() / 2, videoGrabber.getHeight() / 2);
			float diff = glm::distance(fpos, vidcenter);
			if (diff < 16.0f)
			{
				setIdle(false);
				pano.reset();
			}
		}
	}
	if (ofGetMousePressed()) pano.updateXCorrection((mousePressedLocation.x - ofGetMouseX())/2, false);

	float min = minRelativeFaceHeight * videoGrabber.getHeight();
	float max = maxRelativeFaceHeight * videoGrabber.getHeight();
	pano.updatePosition(glm::vec2(lpFilterFacePosition.value().x * turningRate, lpFilterFacePosition.value().y*turningRate*1.25f));
	pano.updateObjectDistance(glm::clamp(ofMap(lpFilterFaceSize.value().x, min, max, 1.5f, 0.0f), 0.5f, 2.0f));
	pano.update();
}

void ofApp::draw()
{
	ofBackground(16);
	ofSetColor(255);

	bool newFrame = false;
	if (bUseVideo && idle==false /*&& videoPlayer.isFrameNew()*/)
	{
		videoTexture.loadData(videoPlayer.getPixels());
		videoTexturePtr = &videoTexture;
		pano.setTexture(videoTexturePtr);
		newFrame = true;
	}
	if (idle)
	{
		ofSetColor(255);
		wormholeImage.draw(20, 100, wormholeImage.getWidth(), wormholeImage.getHeight());

		ofDrawBitmapString("Welcome to WORMCHAT v0.3.1", 40, 40);
		ofDrawBitmapString("A bi-directional wormhole communication system.", 40, 60);
		ofDrawBitmapString("Please line up your face with the red square to proceed.", 40, 100);

		ofPushMatrix();

		ofTranslate(ofGetWindowSize().x/2-videoGrabber.getWidth()/2, ofGetWindowSize().y/2-videoGrabber.getHeight()/2);
		videoGrabber.draw(videoGrabber.getWidth(), 0, -videoGrabber.getWidth(), videoGrabber.getHeight());
		drawFinderFlipped();

		ofPopMatrix();

		ofSetColor(255, 0, 0);
		ofRect(ofGetWindowSize().x / 2 - calibRectSize / 2, ofGetWindowSize().y / 2 - calibRectSize / 2, calibRectSize, calibRectSize);
	}
	else
	{
		if (newFrame) pano.draw();
	}

	if (bShowPreview)
	{
		ofSetColor(255);
		videoGrabber.draw(ofGetWindowSize().x - 20, ofGetWindowSize().y - previewSize.y - 20, -previewSize.x, previewSize.y);
		drawFinderFlipped();

		ofNoFill();
		ofRect(0, 0, videoGrabber.getWidth(), videoGrabber.getHeight());
	}
}

void ofApp::drawFinderFlipped()
{
	ofPushMatrix();
	ofNoFill();
	ofRotateY(180.0f);
	ofTranslate(-videoGrabber.getWidth(), 0);
	finder.draw();
	ofRect(0, 0, videoGrabber.getWidth(), videoGrabber.getHeight());
	ofPopMatrix();
}

void ofApp::loadVideo()
{
	vector<ofFile> videos = videoDir.getFiles();
	string randomVid = videos[ofRandom(videos.size())].path();
	videoPlayer.load(randomVid);

	videoTexture.allocate(videoPlayer.getWidth(), videoPlayer.getHeight(), OF_PIXELS_BGR);
	videoPlayer.play();
}

void ofApp::setIdle(bool b) 
{
	if (!b)
	{
		pano.setFlip(ofRandom(1.0f) > 0.5f);
		loadVideo();
	}
	idle = b;
}

void ofApp::toggleFullScreen()
{
	bFullscreen = !bFullscreen;
	pano.setFullScreen(bFullscreen);
	ofSetFullscreen(bFullscreen);
}

void ofApp::mousePressed(int x, int y, int button)
{
	if (mouseLastReleased)
	{
		mousePressedLocation = glm::vec2(ofGetMouseX(), ofGetMouseY());
		mouseLastReleased = false;
	}
}

void ofApp::mouseReleased(int x, int y, int button)
{
	pano.updateXCorrection((mousePressedLocation.x - ofGetMouseX())/2, true);
	mouseLastReleased = true;
}

void ofApp::keyPressed(int key)
{
    if (key == 'f')
	{
		toggleFullScreen();
    }
	else if (key == 'p')
	{
		bShowPreview = !bShowPreview;
	}
	else if (key == 32) // SPACE
	{
		videoPlayer.stop();
		videoPlayer.firstFrame();
		setIdle(true);
	}
	else if (key == 'r')
	{
		pano.reset();
	}
	else if (key == 'x')
	{
		exit();
	}
}

void ofApp::windowResized(int w, int h)
{
	pano.windowResized(w, h);
}

void ofApp::exit()
{
	pano.exit();
}
