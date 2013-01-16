#include "car.h"

SensorData sensorData[4] =
{
	{2.0f, 0.0f, 1.5f},
	{2.0f, 0.0f, -1.5f},
	{-2.0f, 0.0f, 1.5f},
	{-2.0f, 0.0f, -1.5f},
};

PartData parts[] =
{
	{	// Bonet
		{2.0f, 0.1f, 3.0f},
		{1.65f, 0.8f, 0.0f},
		{0.65f, 0.8f, 0.0f},
		{NE_PI * 0.5f, 0.0f, 0.0f},
		0.0f,
		NE_PI * 0.4f,
	},
	{	//Left Door
		{2.0f, 0.8f, 0.1f},
		{-0.2f, 0.25f, 1.6f},
		{0.65f, 0.25f, 1.6f},
		{0.0f, 0.0f, 0.0f},
		0.0f,
		NE_PI * 0.4f,
	},
	{	//Right Door
		{2.0f, 0.8f, 0.1f},
		{-0.2f, 0.25f, -1.6f},
		{0.65f, 0.25f, -1.6f},
		{0.0f, 0.0f, 0.0f},
		-NE_PI * 0.4f,
		0.0f,
	},
};

CControllerCB cb;

void CControllerCB::RigidBodyControllerCallback(neRigidBodyController * controller, float timeStep)
{
	neRigidBody * rb = controller->GetRigidBody();

	CCar * car = (CCar *)(rb->GetUserData());

	car->CarController(controller);
}

void CCar::MakeCar(neSimulator * sim, neV3 & pos)
{
	f32 mass = 1.0f;

	neRigidBody * rigidBody = sim->CreateRigidBody();

	rigidBody->CollideConnected(true);

	neGeometry * geom = rigidBody->AddGeometry();

	neV3 boxSize;
	{
		// the car consist of two boxes

		boxSize.Set(6.0f, 1.0f, 3.2f);

		geom->SetBoxSize(boxSize);

		neT3 t;

		t.SetIdentity();

		t.pos.Set(0.0f, 0.25f, 0.0f);

		geom->SetTransform(t);

		geom = rigidBody->AddGeometry();

		boxSize.Set(2.5f, 1.0f, 2.8f);

		geom->SetBoxSize(boxSize[0],boxSize[1],boxSize[2]);

		t.pos.Set(-0.6f, 1.0f, 0.0f);

		geom->SetTransform(t);
	}
	{ // add 4 sensors to the rigid body
		neSensor * sn;

		for (s32 si = 0; si < 4; si++)
		{
			sn =rigidBody->AddSensor();

			neV3 snPos, snDir;

			snDir.Set(0.0f, -1.0f, 0.0f);

			snPos.Set(sensorData[si].pos);

			sn->SetLineSensor(snPos, snDir);
		}

		// add a controller to the rigid body

		neRigidBodyController * controller;

		controller = rigidBody->AddController(&cb, 0);
	}

	neV3 tensorSize;

	tensorSize = boxSize;

	rigidBody->UpdateBoundingInfo();

	rigidBody->SetInertiaTensor(neBoxInertiaTensor(tensorSize, mass));

	rigidBody->SetMass(mass);

	rigidBody->SetUserData((u32)this);

	carRigidBody = rigidBody;

	{ // setup the display boxes for the car


		rigidBody->BeginIterateGeometry();

		while (geom = rigidBody->GetNextGeometry())
		{
			neV3 boxSize;

			geom->GetBoxSize(boxSize);

//			CRenderPrimitive * renderPrim = AddRenderPrimitive();

//			renderPrim->SetGraphicBox(boxSize[0], boxSize[1], boxSize[2]);

//			geom->SetUserData((u32)renderPrim);

//			renderPrim->SetDiffuseColor(D3DXCOLOR(0.2f, 0.2f, 0.8f, 1));
		}
	}

//	wheelRenderPrimitive.SetGraphicCylinder(WHEEL_DIAMETER * 0.5f, WHEEL_WIDTH);

	// set the car position

	carRigidBody->SetPos(pos);

	// add bonnet to the car

	MakeParts(sim, pos);
}

void CCar::MakeParts(neSimulator * sim, neV3 & pos)
{
	s32 i;

	for (i = 0; i < N_PARTS; i++)
	{
		neRigidBody * rb = sim->CreateRigidBody();

		if (id == 0 ) //make it harder to sleep
			rb->SetSleepingParameter(0.1f);

		rb->SetPos(parts[i].position + pos);

		neGeometry * geom = rb->AddGeometry();

		geom->SetBoxSize(parts[i].boxSize);

		rb->SetMass(0.01f);

		rb->UpdateBoundingInfo();

		rb->SetInertiaTensor(neBoxInertiaTensor(parts[i].boxSize, 0.01f));

		neJoint * joint = sim->CreateJoint(rb, carRigidBody);

		neT3 trans;

		trans.pos = parts[i].jointPos + pos;

		trans.rot.RotateXYZ(parts[i].jointRot);

		joint->SetJointFrameWorld(trans);

		joint->SetType(neJoint::NE_JOINT_HINGE);

		joint->SetLowerLimit(parts[i].lowerLimit);

		joint->SetUpperLimit(parts[i].upperLimit);

		joint->SetEpsilon(0.0f);

		joint->SetIteration(4);

		joint->EnableLimit(true);

		joint->Enable(true);

		carParts[i] = rb;

//		CRenderPrimitive * renderPrim = AddRenderPrimitive();
//
//		renderPrim->SetGraphicBox(parts[i].boxSize[0], parts[i].boxSize[1], parts[i].boxSize[2]);
//
//		geom->SetUserData((u32)renderPrim);
	}
}

void CCar::CarController(neRigidBodyController * controller)
{
	neT3 body2World = carRigidBody->GetTransform();

	carRigidBody->BeginIterateSensor();

	neSensor * sn;

	neV3 force, torque;

	force.SetZero();

	torque.SetZero();

	s32 i = 0;

	while (sn = carRigidBody->GetNextSensor())
	{
		f32 k = 15.0f; //spring constant

		f32 u = 3.f; //damping constant

		if (i == 2 || i == 3)
		{
			k = 10.0f;
		}

		// add spring force

		f32 depth = sn->GetDetectDepth();

		suspensionLength[i] = 1.0f - depth;

		if (depth == 0.0f)
		{
			i++;
			continue;
		}

		neV3 groundNormal = sn->GetDetectNormal();

		neV3 lineNormal = body2World.rot * sn->GetLineUnitVector();

		neV3 linePos = body2World * sn->GetLinePos();

		neV3 f = depth * lineNormal * -k;

		force += f;

		neV3 r = linePos - carRigidBody->GetPos();

		torque += r.Cross(f);

		// add damping force

		f32 speed = carRigidBody->GetVelocityAtPoint(r).Dot(lineNormal);

		f = -speed * lineNormal * u;

		force += f;

		torque += r.Cross(f);

		// add friction force

		neV3 vel = carRigidBody->GetVelocityAtPoint(sn->GetDetectContactPoint() - carRigidBody->GetPos());

		vel.RemoveComponent(groundNormal);

		if (i == 0 || i == 1)
		{
			//steering

			vel.RemoveComponent(steerDir);
		}
		else
		{
			vel.RemoveComponent(body2World.rot[0]); //rear wheel always parallel to car body
		}


		f = vel;

		f.Normalize();

		f *= -7.0f;

		if (i == 2 || i ==3)
		{
			f *= (1.0f - slide);
			if (slide > MAX_SLIDE)
			{
				f  *= 0.02;
			}
		}
		f[1] = 0.0f;



		force += f;

		r = sn->GetDetectContactPoint() - carRigidBody->GetPos();

		torque += r.Cross(f);

		// driving force

		if (i == 2 || i == 3) // rear wheel
		{
			f = body2World.rot[0];

			f *= accel;

			force += f;

			torque += r.Cross(f);
		}
		i++;
	}

	// drag
	f32 dragConstant = 0.3f;

	neV3 vel = carRigidBody->GetVelocity();

	f32 dot = vel.Dot(body2World.rot[0]);

	neV3 drag = dot * body2World.rot[0] * -dragConstant;

	force += drag;

	controller->SetControllerForce(force);

	controller->SetControllerTorque(torque);
}

void CCar::Process(nInput * input)
{
	neV3 vel;

//	if (GetAsyncKeyState('S'))
//	{
//		accel += 0.1f;
//
//		if (accel > MAX_SPEED)
//			accel = MAX_SPEED;
//	}
//	else if (GetAsyncKeyState('X'))
//	{
//		accel -= 0.1f;
//
//		if (accel < MAX_REVERSE_SPEED)
//			accel = MAX_REVERSE_SPEED;
//	}
//	else if (InputState.Gamepad.bRightTrigger > 0)
//	{
//		accel = (float)InputState.Gamepad.bRightTrigger / 255.0f * MAX_SPEED;
//	}
//	else if (InputState.Gamepad.bLeftTrigger > 0)
//	{
//		accel = (float)InputState.Gamepad.bLeftTrigger / 255.0f * MAX_REVERSE_SPEED;
//	}
//	else
//	{
//		accel *= 0.999999f;
//	}
//
//	if (GetAsyncKeyState('L'))
//	{
//		steer += 0.02f;
//		if (steer > MAX_STEER)
//		{
//			steer = MAX_STEER;
//		}
//	}
//	else if (GetAsyncKeyState('K'))
//	{
//		steer -= 0.02f;
//		if (steer < -MAX_STEER)
//		{
//			steer = -MAX_STEER;
//		}
//	}
//	else if (InputState.Gamepad.sThumbLX != 0)
//	{
//		steer = (float)InputState.Gamepad.sThumbLX / 32767.0f * MAX_STEER;
//
//		if (abs(steer) > (0.9 * MAX_STEER))
//		{
//			slide += 0.02f;
//		}
//	}
//	else
//	{
//		steer *= 0.9f;
//
//		slide *= 0.9f;
//	}
//
//	if (slide > MAX_SLIDE)
//	{
//		slide = MAX_SLIDE;
//	}
//
//	if (GetAsyncKeyState('T'))
//	{
//		slide = MAX_SLIDE+0.5;
//	}
}
