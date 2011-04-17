#include "testApp.h"

float testApp::average(float *values, int sz)
{
	float sum;
	
	for (int i=0; i < sz; i++)
	{
		sum += values[i];
	}

	return sum/sz;
}

//draw a line between startPoint and endPoint, and see if point is on that line
float testApp::distanceToLine(ofVec2f point, ofVec2f startPoint, ofVec2f endPoint, float *lambda)
{
    ofVec2f rv = endPoint - startPoint;
    ofVec2f p_ap = point - startPoint;
	float dot_rv = rv.dot(rv);
    *lambda = p_ap.dot(rv / dot_rv);
    ofVec2f distVec = point - (startPoint + *lambda * rv);
	
	return distVec.length();
}

//figure out which are three points on line (start, in, end), and which is fourth point
//"find the 3 image points which are [closest] to form[ing] a line"
void testApp::testPoints(ofVec2f lineStartPoint, ofVec2f lineEndPoint, ofVec2f inPoint, ofVec2f freePoint)
{
    float lambda;
    float dist = distanceToLine(inPoint, lineStartPoint, lineEndPoint, &lambda);
	
    // check if projected point is between line end and start point
    if (lambda > 0 && lambda < 1)
    {
        // if distance is short, make this combination the result
        if (dist < recordDistance)
        {
            recordDistance = dist;
            imagePoints[0] = lineStartPoint;
            imagePoints[1] = lineEndPoint;
            imagePoints[2] = inPoint;
            imagePoints[3] = freePoint;
        }
    }
}

void testApp::initParticles()
{
	particleCount = 0;
	for(int i=0; i<NUM_PARTICLES; i++) {
		particlePos[i] = 0;
		particleVel[i] = 0;
		particleColor[i].set(1, 0);
		particleTime[i] = ofRandom(1.0);
	}
	
	particleVbo.setColorData(particleColor, NUM_PARTICLES, GL_STATIC_DRAW);
	particleVbo.setVertexData(particlePos, NUM_PARTICLES, GL_DYNAMIC_DRAW);
}

//--------------------------------------------------------------
void testApp::setup()
{
    camWidth = 640;
    camHeight = 480;
	
	inputImage.loadImage("triangle1.png");
	camWidth = inputImage.width;
	camHeight = inputImage.height;
	
	nPixels = camWidth * camHeight;
	
	ofSetWindowShape(camWidth, camHeight);
	
	//how bright is bright
	briThresh = 150;
	
	//how many blobs are we expecting?
	numBlobs = 3;
	
	//openCV buffers
    colorImg.allocate(camWidth, camHeight);
	colorImgHSV.allocate(camWidth, camHeight);
    hueImg.allocate(camWidth, camHeight);
    satImg.allocate(camWidth, camHeight);
    briImg.allocate(camWidth, camHeight);
    trackedImg.allocate(camWidth, camHeight);
    colorTrackedPixels = new unsigned char[camWidth * camHeight];
    trackedTexture.allocate(camWidth, camHeight, GL_LUMINANCE);
	
	
    //vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(camWidth,camHeight);
}

//--------------------------------------------------------------
void testApp::update()
{
	//vidGrabber.grabFrame();
	
	//TODO: blur?
	
	//colorImg.setFromPixels(vidGrabber.getPixels(), camWidth, camHeight);
	colorImg.setFromPixels(inputImage.getPixels(), camWidth, camHeight);
	colorImgHSV = colorImg;
	colorImgHSV.convertRgbToHsv();
	colorImgHSV.convertToGrayscalePlanarImages(hueImg, satImg, briImg);
	
	//bug fix, apparently
	hueImg.flagImageChanged();
	satImg.flagImageChanged();
	briImg.flagImageChanged();
	
	//only checking brightness for now
	//unsigned char * huePixels = hueImg.getPixels();
	//unsigned char * satPixels = satImg.getPixels();
	unsigned char * briPixels = briImg.getPixels();
	
	//loop thru pixels, select bright ones
	for (int i = 0; i < nPixels; i++)
	{
		//bright enough?
		if (briPixels[i] > briThresh)
		{
			colorTrackedPixels[i] = 255;
		}
	}
	
	//TODO: blur?
	
	trackedImg.setFromPixels(colorTrackedPixels, camWidth, camHeight);
	//cvFinder.findContours(ofxCvGrayscaleImage input, int minArea, int maxArea, int nConsidered, bool bFindHoles, bool bUseApproximation)
	cvFinder.findContours(trackedImg, 10, nPixels/10, numBlobs, false, true);
	trackedTexture.loadData(colorTrackedPixels, camWidth, camHeight, GL_LUMINANCE);
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackground(100, 100, 100);
	
    ofSetHexColor(0xffffff);
    //vidGrabber.draw(0, 0);
	inputImage.draw(0, 0);
	
	//colorImgHSV.draw(camWidth+20, 0);
	
    //trackedTexture.draw(20, camWidth*2 + 20);
	trackedTexture.draw(0, 0);
	
    //ofDrawBitmapString("red",20, 280); //label
    cvFinder.draw();
	
    glPushMatrix();
	//glTranslatef(0, 0, 0);
	cvFinder.draw();
    glPopMatrix();
	
	int sz = cvFinder.blobs.size();
    for (int i=0; i < sz-1; i++)
	{
		ofLine(cvFinder.blobs[i].centroid.x, cvFinder.blobs[i].centroid.y, cvFinder.blobs[i+1].centroid.x, cvFinder.blobs[i+1].centroid.y);
    }
	ofLine(cvFinder.blobs[sz-1].centroid.x, cvFinder.blobs[sz-1].centroid.y, cvFinder.blobs[0].centroid.x, cvFinder.blobs[0].centroid.y);
	
	// particles
	//update the vbo
	particleVbo.setColorData(particleColor, NUM_PARTICLES, GL_STATIC_DRAW);
	particleVbo.setVertexData(particlePos, NUM_PARTICLES, GL_DYNAMIC_DRAW);
	// and draw
	particleVbo.draw(GL_POINTS, 0, NUM_PARTICLES);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
	
}
