#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
    ofBackground( 15 , 15 , 15 ) ; 
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	glEnable(GL_DEPTH_TEST);
	//ofEnableLighting();
	
	currentColor = ofColor(255);
		
	timeline.setup();
	timeline.setDurationInFrames(90);
	timeline.setLoopType(OF_LOOP_NORMAL);
	
    
    //rOffset = 0.0f ; 
    //gOffset = 0.0f  ; 
    //bOffset = 0.0f ;
	timeline.addKeyframes("R OFFSET", "rOffset.xml", ofRange(-1, 1) );
	timeline.addKeyframes("G OFFSET", "gOffset.xml", ofRange(-1, 1) );
    timeline.addKeyframes("B OFFSET", "bOffset.xml", ofRange(-1, 1) );
    timeline.addKeyframes("SOUND MULTIPLIER", "soundMult.xml", ofRange( 0 , 3000 ) );
    
    //uniform float soundMultiplier ; 
	
	//timeline.addTriggers("Colors", "colors.xml");
	
	timeline.getColors().loadColors("defaultColors.xml");

	ofAddListener(ofxTLEvents.trigger, this, &testApp::receivedTrigger);
    
    
    sourceVideo.loadMovie("movies/yelle_loop.mov");
	sourceVideo.play();
    sourceVideo.setVolume(0) ; 
    
    //set the texture parameters for the maks shader. just do this at the beginning
	glitchShader.load("shaders/glitch");
	glitchShader.begin();
	glitchShader.setUniformTexture( "Tex0", sourceVideo.getTextureReference(), 0);
	//glitchShader.setUniformTexture("Tex1", mask.getTextureReference(), 1);
	glitchShader.end();
    
    
    
	// 0 output channels, 
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	
	soundStream.listDevices();
	
	//if you want to set a different device id 
	//soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
	
	int bufferSize = 256;
	
	
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);
	
	bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
	scaledVol		= 0.0;
    
	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    

}

//--------------------------------------------------------------
void testApp::update(){

    //lets scale the vol up to a 0-1 range 
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
	//lets record the volume into an array
	volHistory.push_back( scaledVol );
	
	//if we are bigger the the size we want to record - lets drop the oldest value
	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}
    
    sourceVideo.idleMovie() ;
    
    rOffset = sin ( ofGetElapsedTimef() ) * .5 ; 
}

//--------------------------------------------------------------
void testApp::draw(){
	/*
	ofBackground(.15*255);
	ofPushMatrix();
	
	ofSetColor(currentColor);
	
	ofTranslate(ofGetWidth()*.5, ofGetHeight()*.5, 40);	
	ofRotate(timeline.getKeyframeValue("Rotate X"), 1, 0, 0);
	ofRotate(timeline.getKeyframeValue("Rotate Y"), 0, 1, 0);
	
	ofBox(0,0,0, 200);
	
	ofPopMatrix();*/
    
    
    ofPushMatrix();
    ofTranslate( 0 , 300 , 0 );
    
    ofSetColor( 255 , 255 , 255 ) ;
    
    ofRectangle glitchRect = ofRectangle( 15 , 15 , sourceVideo.getWidth() , sourceVideo.getHeight() ) ; 
    glitchShader.begin() ; 
    
    glActiveTexture(GL_TEXTURE0_ARB);
    sourceVideo.getTextureReference().bind();
    
    //draw a quad the size of the frame
    glBegin(GL_QUADS);
    
    //move the mask around with the mouse by modifying the texture coordinates
    float maskOffset = 15 - mouseY;
    glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, 0);
    glVertex2f( glitchRect.x , glitchRect.y );
    
    glMultiTexCoord2d(GL_TEXTURE0_ARB, glitchRect.width , 0);
    glVertex2f( glitchRect.x + glitchRect.width , glitchRect.y );
    
    glMultiTexCoord2d(GL_TEXTURE0_ARB, glitchRect.x + glitchRect.width , glitchRect.y + glitchRect.height );
    glVertex2f( glitchRect.x + glitchRect.width , glitchRect.y + glitchRect.height );
    
    glMultiTexCoord2d(GL_TEXTURE0_ARB, glitchRect.x , glitchRect.y + glitchRect.height );
    glVertex2f( glitchRect.x , glitchRect.y + glitchRect.height );
    
    glEnd();
    
    //we want to pass in some varrying values to animate our type / color 
    glitchShader.setUniform1f("time", ofGetElapsedTimef() );
    glitchShader.setUniform1f("soundMultiplier",  timeline.getKeyframeValue("SOUND MULTIPLIER") );
    glitchShader.setUniform1f("globalSound", smoothedVol ) ; 
    
    glitchShader.setUniform1f("rOffset", timeline.getKeyframeValue("R OFFSET") ) ; 
    glitchShader.setUniform1f("gOffset", timeline.getKeyframeValue("G OFFSET") ) ; 
    glitchShader.setUniform1f("bOffset", timeline.getKeyframeValue("B OFFSET") ) ; 
    
    
    
    glActiveTexture(GL_TEXTURE0_ARB);
    sourceVideo.getTextureReference().unbind();
    
    
    glitchShader.end() ;
    
    // draw the average volume:
	ofPushStyle();
   
    
    ofSetColor(245, 58, 135);
    ofFill();		
    ofCircle( glitchRect.x + glitchRect.width * 1.55f , 200 , scaledVol * 100.0f);
    
	ofPopStyle();
    ofPopMatrix() ; 
	timeline.draw();
}

//--------------------------------------------------------------
void testApp::receivedTrigger(ofxTLTriggerEventArgs& trigger){
	if(trigger.triggerName == "RED"){
		currentColor = ofColor(255,0,0);
	}
	else if(trigger.triggerName == "GREEN"){
		currentColor = ofColor(0,255,0);		
	}
	else if(trigger.triggerName == "BLUE"){
		currentColor = ofColor(0,0,255);
	}
}


//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){	
	
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;	
    
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;
        
		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
	}
	
	//this is how we get the mean of rms :) 
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :) 
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	
	bufferCounter++;
	
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		timeline.togglePlay();
	}
	if(key == 'h'){
		timeline.toggleShow();
	}
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
	