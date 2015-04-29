/*
Code created for the noise workshop by Andreas Muller 


broken down and commented by Kelia Murata
*/


#include "ofApp.h"

//-----------------------------------------------------------------------------------------
//
void ofApp::setup()
{
	//ofSetLogLevel( OF_LOG_VERBOSE );
	
	//sets frame rate (how many frames cycle per second) 
	ofSetFrameRate( 60 );
	
	//loads in a fount from a file and assigns a size 
	fontSmall.loadFont("Fonts/DIN.otf", 8 );
	
	//loads font into the gui interface 
	ofxGuiSetFont( "Fonts/DIN.otf", 8 );
	//changes the width of the gui interface screen to be 300 pixels wide 
	ofxGuiSetDefaultWidth( 300 );

	//creates a string variable that holds the name path to read the particles from a file 
	string xmlSettingsPath = "Settings/Particles.xml";
	//gives a name to the path and uses the file pathway string we just created 
	gui.setup( "Particles", xmlSettingsPath );
	
	//adds a gui interface int bar slider that controls the number of particles displayed on screen 
	//called Num particles, defaulting to 300 at the beginning of the program and ranging from 1 
	//to 10000
	gui.add( numParticles.set("Num particles",  300,  1,  10000) );
	//adds a gui interface float bar slider to control the offset of the particles defaulting at 0.1
	//at the start of the program and going from 0 to 5 
	gui.add( randomOffset.set("Random Offset",	0.1f, 0, 5) );
	
	//adds a float slider bar that controls our space frequency variable and defaults to 1.0 at be start 
	//of the program and ranges from 0.0001 to 1.0 
	gui.add( spaceFrequency.set("Space Frequency",   1.0f,  0.0001f,  1.0f) );
	//adds a float slider bar that controls the time frequency variable, defaulting to 1.0 at the start of the 
	//program and ranging from 0.0001 to 2.0 
	gui.add( timeFrequency.set("Time Frequency",	1.0f,  0.0001f,  2.0f) );

	//adds a float slider bar to the interface that controls the noise magnitude variable, defaulting at 1.0 
	//and ranging from 0.0001 to 2.0 
	gui.add( noiseMagnitude.set("Noise Magnitude",	1.0f,  0.0001f,  2.0f) );
	
	//adds a float slider bar that controls the oldveamount variable and defaults to 0.9 and ranges from 0, to 1
	gui.add( oldVelAmount.set("Old Vel Amount",	0.9f, 0, 1) );

	//adds a float slider bar that controls the maxvel variable and defaults to 0.1 and ranges from 0 to 1
	gui.add( maxVel.set("Max Vel",	0.1f, 0, 1) );
	//adds a float slider bar that controls the max age variable of the particles and defaults at 3.0 at the begining 
	//of the program and goes to 10 
	gui.add( maxAge.set("Max Age",	3.0f, 0, 10) );

	//loads from our particle fill (takes the string that we set at the top) 
	gui.loadFromFile( xmlSettingsPath );
	
	// Registers callbacks
	//listens for when the number of particles is changed and calls the number particles changed function that 
	//is part of the particle class
	numParticles.addListener( this, &ofApp::numParticlesChanged );
	
	//creates an int called temp num particles that gets set to the current number of particles 
	int tmpNumParticles = numParticles;
	//it passes in the copy of the number of particles (the int temp num particles just created) and passes it into
	//the function call for numParticlesChanged 
	//Note: better to give a copy of the information so the data is not manipulated before we want it to 
	numParticlesChanged( tmpNumParticles );
	
	// Give us a starting point for the camera, we're changing the scale a bit here to keep things more intuitive
	camera.setNearClip(0.01f);
	//sets the position of the camera within the space, taking in x, y, and z coordinates
	camera.setPosition( 0, 5, 15.0 );
	//the camera can be moved with the mouse (right click) this sets the speed at which the camera moves when it 
	//is dragged 
	camera.setMovementMaxSpeed( 0.4f );
	
	//defaults the scene to not draw the gui interface at the start of the project, instead keeping it hidden by default 
	drawGui = false;
}

//-----------------------------------------------------------------------------------------
//a function created for our particle class, takes in int 
void ofApp::numParticlesChanged(int& _num )
{
	//resizes the size of the vector holding all the programs particles to the specified amount the function took in 
	particles.resize( _num );
	//forloop to go through each part of the vector and look at each individual particle stored there 
	for( unsigned int i = 0; i < particles.size(); i++ )
	{
		//re-initializes the particle at the spot in the vector we are looking at right now calling the function 
		//created for the particle class 
		reInitParticle( &particles.at(i) );
		//telling the specific particle currently being looked at and setting its age to a random number between 
		//0 and our determined max age (controlled by slider) 
		particles.at(i).age = ofRandom( maxAge );
	}
}

//-----------------------------------------------------------------------------------------
//a function created for our particle class to re-initailize them after the first initialization so we can change the 
//number of particles at will 
//takes in a particle 
void ofApp::reInitParticle( Particle* _p )
{
	//creates a vector called spawnSize and loading in a value (x,y,z coordinates) 
	ofVec3f spawnSize(10,10,10);
	
	//asks the particle for the reference to its pos and sets it to a random x, y, and z determined by the half the spawn size smaller 
	//and half the spawnsize larger 
	_p->pos = ofVec3f(  ofRandom( spawnSize.x * -0.5, spawnSize.x * 0.5),
						ofRandom( spawnSize.y *  0.0, spawnSize.y * 1.0),
						ofRandom( spawnSize.z * -0.5, spawnSize.z * 0.5) );
	
	//sets the particle we took into the functions velocity to 0
	_p->vel = ofVec3f(0);
	
	//sets the particle we took into the functions noise offset to a random value between 0 and the randomOffSet (which is determined by 
	//our slider bar value)
	_p->noiseRandomOffset = ofVec3f( ofRandom( 0, randomOffset),
									 ofRandom( 0, randomOffset),
									 ofRandom( 0, randomOffset) );
	
	//sets the particles age to 0 
	_p->age = 0;
	
	//clears the particles position history 
	//cleans in slate, not past recorded positions so its like new 
	_p->posHistory.clear();
}


//-----------------------------------------------------------------------------------------
void ofApp::update()
{
	//creates a float type variable called time and stores the elapsed time received from the program with it 
	//Note: this variable will be re-set everytime the update is called 
	float time = ofGetElapsedTimef();
	//creates a float variable to store the value 1.0 divided by 60.0 
	float timeDelta = 1.0 / 60.0; //ofGetLastFrameTime();
	
	//creates a float type variable to store the value of the time (elapsed time we just created) and multiplying it 
	//by the time frequency whose value is controlled by our slider bar 
	float noiseReadTime = time * timeFrequency;
	//creates an int that holds the length of the tail of color a particle leaves behind 
	int tailLength = 6;
	
	//a forloop to go through each particle in the particle vector 
	for( unsigned int i = 0; i < particles.size(); i++ )
	{
		//creates a pointer to the particle currently being looked at in the vector 
		Particle* p = &particles.at(i);
		//looks at that particles age and sets it to its current age plus our time delta value (aging the particle) 
		particles.at(i).age += timeDelta;
		
		//if the particle we are looking at is passed our set max age...
		if( particles.at(i).age >= maxAge )
		{
			//re-initialize that particle 
			reInitParticle( p );
		}
		
		//creates a vector called noise read position and sets it to the current particles position plus the particles 
		//random offset (which is controlled by the slider bar) and multiples that added value by the space frequency
		//(which is also controlled by a slider bar) 
		ofVec3f noiseReadPos = (p->pos + p->noiseRandomOffset ) * spaceFrequency;
		//looks at the particles velocity and sets it to the noise of the noise read position just created and the noise read time
		//which was created at the beginning of update, takes that noise value and multiples it by the noise magnitude (controlled by 
		//slider bar)  and calles the get interpolated function which is taking in the particles velocity and the old velocity amount 
		p->vel = (getNoise( noiseReadPos, noiseReadTime ) * noiseMagnitude).getInterpolated( p->vel, oldVelAmount );
		//limits the particles velocity to the max velocity 
		p->vel.limit( maxVel );
		
		//sets the particles postition to its position plus its velocity 
		p->pos += p->vel;
		
		// Save positions to use as a tail
		p->posHistory.push_front( p->pos );
		//while the position history is larger than the tail length pop the position history back 
		while( p->posHistory.size() > tailLength ) 
		{ 
			p->posHistory.pop_back(); 
		}
	}
}

//-----------------------------------------------------------------------------------------
//a new function create for ofApp that takes a vector and a float for hte noise positions and time 
ofVec3f ofApp::getNoise( ofVec3f _noisePos, float _time )
{
	//creates a vector called p and loads in some values 
	ofVec3f p(0,0,0);
	
//	p.x += ofSignedNoise( _noisePos.x, _noisePos.y, _noisePos.z, _time );
//	p.y += ofSignedNoise( _noisePos.y, _noisePos.z, _noisePos.x, _time );
//	p.z += ofSignedNoise( _noisePos.z, _noisePos.x, _noisePos.y, _time );
	
	//sets each of those values (just placed in the p vector) to the signed noise of each of the noise positions 
	//for x,y, and z and the time handed in through the function call 
	p.x += ofSignedNoise( _noisePos.x, _noisePos.y, _noisePos.z, _time );
	p.y += ofSignedNoise( _noisePos.y, _noisePos.z, _time,		 _noisePos.x );
	p.z += ofSignedNoise( _noisePos.z, _time,		_noisePos.x, _noisePos.y );

	//return this vector to whoever called the function 
	return p;
}

//-----------------------------------------------------------------------------------------
//
void ofApp::draw()
{
	//creates a backround grandient circle at the center of the screne radiating out in a circle 
	ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);
	
	//truns depth test to true to so rendering happens according to z-depth, not draw order 
	ofEnableDepthTest();
	
	//starts the easy camera 
	camera.begin();
	
	//Note: all tabbed in to show these calls are within the camera 
		
	//sets the color for all drawn objects after this call 
		ofSetColor( ofColor(60) );
		//creates a matrix 
		ofPushMatrix();
			//rotates the scene 
			ofRotate(90, 0, 0, -1);
			// draws a grid on the floor
			ofDrawGridPlane( 10, 12, false );
		//leave the matrix created
		ofPopMatrix();
	
		//resets the color to white for objects after this call 
		ofSetColor( ofColor::white );
		//draws the particles by calling the draw particles function created 
		drawParticles();
	
	//stops the camera 
	camera.end();
	
	//turns the depth test to false 
	ofDisableDepthTest();
	//turns the blend mode to true which smooths out the lines 
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	//sets the color to white 
	ofSetColor( ofColor::white );
	
	//if the bool value draw gui is set to true...
	if( drawGui )
	{
		//draw the gui interface 
		gui.draw();
	}
	
	//~~~~~~~~~~~~~~~~~~~
	//create 
	fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(),2), ofGetWidth()-35, ofGetHeight() - 6, ofColor::whiteSmoke, ofColor::black );
}


//-----------------------------------------------------------------------------------------
//a new function created called draw particles to take care of out the particles are drawn 
void ofApp::drawParticles()
{
	//creates a float variable called time that gets the elapsed time from the program and multiplies it by 0.1 
	float time = ofGetElapsedTimef() * 0.1;
	
	//creates an ofMesh and calls it mesh
	ofMesh mesh;
	//sets our mesh mode to primitive lines to draw the mesh with lines 
	mesh.setMode( OF_PRIMITIVE_LINES );
	//forloop for looking at each particle individually in the particle vector 
	for( unsigned int i = 0; i < particles.size(); i++ )
	{
		//creates a temp particle called p and sets it equal to the particle currently being looked at in the vector 
		Particle* p = &particles.at(i);
		
		//creates a random seed for the random number generator to make sure each "random" number doesnt have the same 
		//seed, making it more random 
		ofSeedRandom( i << 24 );
		//creates a float value color called temp color and setting it to a random color from the hue saturation and brightness 
		ofFloatColor tmpCol = ofFloatColor::fromHsb( ofRandom(1.0), 0.8, 0.8 );
		//sets the temperary colors alpha channel to a smooth input and output determined by max age (a slider bar manipulated value) 
		tmpCol.a = MathUtils::smoothStepInOut(0, maxAge * 0.1, maxAge * 0.9, maxAge, p->age);
		
		//if the position history is greater than 1 (its had at least a position update, ie one position in its 'life')
		if( p->posHistory.size() > 1 )
		{
			//forloop to go through the particles position history 
			for( int j = 1; j < p->posHistory.size(); j++ )
			{
				//mesh.addColor( tmpCol );
				//add a random color to the mesh 
				mesh.addColor( ofFloatColor(tmpCol, ofMap( j-1, 0, p->posHistory.size()-1, 1, 0)) );
				//add the particle just before the one we are looking at to the mesh 
				mesh.addVertex( p->posHistory.at(j-1) );
				
				//mesh.addColor( tmpCol );
				mesh.addColor( ofFloatColor(tmpCol, ofMap( j  , 0, p->posHistory.size()-1, 1, 0)) );
				//add the particle that we are currently looking at 
				mesh.addVertex( p->posHistory.at(j) );
			}
		}
		//ofDrawSphere( particles.at(i).pos, 0.2 );
	}
	//draw the mesh 
	mesh.draw();
	
	//get a random seed 
	ofSeedRandom();
}


//-----------------------------------------------------------------------------------------
//
void ofApp::keyPressed(int key)
{
	//if the tab key is hit, toggle the gui interface visible or invisible 
	//Note: gui defaults to not draw at the start of the program 
	if( key == OF_KEY_TAB )
	{
		//whatever state the gui is in right now, not set it to the opposite of that 
		drawGui = !drawGui;
	}

	//if the 'f' key is pressed toggle to a full screen verision of the app 
	else if( key == 'f' )
	{
		ofToggleFullscreen();
	}
}
