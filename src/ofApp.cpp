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
float changeInterval = 0.75f; // heartbeat
                    // adrenaline spikes
    float hbOffset  = 0.7f; 
    float hbAmp     = 0.3f; 
    float hbFreq    = 0.02f; 
    float hbPhase   = 0.0f;

float lastChangeTime = 0.0f;


// shape params
float currentLiquidness = 5.0f;
float targetLiquidness = 5.0f;

float currentAmplitude = 5.0f;
float targetAmplitude = 5.0f;

// network
int messageCounter = 0;

 //--------------------------------------------------------------
 void ofApp::setup(){
 
    ofSetVerticalSync(true);
	// ofEnableAntiAliasing();
 
    //  ofSetFullscreen(true);
     ofSetFrameRate(120);
     ofSetVerticalSync(true);
     ofBackground(0, 0, 0, 0);
 
     //we need to call this for textures to work on models
     // ofDisableArbTex();
 
     //this makes sure that the back of the model doesn't show through the front
     ofEnableDepthTest();

     //now we load our model
     model.loadModel("characters/untitled.obj");
     // model.loadModel("characters/head.dae");
     // model.loadModel("characters/bust.dae");
     model.setPosition(ofGetWidth()*.5, ofGetHeight() * 0.5, 0);
     model.setScale(0.5f, 0.5f, 0.5f);
 
     cam.setDistance(100);
     cam.setNearClip(10);
     cam.setFarClip(5000);
 
     // light.enable();
     // light.setPosition(model.getPosition() + glm::vec3(0, 0, 600));

    lastChangeTime = ofGetElapsedTimef();

    /****************
    *    Network    *
    *****************/

    //create the socket and set to send to 127.0.0.1:11999
    ofxUDPSettings settings;
    settings.sendTo("127.0.0.1", 11999);
    settings.blocking = false;

    udpConnection.Setup(settings);


 }
 
 //--------------------------------------------------------------
 void ofApp::update(){

    currentTime = ofGetElapsedTimef();
    

    bool isItUDPTime = true;
    int currentFrame = ofGetFrameNum();
    isItUDPTime = currentFrame % 2 == 0;

    // UDP spamming the port 
    if(isItUDPTime) {
        messageCounter++;
        
        // Create a detailed message with sequential counter
        string message = "message " + ofToString(messageCounter) + ":\n";
        message += "liquidness=" + ofToString(currentLiquidness) + ";\n";
        message += "amplitude=" + ofToString(currentAmplitude) + ";\n";
        message += "interval=" + ofToString(changeInterval) + ";\n";
        message += "time=" + ofToString(currentTime) + ";\n";
        

        // std::cout << "message to be sent:\n " << message << std::endl;
        // Send the UDP message
        udpConnection.Send(message.c_str(), message.length());
        
    }


     currentTime = ofGetElapsedTimef();
    
     changeInterval = hbAmp * sin(hbFreq * currentTime + hbPhase) + hbOffset;

     // check if it's time to change the target liquidness
    if (currentTime - lastChangeTime > changeInterval) {
        
        targetLiquidness = ofRandom(1, 20);  // new random 
        targetAmplitude = ofRandom(1, 5);   // targets
        
        lastChangeTime = currentTime;
    }

    // ease towards the target liquidness
    float easeAmount = 0.025f; 
    currentLiquidness += (targetLiquidness - currentLiquidness) * easeAmount;
    currentAmplitude += (targetAmplitude - currentAmplitude) * easeAmount;  


     glm::vec3 position = model.getPosition();
     cam.lookAt(position);
 }
 
 //--------------------------------------------------------------
 //--------------------------------------------------------------
void ofApp::draw() {
    cam.begin();

    // TODO: this happens three times in the code
    float currentTime = ofGetElapsedTimef();
    float timeSinceChange = currentTime - lastChangeTime;

    // opacity gradual drop off
    float opacity = ofMap(timeSinceChange, 0, changeInterval, 255, 0, true);
    opacity = ofClamp(opacity, 0, 255); // ensure opacity is within 0-255
    ofSetColor(255, 255, 255, opacity);

    drawWithMesh();

    cam.end();

    ofSetColor(255);

    ofDrawBitmapString("violence: " + ofToString(currentLiquidness), 10, 10);
    ofDrawBitmapString("interval: " + ofToString(changeInterval), 10, 30);
    ofDrawBitmapString("amplitude: " + ofToString(currentAmplitude), 10, 50);
    ofDrawBitmapString("opacity: " + ofToString(opacity), 10, 70);

    // network example
	for(unsigned int i=1;i<stroke.size();i++){
		ofDrawLine(stroke[i-1].x,stroke[i-1].y,stroke[i].x,stroke[i].y);
	}
}

 
 //draw the model the built-in way
 void ofApp::drawWithModel(){
 
     //get the position of the model
     glm::vec3 position = model.getPosition();
 
     //save the current view
     ofPushMatrix();
 
     ofRotateDeg(ofGetMouseX() + 270, 0, 1, 0);
 
     //draw the model
     model.drawFaces();
 
     //restore the view position
     ofPopMatrix();
 }
 
 //draw the model manually
 void ofApp::drawWithMesh(){
 
     //get the model attributes we need
     glm::vec3 scale = model.getScale();
     glm::vec3 position = model.getPosition();
     float normalizedScale = model.getNormalizedScale();
     ofVboMesh mesh = model.getMesh(0);
     ofTexture texture;
     ofxAssimpMeshHelper& meshHelper = model.getMeshHelper( 0 );
     bool bHasTexture = meshHelper.hasTexture();
     if( bHasTexture ) {
         texture = model.getTextureForMesh(0);
     }
 
     ofMaterial material = model.getMaterialForMesh(0);
 
     ofPushMatrix();
 
     // translate and scale based on the positioning
     ofTranslate(position);
     // ofRotateDeg(ofGetMouseX() + 270, 0, 1, 0);
     ofRotateDeg(90,1,2,0);
 
 
     ofScale(normalizedScale, normalizedScale, normalizedScale);
     ofScale(scale.x,scale.y,scale.z);
 
     //modify mesh with some noise

    
    // float liquidness = ofMap(mouseX, 0, ofGetWidth(), 1, 20);
    liquidness = currentLiquidness;
    amplitude = currentAmplitude;

    // float amplitude = 5.0f;
    float speedDampen = 5.0f;
     auto &verts = mesh.getVertices();
 
     for(unsigned int i = 0; i < verts.size(); i++){
         verts[i].x += ofSignedNoise(verts[i].x/liquidness, verts[i].y/liquidness,verts[i].z/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
         verts[i].y += ofSignedNoise(verts[i].z/liquidness, verts[i].x/liquidness,verts[i].y/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
         verts[i].z += ofSignedNoise(verts[i].y/liquidness, verts[i].z/liquidness,verts[i].x/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
     }
 
     //draw the model manually
     // if(bHasTexture) texture.bind();
     // material.begin();
     mesh.drawWireframe();
     // mesh.drawFaces();
     // material.end();
     if(bHasTexture) texture.unbind();
 
     ofPopMatrix();

    
 }
 
 //--------------------------------------------------------------
 void ofApp::keyPressed(int key) {
     if (key == '+' || key == '=') {
         cam.setDistance(cam.getDistance() * 0.8f);
     } else if (key == '-' || key == '_') {
         cam.setDistance(cam.getDistance() * 1.2f);
     }
 }
 
 //--------------------------------------------------------------
 void ofApp::keyReleased(int key){
 
 }
 
 //--------------------------------------------------------------
 void ofApp::mouseMoved(int x, int y ){
 
 }
 
 //--------------------------------------------------------------
 void ofApp::mouseDragged(int x, int y, int button){
    stroke.push_back(glm::vec3(x,y,0));
 }
 
 //--------------------------------------------------------------
 void ofApp::mousePressed(int x, int y, int button){
    stroke.clear();
 }
 
 //--------------------------------------------------------------
 void ofApp::mouseReleased(int x, int y, int button){
    string message="";
	for(unsigned int i=0; i<stroke.size(); i++){
		message+=ofToString(stroke[i].x)+"|"+ofToString(stroke[i].y)+"[/p]";
	}
	udpConnection.Send(message.c_str(),message.length());
}
 
 //--------------------------------------------------------------
 void ofApp::mouseEntered(int x, int y){
 
 }
 
 //--------------------------------------------------------------
 void ofApp::mouseExited(int x, int y){
 
 }
 
 //--------------------------------------------------------------
 void ofApp::windowResized(int w, int h){
 
 }
 
 //--------------------------------------------------------------
 void ofApp::gotMessage(ofMessage msg){
 
 }
 
 //--------------------------------------------------------------
 void ofApp::dragEvent(ofDragInfo dragInfo){
 
 }
 