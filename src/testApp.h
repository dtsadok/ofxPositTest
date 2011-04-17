#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"
#include "ofxSynth.h"
#include "ofxXmlSettings.h"
#include "ofx3DModelLoader.h"
//#include "ofxAssimpModelLoader.h"
#include "ofxThreadedImageLoader.h"

//#def _DEBUG 1

class testApp : public ofBaseApp{
	
public:
	
	//from: http://channel9.msdn.com/coding4fun/articles/Wiimote-Virtual-Reality-Desktop
	float distanceToLine(ofVec2f point, ofVec2f startLinePoint, ofVec2f endLinePoint, float *lambda);
	void testPoints(ofVec2f lineStartPoint, ofVec2f lineEndPoint, ofVec2f onLinePoint, ofVec2f freePoint);
	
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);		
	
	// we don't actually use these
	// just checking to see if they
	// all work in the same place :)
	/*
	ofxCvGrayscaleImage cvGray;
	ofxTCPClient client;
	ofxTCPServer server;
	ofxOscSender osc_sender;
	ofxXmlSettings settings;
	ofx3DModelLoader modelLoader;
	//ofxAssimpModelLoader betterModelLoader;
	ofxSynth synth;
	ofxThreadedImageLoader threadedLoader;
	 */
	
	//From: http://wiki.openframeworks.cc/index.php?title=Color_Tracking
	ofVideoGrabber vidGrabber;
	ofImage inputImage;
	int camWidth;
	int camHeight;
	
	int briThresh;
	int numBlobs;
	int nPixels; //only need to compute once...
	
	//for POSIT
	float recordDistance; //for line detection algorithm
	ofVec2f imagePoints[4];

	ofxCvGrayscaleImage trackedImg;
	unsigned char *colorTrackedPixels;
	ofTexture trackedTexture;
	ofxCvContourFinder cvFinder;
	
	ofxCvColorImage colorImg;
	ofxCvColorImage colorImgHSV;
	
	ofxCvGrayscaleImage hueImg;
	ofxCvGrayscaleImage satImg;
	ofxCvGrayscaleImage briImg;
};