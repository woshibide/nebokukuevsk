/**
 *
 * Adaptation of OFDevCon Example Code Sprint
 *
 * Created by James George for openFrameworks workshop at Waves Festival Vienna sponsored by Lichterloh and Pratersauna
 * Adapted during ofDevCon on 2/23/2012
 *
 * Now adapted by Pyotr Goloub for the Metamorphoses 2025 exhibition
 *
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

	// utilities
	float smoothRemapper(float outputMin, float outputMax);
	float smoothStep(float x);

	// shape params
	float violence;
	float amplitude;
	float opacity;
	float speedDampen;

	// easing out
	float easeAmount;
	float currentTime;
	float changeInterval;

	float timeSinceChange;
	float normalizedTime;
	float easedTime;

	// transitions
	bool isTransitioning;
	float transitionStart;
	float transitionDuration;
	float startChangeInterval, startHbOffset, startHbAmp, startHbFreq, startHbPhase;
	float targetChangeInterval, targetHbOffset, targetHbAmp, targetHbFreq, targetHbPhase;

	// this is our model we'll draw
	ofxAssimpModelLoader model;
	void drawWithMesh();
	ofEasyCam cam;
	float camDis;

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
