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

#include "ofApp.h"
#include <cmath>

// smooth change
float changeInterval = 0.5f; // heartbeat
                              // adrenaline spikes
float hbOffset = 0.7f;
float hbAmp = 0.3f;
float hbFreq = 0.2f;
float hbPhase = 0.0f;

float lastChangeTime = 0.0f;

// shape params
float currentViolence = 5.0f;
float targetViolence = 5.0f;
float opacity = 255.0f;

float currentAmplitude = 5.0f;
float targetAmplitude = 5.0f;

// network
int messageCounter = 0;

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
    model.setScale(2.0f, 2.0f, 2.0f);

    cam.setDistance(500);
    cam.setNearClip(10);
    cam.setFarClip(5000);

    // light.enable();
    // light.setPosition(model.getPosition() + glm::vec3(0, 0, 600));

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

    currentTime = ofGetElapsedTimef();

    bool isItUDPTime = true;
    int currentFrame = ofGetFrameNum();
    isItUDPTime = currentFrame % 2 == 0;

    /*********************
       pure data sender
    **********************/
    if (isItUDPTime)
    {
        messageCounter++;
        // in pd from left to right:
        string message = ofToString(messageCounter) + " "; // message
        message += ofToString(currentTime) + " ";          // time
        message += ofToString(currentViolence) + " ";      // violence
        message += ofToString(currentAmplitude) + " ";     // amplitude
        message += ofToString(opacity) + " ";              // opacity
        message += ofToString(changeInterval) + "\n";      // interval

        // std::cout << "message to be sent:\n " << message << std::endl;
        udpConnection.Send(message.c_str(), message.length());
    }

    /*************************
       raspberry pi receiver
    **************************/
    int bytesReceived = udpReceiver.Receive(udpMessage, 1024);
    if (bytesReceived > 0)
    {
        udpMessage[bytesReceived] = '\0'; // Null-terminate the string
        std::string message(udpMessage);
        std::cout << "received from raspberry pi: " << message << std::endl;
    }

    currentTime = ofGetElapsedTimef();

    // simple trigonometry for build up and down of the tempo. 
    // TODO: instead of sin cos tan use raspberry pi with distance sensor
            // TODO: if nobody is around pick either sin or tan and play a loop
    changeInterval = hbAmp * sin(hbFreq * currentTime + hbPhase) + hbOffset;
                            // tan is also very fun

    // check if it's time to change the target violence
    if (currentTime - lastChangeTime > changeInterval)
    { 
        // targetViolence = ofRandom(1, 10); // new random
        targetAmplitude = ofRandom(1, 5); // targets
        targetViolence = ofMap(changeInterval, hbOffset - hbAmp, hbOffset + hbAmp, 1, 20);
        targetAmplitude = ofMap(changeInterval, hbOffset - hbAmp, hbOffset + hbAmp, 1, 20);

        lastChangeTime = currentTime;
    }

    // ease towards the target violence
    float easeAmount = 0.025f;
    currentViolence += (targetViolence - currentViolence) * easeAmount;
    currentAmplitude += (targetAmplitude - currentAmplitude) * easeAmount;

    glm::vec3 position = model.getPosition();
    cam.lookAt(position);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    cam.begin();

    // TODO: this happens three times in the code
    float currentTime = ofGetElapsedTimef();
    float timeSinceChange = currentTime - lastChangeTime;

    
    // opacity gradual drop off
    opacity = ofMap(timeSinceChange, 0, changeInterval, 255, 0, true);
    opacity = ofClamp(opacity, 0, 255); // ensure opacity is within 0-255


// pause at random in darkness
if (ofRandom(1.0) < 1 && opacity < 10.0f) // chance 
{
    /* feels cool, like an engine maxing out rpm, but soon gets boring
    ofSetColor(0, 0, 0, 0); // BUG: doesnt work becaue of the scope or what?
    lastChangeTime = ofGetElapsedTimef() - changeInterval + 2.0f; */

    /* feels like program just crashes, sleep doesnt work
        ofSetColor(0, 0, 0, opacity); // BUG: doesnt work becaue of the scope or what?
        ofSleepMillis(2000); 
    */

}
    ofSetColor(255, 255, 255, opacity);

    drawWithMesh();

    cam.end();

    ofSetColor(255);

    ofDrawBitmapString("violence: " + ofToString(currentViolence), 10, 10);
    ofDrawBitmapString("interval: " + ofToString(changeInterval), 10, 30);
    ofDrawBitmapString("amplitude: " + ofToString(currentAmplitude), 10, 50);
    ofDrawBitmapString("opacity: " + ofToString(opacity), 10, 70);

    // network example
    for (unsigned int i = 1; i < stroke.size(); i++)
    {
        ofDrawLine(stroke[i - 1].x, stroke[i - 1].y, stroke[i].x, stroke[i].y);
    }
}

// draw the model the built-in way
void ofApp::drawWithModel()
{

    // get the position of the model
    glm::vec3 position = model.getPosition();

    // save the current view
    ofPushMatrix();

    ofRotateDeg(ofGetMouseX() + 270, 0, 1, 0);

    // draw the model
    model.drawFaces();

    // restore the view position
    ofPopMatrix();
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
    ofRotateDeg(90, 1, 2, 0);

    ofScale(normalizedScale, normalizedScale, normalizedScale);
    ofScale(scale.x, scale.y, scale.z);

    // modify mesh with some noise

    // float violence = ofMap(mouseX, 0, ofGetWidth(), 1, 20);
    violence = currentViolence;
    amplitude = currentAmplitude;

    // float amplitude = 5.0f;
    float speedDampen = 5.0f;
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
    stroke.push_back(glm::vec3(x, y, 0));
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    stroke.clear();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
    string message = "";
    for (unsigned int i = 0; i < stroke.size(); i++)
    {
        message += ofToString(stroke[i].x) + "|" + ofToString(stroke[i].y) + "[/p]";
    }
    udpConnection.Send(message.c_str(), message.length());
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
