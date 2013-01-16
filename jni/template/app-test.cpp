//--------------------------------------------------------------------------------------
// File: app.cpp
//
// Empty starting point for new NA applications
//
// Copyright (c) 2011 Paulo Lopes. All rights reserved.
//--------------------------------------------------------------------------------------
#include <GLES/gl.h>

#include <na.h>
#include <ug.h>
#include <tokamak.h>

typedef struct {
	void * cb;					/* Opaque type for the CB */
	neSimulator *simTok;        /* Simulation Object */
	neRigidBody *rgdBall;		/* Rigid Body for the ball */
	neAnimatedBody *aniFloor;	/* Animated body for the Floor */
} tkState;

void tokamapLog(char * logString) {
	LOG_INFO("Tokamak", logString);
}

int initSim(tkState* gState) {
	neV3 gravity; gravity.Set(0.0f, -10.f, 0.0f);
	neSimulatorSizeInfo sizeInfo;
	sizeInfo.rigidBodiesCount = 1;
	sizeInfo.animatedBodiesCount = 1;
	sizeInfo.geometriesCount = 2;
	sizeInfo.overlappedPairsCount = 2;
	gState->simTok = neSimulator::CreateSimulator(sizeInfo, NULL, &gravity);
	if(gState->simTok != NULL) {
		gState->simTok->SetLogOutputCallback(tokamapLog);
		return TRUE;
	}
	return FALSE;
}

bool initBodies(tkState* gState) {
	/* Setup Ball */
	neV3 ballPos;
	gState->rgdBall = gState->simTok->CreateRigidBody();
	neGeometry *geoBall = gState->rgdBall->AddGeometry();
	geoBall->SetSphereDiameter(1.5f);
	gState->rgdBall->UpdateBoundingInfo();
	gState->rgdBall->SetMass(2.0f);
	gState->rgdBall->SetInertiaTensor(neSphereInertiaTensor(1.5f, 2.0f));
	ballPos.Set(0.0f, 5.0f, 0.0f);
	gState->rgdBall->SetPos(ballPos);
	
	/* Setup Floor */
	neV3 floorPos;
	gState->aniFloor = gState->simTok->CreateAnimatedBody();
	neGeometry *geoFloor = gState->aniFloor->AddGeometry();
	geoFloor->SetBoxSize(30.0f, 1.0f, 30.0f);
	gState->aniFloor->UpdateBoundingInfo();
	floorPos.Set(0.0f, 1.0f, 0.0f);
	gState->aniFloor->SetPos(floorPos);
	
	return true;
}

void* naCreate(void * cb) {
	TRACE("NA");
	tkState* gState = reinterpret_cast<tkState*>(malloc(sizeof(tkState)));
	if(gState) {
		gState->cb = cb;
		/* Initialise the Simulator */
		initSim(gState);
		/* Initialise the items in the simulation */
		initBodies(gState);
	}
	return gState;
}

void naDestroy (void * userData) {
	TRACE("NA");
	tkState *gState = reinterpret_cast<tkState*>(userData);
	neSimulator::DestroySimulator(gState->simTok);
	LOG_INFO("NA", "TOKAMAK deleted");
	free(gState);
}

void initLights() {

	GLfloat light_pos [] = {0.0f, 0.0f, 50.0f, 0.0f};
	GLfloat  light_diffuse_color [] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_specular_color [] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular_color);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, light_specular_color);
	glLightfv (GL_LIGHT0, GL_POSITION, light_pos);

	glLightfv (GL_LIGHT1, GL_SPECULAR, light_specular_color);
	glLightfv (GL_LIGHT1, GL_DIFFUSE, light_specular_color);
	glLightfv (GL_LIGHT1, GL_POSITION, light_pos);

	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glEnable (GL_LIGHT1);
}

int naInit(void *userData) {
	TRACE("NA");
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glClearDepthf(1.0f);
	initLights();
	
	return TRUE;
}

void naUpdate(void *userData, float elapsedTime, nInput * input) {
	TRACE("NA");
	tkState *gState = reinterpret_cast<tkState*>(userData);
	
	if(input->buttons & BUTTON_FIRE) {
		neV3 ballPos;
		ballPos.Set(0, 5, 0);
		gState->rgdBall->SetPos(ballPos);
	}

	gState->simTok->Advance(0.01f);
}

int naRender(void *userData) {
	TRACE("NA");
	tkState *gState = reinterpret_cast<tkState*>(userData);
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	
	ugluLookAtf(0.0f, 5.0f, -25.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f);

	glPushMatrix();
		glPushMatrix();
			neT3 ballPos = gState->rgdBall->GetTransform();
			glMultMatrixf(reinterpret_cast<GLfloat*>(&ballPos));
			ugSolidSpheref(1.5f, 30.0f, 30.0f);
		glPopMatrix();
	
		glPushMatrix();
			glScalef(30.0f, 1.0f, 30.0f);
			ugSolidCubef(1.0f);
		glPopMatrix();
	glPopMatrix();

	return TRUE;
}

int naResize (void * userData, int w, int h) {
	TRACE("NA");
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ugluPerspectivef(35.0f, (GLfloat) w / (GLfloat) h, 0.1f, 350.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return TRUE;
}
