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

void testApp::initVivarium(int numCreatures, int avgLifespan, int numSugars, int avgSugar)
{
	//create "sugarscape"
	for (int i=0; i<numSugars; i++)
	{
		//get starting position somewhere inside unit sphere
		//3 random numbers and normalize
		ofVec3f *startPos = new ofVec3f(ofRandom(-100, 100), ofRandom(-100, 100), ofRandom(-100, 100));
		startPos->normalize();
		
		int energy = avgSugar; //TODO: gaussian sugar distribution
		sugarPackets.push_back(new SugarPacket(energy, startPos));
	}

	for (int i=0; i<numCreatures; i++)
	{
		int lifespan = avgLifespan; //TODO: gaussian lifespan distro
		
		//everyone start at bottom
		ofVec3f *startPos = new ofVec3f(0, -1, 0);

		//ok that it's 100 because we are normalizing
		ofVec3f *startVel = new ofVec3f(ofRandom(100), ofRandom(100), ofRandom(100));
		startVel->normalize();
		*startVel /= 100.0f; //slow down, dude

		creatures.push_back(new Creature(lifespan, startPos, startVel));
	}
}

//GL specific settings
void testApp::initGL()
{
	glEnable(GL_DEPTH_TEST);

	//GLUquadricObj *
	quadratic = gluNewQuadric();
	gluQuadricNormals(quadratic, GLU_SMOOTH);
	//gluQuadricDrawStyle(quadratic, GLU_FILL);

	/*
	//Making a display list to draw sphere
	//http://www.opengl.org/wiki/Texturing_a_Sphere
	sphere = glGenLists(1);
	glNewList(sphere, GL_COMPILE);
	gluSphere(quadratic, 1.0, 20, 20);
	glEndList();
	*/
	gluDeleteQuadric(quadratic);
}

void testApp::updateCritters()
{
	for (int i = 0; i < sugarPackets.size(); i++)
	{
		if (!sugarPackets.at(i)) continue;
		
		//every sugar packet pulls on every creature
		for (int j = 0; j < creatures.size(); j++)
		{
			if (creatures[j])
				creatures.at(j)->goTowards(sugarPackets.at(i));
		}
	}

	//iterate thru creatures
	for (int i = 0; i < creatures.size(); i++)
	{
		//TODO: prune creatures
		if (creatures.at(i) && creatures.at(i)->alive)
		{			
			//every creature pulls on desperate creature
			for (int j = 0; j < creatures.size(); j++)
			{
				if (creatures.at(j) && creatures.at(j)->desperate() && creatures.at(j)->alive)
					creatures.at(j)->goTowards(creatures.at(i));
			}
			
			//update creature position
			//*(creatures.at(i)->pos) += *(creatures.at(i)->vel);
			
			//if timer % tick time == 0
			//creatures.at(i)->ttl -= 1;
			if (creatures.at(i)->ttl <= 0) creatures.at(i)->die();
		}
		//else {} //remove dead creatures from list - need better iter
	}
}

void testApp::initCV()
{
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
		
    //vidGrabber.setVerbose(true);
    //vidGrabber.initGrabber(camWidth,camHeight);

	//numCreatures, avgLifespan, numSugars, avgSugar
	initVivarium(50, 120, 25, 40);
	
	initCV();
	initGL();

	ofSetupScreen();

	ofBackgroundHex(0x333333, 1.0);
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
	
	//updateCritters();
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofSetHexColor(0xffffff);
	
	//gluLookAt(0, 0, -1,
	//		  ofGetWidth()/2, ofGetHeight()/2, 0,
	//		  0, 1, 0);
	

	/*	
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
	*/
	
	//draw creatures
	ofPushMatrix();
	//ofTranslate(0, 0, -50); //move camera back

	//put 0,0,0 at center of the screen
	ofTranslate(ofGetWidth()/2, ofGetHeight()/2, -50);
	//ofScale(0, -1, 0); //flip y coordinates - don't work for sum reason...
	ofScale(25, 25, 25); //each 1.0 unit is now ~25px (i think :-)
	
	ofPushStyle();
	//ofSphere(0, 0, 0, 0.2);
	//ofSphere(1, 0, 0, 0.2);
	//ofSphere(0, -1, 0, 0.2);

	for (int i = 0; i < sugarPackets.size(); i++)
	{
		if (!sugarPackets.at(i)) continue;
		
		ofPushMatrix();
		ofVec3f *pos = sugarPackets.at(i)->pos;
		
		//draw sumthin' - sugar cubes?
		ofSetHexColor(0xffffff);
		float f = 10;
		ofSphere(f*pos->x, f*pos->y, f*pos->z, 0.2);
		
		ofPopMatrix();
	}

	for (int i = 0; i < creatures.size(); i++)
	{
		if (creatures.at(i) == NULL || creatures.at(i)->alive == false) continue;

		ofPushStyle();

		ofVec3f *pos = creatures.at(i)->pos;
		ofSetHexColor(creatures.at(i)->color());
		//ofSetHexColor(0xffffff);

		float f = 1;
		ofSphere(f*pos->x, f*pos->y, f*pos->z, 1.0);
		
		/*
		for (int j=0; j < creatures.size(); j++)
		{
			if (creatures.at(j) == NULL || creatures.at(j)->alive == false) continue;
			//draw 3d line between i and j
		}
		 */

		ofPopStyle();
	}
	ofPopMatrix();
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
