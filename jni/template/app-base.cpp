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

typedef struct {
	void * cb;					/* Opaque type for the CB */
} tkState;

void* naCreate(void * cb) {
	TRACE("NA");
	tkState* gState = reinterpret_cast<tkState*>(malloc(sizeof(tkState)));
	if(gState) {
		/* Save a ref to the callback object */
		gState->cb = cb;
		/* Initialise your stuff here */
	}
	return gState;
}

void naDestroy (void * userData) {
	TRACE("NA");
	tkState *gState = reinterpret_cast<tkState*>(userData);
	free(gState);
}

int naInit(void *userData) {
	TRACE("NA");
	
	return TRUE;
}

void naUpdate(void *userData, float elapsedTime, nInput * input) {
	TRACE("NA");
	tkState *gState = reinterpret_cast<tkState*>(userData);
	
	if(input->buttons & BUTTON_FIRE) {
		/* Do something */
	}
}

int naRender(void *userData) {
	TRACE("NA");
	tkState *gState = reinterpret_cast<tkState*>(userData);
	
	return TRUE;
}

int naResize (void * userData, int w, int h) {
	TRACE("NA");
	return TRUE;
}
