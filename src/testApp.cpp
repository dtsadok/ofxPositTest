#include "testApp.h"

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
void testApp::draw(){
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
	
    for (int i=0; i < cvFinder.blobs.size(); i++)
	{
        char tempStr1[255];
        sprintf(tempStr1, "x : %f\ny : %f", cvFinder.blobs[i].centroid.x, cvFinder.blobs[i].centroid.y);  
        ofDrawBitmapString(tempStr1, 20, 250 + 100*i); //draw the string
    }
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
