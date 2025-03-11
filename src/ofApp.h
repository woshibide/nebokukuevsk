/**
 *
 * OFDevCon Example Code Sprint
 * Model Distort Example
 *
 * This example loads a model and distorts it using noise
 *
 * The model is the open source and freely licensed balloon dog by Rob Myers, commissioned by furtherfield:
 * http://www.furtherfield.org/projects/balloon-dog-rob-myers
 *
 * Created by James George for openFrameworks workshop at Waves Festival Vienna sponsored by Lichterloh and Pratersauna
 * Adapted during ofDevCon on 2/23/2012
 */

#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxNetwork.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	// shape params
	float violence;
	float amplitude;
	float opacity;

	// easing out
	float currentTime;
	float changeInterval;
	
	// this is our model we'll draw
	ofxAssimpModelLoader model;
	ofEasyCam cam;
	ofLight light;
	void drawWithModel();
	void drawWithMesh();


	// network params
	// pure data
	ofxUDPManager udpConnection;
	ofTrueTypeFont mono;
	ofTrueTypeFont monosm;
	vector<glm::vec3> stroke;

	// raspberry pi
	ofxUDPManager udpReceiver;
	char udpMessage[1024];
};
