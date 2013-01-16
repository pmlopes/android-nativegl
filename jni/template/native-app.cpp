#include <GL/glut.h> 
#include <tokamak.h>

struct tkState {
	neSimulator *simTok;        // Simulation Object
	neRigidBody *rgdBall;		// Rigid Body for the ball
	neAnimatedBody *aniFloor;	// Animated body for the Floor
};

bool initSim(struct tkState* gState) {
	neV3 gravity; gravity.Set(0.0f, -10.f, 0.0f);
	neSimulatorSizeInfo sizeInfo;
	sizeInfo.rigidBodiesCount = 1;
	sizeInfo.animatedBodiesCount = 1;
	sizeInfo.geometriesCount = 2;
	sizeInfo.overlappedPairsCount = 2;
	gState->simTok = neSimulator::CreateSimulator(sizeInfo, NULL, &gravity);
	return true;
}

bool initBodies(struct tkState* gState) {
	// Setup Ball
	neV3 ballPos;
	gState->rgdBall = gState->simTok->CreateRigidBody();
	neGeometry *geoBall = gState->rgdBall->AddGeometry();
	geoBall->SetSphereDiameter(1.5f);
	gState->rgdBall->UpdateBoundingInfo();
	gState->rgdBall->SetMass(2.0f);
	gState->rgdBall->SetInertiaTensor(neSphereInertiaTensor(1.5f, 2.0f));
	ballPos.Set(0.0f, 5.0f, 0.0f);
	gState->rgdBall->SetPos(ballPos);
	
	//Setup Floor
	neV3 floorPos;
	gState->aniFloor = gState->simTok->CreateAnimatedBody();
	neGeometry *geoFloor = gState->aniFloor->AddGeometry();
	geoFloor->SetBoxSize(30.0f, 1.0f, 30.0f);
	gState->aniFloor->UpdateBoundingInfo();
	floorPos.Set(0.0f, 1.0f, 0.0f);
	gState->aniFloor->SetPos(floorPos);
	
	return true;
}

void* naCreate() {
	struct tkState* gState = (struct tkState*) malloc(sizeof(struct tkState));
	if(gState) {
		gState->simTok = NULL;
		// Initialise the Simulator
		initSim(gState);
		// Initialise the items in the simulation
		initBodies(gState);
	}
	return (void *) gState;
}

void naDestroy (void * userData) {
	struct tkState *gState = (struct tkState *) userData;
	if(gState) {
		neSimulator::DestroySimulator(gState->simTok);
		free(gState);
	}
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	initLights();
	
	return TRUE;
}

int naRender(void *userData, long tick) {
	struct tkState *gState = (struct tkState *) userData;
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	
	gluLookAt(0.0f, 5.0f, -25.0f, 
				0.0f, 0.0f, 0.0f, 
				0.0f, 1.0f, 0.0f);	

	glPushMatrix();
		glPushMatrix();
			neT3 ballPos = gState->rgdBall->GetTransform();
			glMultMatrixf((GLfloat*) &ballPos);
			glutSolidSphere(1.5f, 30.0f, 30.0f);
		glPopMatrix();
	
		glPushMatrix();
			glScalef(30.0f, 1.0f, 30.0f);
			glutSolidCube(1.0f);
		glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
	gState->simTok->Advance(0.01f);
	return TRUE;
}

int naResize ( void * userData, int w, int h ) {
	glViewport(0,0,(GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective (35.0,(GLfloat)w/(GLfloat)h,0.1,350);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static void* userData;

void display() { 
	naRender(userData, 0);
}

void reshape(int w, int h) {
	naResize(userData, w, h);
}

void sampleKeyFunc(unsigned char c, int x, int y) {
	struct tkState *gState = (struct tkState *) userData;
	
	switch (c)
	{
	case 27:
		exit(0);
		break;
	case 'f':
	case 'F':
		{
			glutFullScreen ();
		}
	break;
	case 'a':
	case 'A':
		{
			neV3 ballPos;
			ballPos.Set(0,5,0);
			gState->rgdBall->SetPos(ballPos);
		}
	break;
	}
}

int main (int argc, char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(240,320);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Tokamak Introduction");
	
	userData = naCreate();
	naDestroy(userData);
	
	userData = naCreate();
	naInit(userData);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);
	glutKeyboardFunc(sampleKeyFunc);

	glutMainLoop();
	return 0;
}
