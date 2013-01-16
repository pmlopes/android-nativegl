#ifndef _CAR_H_
#define _CAR_H_ 1

#include <tokamak.h>
#include <na.h>

const s32 N_CARS = 10;
const s32 N_BODY_BOXES = 2;
const s32 N_PARTS = 3;
const s32 N_RENDER_PRIMITIVES = (N_BODY_BOXES + N_PARTS);
const s32 MAX_OVERLAPPED_PAIR = 1000;
const f32 WHEEL_DIAMETER = 0.9f;
const f32 WHEEL_WIDTH = 0.3f;
const f32 MAX_SPEED = 5.0f;
const f32 MAX_REVERSE_SPEED = -2.0f;
const f32 MAX_STEER = 0.7f;
const f32 MAX_SLIDE = 0.90f;

struct SensorData
{
	neV3 pos;
};

struct PartData
{
	neV3 boxSize;
	neV3 position;
	neV3 jointPos;
	neV3 jointRot;
	f32 lowerLimit;
	f32 upperLimit;
};


class CControllerCB: public neRigidBodyControllerCallback
{
public:
	void RigidBodyControllerCallback(neRigidBodyController * controller, float timeStep);
};


class CCar
{
public:
	void MakeCar(neSimulator * sim, neV3 & pos);

	void MakeParts(neSimulator * sim, neV3 & pos);

	void CarController(neRigidBodyController * controller);

	void Process(nInput * input);

//	CRenderPrimitive * AddRenderPrimitive()
//	{
//		return &carRenderPrimitives[nextRenderPrim++];
//	}

public:
	CCar()
	{
		id = -1;

		accel = 0.0f;

		steer = 0.0f;

		slide = 0.0f;

//		gameWorld = NULL;
//
//		nextRenderPrim = 0;
	}

	neRigidBody * carRigidBody;

	neRigidBody * carParts[N_PARTS];

//	CRenderPrimitive carRenderPrimitives[N_RENDER_PRIMITIVES];
//
//	CRenderPrimitive wheelRenderPrimitive;
//
//	neV3 displayLines[4][2];

	f32 suspensionLength[4];

//	CSampleCar * gameWorld;

	s32 id;

	f32 accel;

	f32 steer;

	f32 slide;

	neV3 steerDir;

//	s32 nextRenderPrim;
};

#endif
