/*
Code created for the noise workshop by Andreas Muller 


broken down and commented by Kelia Murata
*/

//takes care of defines for class
#pragma once

//includes main and gui library definitions 
#include "ofMain.h"
#include "ofxGui.h"
//includes the math definitions 
#include "Math/MathUtils.h"

//includes the untility, camera, and font definitions
#include "Utils/Cameras/ofxFirstPersonCamera.h"
#include "Utils/ofTrueTypeFontExt.h"

// -----------------------------------------------------------
class Particle
{
	public:
	
		//defines the variables associated with the particle class created here 
		ofVec3f pos;
		ofVec3f vel;
		ofVec3f noiseRandomOffset;
		deque<ofVec3f> posHistory;
		float age;
};

// -----------------------------------------------------------
class ofApp : public ofBaseApp
{
	public:
		
		//variables for made program created here as global class variables 
		ofParameter<int>		numParticles;
		ofParameter<float>		randomOffset;
		ofParameter<float>		noiseMagnitude;
		ofParameter<float>		spaceFrequency;
		ofParameter<float>		timeFrequency;
		ofParameter<float>		oldVelAmount;
		ofParameter<float>		maxVel;
		ofParameter<float>		maxAge;
		bool					drawGui;

		//vector to store all particles 
		vector<Particle>		particles;
	
		//the scene camera 
		ofxFirstPersonCamera	camera;
	
		//the text font 
		ofTrueTypeFontExt		fontSmall;
	
		//the interface gui panel 
		ofxPanel				gui;

		//functions for application definitions 
		void setup();
		void update();
		void draw();

		void drawParticles();
		void reInitParticle( Particle* _p );
		ofVec3f getNoise( ofVec3f _noisePos, float _time );
		void numParticlesChanged(int& _param );			// Callbacks from the UI
		void keyPressed(int key);
	
		
	
	
};