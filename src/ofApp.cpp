/**
 *
 * OFDevCon Example Code Sprint
 *
 * Created by James George for openFrameworks workshop at Waves Festival Vienna sponsored by Lichterloh and Pratersauna
 * Adapted during ofDevCon on 2/23/2012
 *
 * Now adapted by Pyotr Goloub for the Metamorphoses 2025 exhibition
 *
 */

#include "ofApp.h"
#include <cmath>

// for smooth change
float changeInterval = 2.0f; // heartbeat
// adrenaline spikes
float hbOffset = 1.10f;
float hbAmp = 0.75f;
float hbFreq = 0.80f;
float hbPhase = 30.0f;

float lastChangeTime = 0.0f;

// shape params
float opacity = 255.0f;

float currentViolence = 5.0f;
float currentAmplitude = 5.0f;
float currentSpread = 5.0f;

float targetViolence = 5.0f;
float targetAmplitude = 5.0f;
float targetSpread = 5.0f;

int targetCamDis = 1500;

// network
int messageCounter = 0;
bool isItUDPTime = true;
bool debug = false;

// raspberry pi receiver
float lastUdpSendTime;
float udpSendInterval;

//--------------------------------------------------------------
void ofApp::setup()
{

    ofSetVerticalSync(true);
    // ofEnableAntiAliasing();

    ofSetFrameRate(120);
    ofSetVerticalSync(true);
    ofBackground(0, 0, 0, 0);

    // we need to call this for textures to work on models
    //  ofDisableArbTex();

    // this makes sure that the back of the model doesn't show through the front
    ofEnableDepthTest();

    // now we load our model
    model.loadModel("characters/untitled.obj");
    // model.loadModel("characters/head.dae");
    // model.loadModel("characters/bust.dae");
    model.setPosition(ofGetWidth() * .5, ofGetHeight() * 0.5, 0);
    model.setScale(1.0f, 1.0f, 1.0f);

    cam.setDistance(1500);
    cam.setNearClip(10);
    cam.setFarClip(10000);

    lastChangeTime = ofGetElapsedTimef();

    /****************
     *    Network    *
     *****************/

    // OF talks to pure data via 127.0.0.1:11999
    ofxUDPSettings settings;

    settings.sendTo("127.0.0.1", 11999);
    settings.blocking = false;

    udpReceiver.Create();
    udpReceiver.Bind(12000); // ame port as the Raspberry Pi server
    udpReceiver.SetNonBlocking(true);

    udpConnection.Setup(settings);
}

//--------------------------------------------------------------

void ofApp::update()
{
    // Calculate elapsed time and ease variables
    float currentTime = ofGetElapsedTimef();
    timeSinceChange = currentTime - lastChangeTime;

    if (ofRandom(5000) < 1.0)
    {
        // Reset to base values before applying modifications
        float baseChangeInterval = 1.50f;
        float baseHbOffset = 1.10f;
        float baseHbAmp = 0.75f;
        float baseHbFreq = 0.80f;
        float baseHbPhase = 30.0f;

        int choice = ofRandom(5);

        float targetValues[5] = {baseChangeInterval,
                                 baseHbOffset,
                                 baseHbAmp,
                                 baseHbFreq,
                                 baseHbPhase};

        switch (choice)
        {
        case 0:
            targetValues[0] = ofRandom(0.5f, 10.0f);
            targetValues[1] = targetValues[0] / 2;
            targetValues[2] = targetValues[1] * ofRandom(0.3f, targetValues[0] / 2);
            targetValues[3] = ofRandom(0.1f, 20.0f);
            targetValues[4] = ofRandom(0.5f, 360.0f);
            break;

        case 1:
            targetValues[0] = baseChangeInterval * 2;
            targetValues[1] = baseHbOffset * 2;
            targetValues[2] = baseHbAmp * 2;
            targetValues[3] = baseHbFreq * 2;
            targetValues[4] = baseHbPhase / 2;
            break;
        case 2:
            targetValues[0] = baseChangeInterval * 6;
            targetValues[1] = baseHbOffset * 6;
            targetValues[2] = baseHbAmp * 6;
            targetValues[3] = baseHbFreq * 6;
            targetValues[4] = baseHbPhase * 6;
            break;
        case 3:
            targetValues[0] = baseChangeInterval / 4;
            targetValues[1] = baseHbOffset / 4;
            targetValues[2] = baseHbAmp / 4;
            targetValues[3] = baseHbFreq / 4;
            targetValues[4] = baseHbPhase / 4;
            break;
        case 4:
            targetValues[0] = baseChangeInterval * baseChangeInterval;
            targetValues[1] = baseHbOffset * baseChangeInterval;
            targetValues[2] = baseHbAmp * baseChangeInterval;
            targetValues[3] = baseHbFreq * baseChangeInterval;
            targetValues[4] = baseHbPhase * baseChangeInterval;
            break;
        }

        // initiate transition
        transitionStart = ofGetElapsedTimef();
        transitionDuration = ofRandom(0.5f, 1.5f);
        isTransitioning = true;

        // target values for interpolation
        targetChangeInterval = targetValues[0];
        targetHbOffset = targetValues[1];
        targetHbAmp = targetValues[2];
        targetHbFreq = targetValues[3];
        targetHbPhase = targetValues[4];

        // start values
        startChangeInterval = changeInterval;
        startHbOffset = hbOffset;
        startHbAmp = hbAmp;
        startHbFreq = hbFreq;
        startHbPhase = hbPhase;
    }

    // to handle transitions
    if (isTransitioning)
    {
        float now = ofGetElapsedTimef();
        float progress = (now - transitionStart) / transitionDuration;

        if (progress >= 1.0f)
        {
            // transition complete
            isTransitioning = false;
            changeInterval = targetChangeInterval;
            hbOffset = targetHbOffset;
            hbAmp = targetHbAmp;
            hbFreq = targetHbFreq;
            hbPhase = targetHbPhase;
        }
        else
        {
            // smoothly interpolate between start and target values
            float t = smoothStep(progress); // separate easing function
            changeInterval = ofLerp(startChangeInterval, targetChangeInterval, t);
            hbOffset = ofLerp(startHbOffset, targetHbOffset, t);
            hbAmp = ofLerp(startHbAmp, targetHbAmp, t);
            hbFreq = ofLerp(startHbFreq, targetHbFreq, t);
            hbPhase = ofLerp(startHbPhase, targetHbPhase, t);
        }
    }

    /*************************
       raspberry pi receiver
    **************************/
    //    int bytesReceived = udpReceiver.Receive(udpMessage, 1024);
    //    if (bytesReceived > 0)
    //    {
    //        udpMessage[bytesReceived] = '\0'; // Null-terminate the string
    //        std::string message(udpMessage);
    //        std::cout << "received from raspberry pi: " << message << std::endl;
    //    }

    normalizedTime = ofClamp(timeSinceChange / changeInterval, 0, 1);
    easedTime = 1 - pow(1 - normalizedTime, 3); // cubic ease out

    currentTime = ofGetElapsedTimef();

    /*********************
     pure data sender
     **********************/

    int currentFrame = ofGetFrameNum();
    isItUDPTime = currentFrame % 2 == 0;

    if (isItUDPTime)
    {
        messageCounter++;
        // in pd from left to right:
        string message = ofToString(messageCounter) + " "; // message
        message += ofToString(currentTime) + " ";          // time
        message += ofToString(currentViolence) + " ";      // violence
        message += ofToString(currentAmplitude) + " ";     // amplitude
        message += ofToString(opacity) + " ";              // opacity
        message += ofToString(changeInterval) + " ";       // interval
        message += ofToString(speedDampen) + "\n";         // spread

        // std::cout << "message to be sent:\n " << message << std::endl;
        udpConnection.Send(message.c_str(), message.length());
    }

    currentTime = ofGetElapsedTimef();

    // simple trigonometry for build up and down of the tempo
    // TODO: instead of sin cos tan use raspberry pi with distance sensor
    // TODO: if nobody is around pick either sin or tan and play a loop
    changeInterval = hbAmp * sin(hbFreq * currentTime + hbPhase) + hbOffset;
    // if value is above 1 shit breaks

    // these targets will gradually change over specified interval
    if (currentTime - lastChangeTime > changeInterval)
    {
        float randomizer = ofRandom(0.1f, 5.0f);
        
        targetViolence = smoothRemapper(1.0f, 20.0f + randomizer);
        targetAmplitude = smoothRemapper(0.5f, 30.0f + randomizer);
        targetSpread = smoothRemapper(0.5f, 20.0f + randomizer);

        lastChangeTime = currentTime;
    }

    // ease towards the target violence
    // float easeAmount = 0.25f;
    easeAmount = ofMap(changeInterval, hbOffset - hbAmp, hbOffset + hbAmp, 1.0f, 0.05f);

    currentViolence += (targetViolence - currentViolence) * easeAmount;
    currentAmplitude += (targetAmplitude - currentAmplitude) * easeAmount;
    currentSpread += (targetSpread - currentSpread) * easeAmount;
    camDis += (targetCamDis - camDis) * easeAmount;

    // Apply camera distance
    cam.setDistance(camDis);

    glm::vec3 position = model.getPosition();
    cam.lookAt(position);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    cam.begin();

    // TODO: this happens three times in the code
    float currentTime = ofGetElapsedTimef();
    timeSinceChange = currentTime - lastChangeTime;

    opacity = ofMap(easedTime, 0, 1, 255, 0);

    // // opacity gradual drop off
    opacity = ofMap(timeSinceChange, 0, changeInterval, 255, 0, true);
    opacity = ofClamp(opacity, 0, 255); // ensure opacity is within 0-255

    // pause at random in darkness
    // if (ofRandom(1.0) < 1 && opacity < 10.0f) // chance
    //{
    /* feels cool, like an engine maxing out rpm, but soon gets boring
    ofSetColor(0, 0, 0, 0); // BUG: doesnt work becaue of the scope or what?
    lastChangeTime = ofGetElapsedTimef() - changeInterval + 2.0f; */

    /* feels like program just crashes, sleep doesnt work
        ofSetColor(0, 0, 0, opacity); // BUG: doesnt work becaue of the scope or what?
        ofSleepMillis(2000);
    */
    //}

    // cubic ease in
    float opacityGain = pow(normalizedTime, 3);
    opacity = ofMap(opacityGain, 0, 1, 0, 255);
    ofSetColor(255, 255, 255, opacity);

    targetCamDis = ofMap(opacity, 0, 255, 1600, 1100);
    drawWithMesh();

    cam.end();

    ofSetColor(255);

    if (debug)
    {

        ofDrawBitmapString("violence: " + ofToString(currentViolence), 20, 40);
        ofDrawBitmapString("interval: " + ofToString(changeInterval), 20, 60);
        ofDrawBitmapString("amplitude: " + ofToString(currentAmplitude), 20, 80);
        ofDrawBitmapString("spread: " + ofToString(currentSpread), 20, 100);
        ofDrawBitmapString("opacity: " + ofToString(opacity), 20, 120);

        ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 20, ofGetHeight() - 40);
        ofDrawBitmapString("distance: " + ofToString(cam.getDistance()), 20, ofGetHeight() - 60);
        ofDrawBitmapString("easing: " + ofToString(easeAmount), 20, ofGetHeight() - 80);

        int yPos = ofGetHeight() - 180;
        ofDrawBitmapString("interval: " + ofToString(changeInterval, 2), ofGetWidth() - 200, yPos + 20);
        ofDrawBitmapString("offset: " + ofToString(hbOffset, 2), ofGetWidth() - 200, yPos + 40);
        ofDrawBitmapString("amplitude: " + ofToString(hbAmp, 2), ofGetWidth() - 200, yPos + 60);
        ofDrawBitmapString("frequency: " + ofToString(hbFreq, 2), ofGetWidth() - 200, yPos + 80);
        ofDrawBitmapString("phase: " + ofToString(hbPhase, 2), ofGetWidth() - 200, yPos + 100);
    }
    // network example
    // for (unsigned int i = 1; i < stroke.size(); i++)
    // {
    //     ofDrawLine(stroke[i - 1].x, stroke[i - 1].y, stroke[i].x, stroke[i].y);
    // }
}

float ofApp::smoothRemapper(float outputMin, float outputMax)
{
    return ofMap(changeInterval, hbOffset - hbAmp, hbOffset + hbAmp, outputMin, outputMax);
}

float ofApp::smoothStep(float x)
{
    // Smooth step function: 3x^2 - 2x^3
    return x * x * (3 - 2 * x);
}

// draw the model manually
void ofApp::drawWithMesh()
{

    // get the model attributes we need
    glm::vec3 scale = model.getScale();
    glm::vec3 position = model.getPosition();
    float normalizedScale = model.getNormalizedScale();
    ofVboMesh mesh = model.getMesh(0);
    ofTexture texture;
    ofxAssimpMeshHelper &meshHelper = model.getMeshHelper(0);
    bool bHasTexture = meshHelper.hasTexture();
    if (bHasTexture)
    {
        texture = model.getTextureForMesh(0);
    }

    ofMaterial material = model.getMaterialForMesh(0);

    ofPushMatrix();

    // translate and scale based on the positioning
    ofTranslate(position);
    // ofRotateDeg(ofGetMouseX() + 270, 0, 1, 0);
    // ofRotateDeg(90, 1, 2, 0);

    ofScale(normalizedScale, normalizedScale, normalizedScale);
    ofScale(scale.x, scale.y, scale.z);

    // float violence = ofMap(mouseX, 0, ofGetWidth(), 1, 20);
    violence = currentViolence;
    amplitude = currentAmplitude;
    speedDampen = currentSpread;

    auto &verts = mesh.getVertices();

    for (unsigned int i = 0; i < verts.size(); i++)
    {
        verts[i].x += ofSignedNoise(verts[i].x / violence, verts[i].y / violence, verts[i].z / violence, ofGetElapsedTimef() / speedDampen) * amplitude;
        verts[i].y += ofSignedNoise(verts[i].z / violence, verts[i].x / violence, verts[i].y / violence, ofGetElapsedTimef() / speedDampen) * amplitude;
        verts[i].z += ofSignedNoise(verts[i].y / violence, verts[i].z / violence, verts[i].x / violence, ofGetElapsedTimef() / speedDampen) * amplitude;
    }

    // draw the model manually
    //  if(bHasTexture) texture.bind();
    //  material.begin();
    mesh.drawWireframe();
    // mesh.drawFaces();
    // material.end();
    if (bHasTexture)
        texture.unbind();

    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == '+' || key == '=')
    {
        cam.setDistance(cam.getDistance() * 0.8f);
    }
    else if (key == '-' || key == '_')
    {
        cam.setDistance(cam.getDistance() * 1.2f);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
    // stroke.push_back(glm::vec3(x, y, 0));
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}
