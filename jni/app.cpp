//--------------------------------------------------------------------------------------
// File: app.cpp
//
// Empty starting point for new NA applications
//
// Copyright (c) 2011 Paulo Lopes. All rights reserved.
//--------------------------------------------------------------------------------------

#include "na.h"

void* naCreate(void *cb) {
	TRACE("NA");
//	AppState* gState = new AppState;
//	if(gState) {
//		/* Initialize your stuff here that does not require GL context */
//	}
//	return gState;
	return NULL;
}

void naDestroy(void *userData) {
	TRACE("NA");
//	AppState *gState = reinterpret_cast<AppState*>(userData);
//	if(gState) {
//		/* Delete your stuff */
//	}
//	delete gState;
}

int naReset(void *userData, void *cb) {
	TRACE("NA");
//	/* init GL stuff */
//
//	AppState *gState = reinterpret_cast<AppState*>(userData);
//	if(gState) {
//		/* Initialize your stuff here */
//
//		/* If data is already used, free it first */
//	}
	return TRUE;
}

void naUpdate(void *userData, float elapsedTime, nInput *input) {
	/* TRACE("NA"); */
//	AppState *gState = reinterpret_cast<AppState*>(userData);
//
//	if(input->buttons & BUTTON_FIRE) {
//		/* Do something */
//	}
}

int naRender(void *userData) {
	/* TRACE("NA"); */
//	AppState *gState = reinterpret_cast<AppState*>(userData);
//
//	if(gState) {
//	}

	return TRUE;
}

int naResize(void *userData, int w, int h) {
	TRACE("NA");
	return TRUE;
}
