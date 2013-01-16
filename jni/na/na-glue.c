#include <stdlib.h>
#include <jni.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#include <pthread.h>
#include <GLES/gl.h>

#include "na.h"
#include "namodel.h"

#define UNUSED(x) x __attribute__((unused))

static JavaVM* jvm;

static jmethodID j_callback;
static jmethodID j_loadModel;

typedef struct {
	/* timer */
	long lastTick;
	float elapsedTime;
	/* Running State */
	int pause;
	/* Mutex */
	int done;
	pthread_t updateThread;
	pthread_mutex_t vsyncMutex;
	pthread_cond_t vsyncCond;
	/* Input */
	nInput input;
	/* App Data */
	void *userData;
} naAppState;

#define MODEL_GROUP_BUFFER 1
#define MODEL_INDEX_BUFFER 2
#define MODEL_VERTEX_BUFFER 3
#define MODEL_NORMAL_BUFFER 4
#define MODEL_UV_BUFFER 5
#define MODEL_TEX_BUFFER 6

typedef struct {
	/* Usermode model */
	nModel *userModel;
	/* tmp storage for pixel data before upload to GPU */
	short *tex;
	int tw;
	int th;
} naAppModel;

/* Utility exported by the na.h */
void naAnalogInput(nInput * userInput) {

	float angle = atan2f(userInput->dy, userInput->dx);
	/* Update the input struct */
	userInput->dx = cosf(angle);
	userInput->dy = sinf(angle);

	if(userInput->dy > 0) {
		userInput->buttons |= BUTTON_UP;
	}
	if(userInput->dy < 0) {
		userInput->buttons |= BUTTON_DOWN;
	}
	if(userInput->dx > 0) {
		userInput->buttons |= BUTTON_RIGHT;
	}
	if(userInput->dx < 0) {
		userInput->buttons |= BUTTON_LEFT;
	}
}

void naCallback(void * obj, int resId) {

	JNIEnv *env;
	(*jvm)->GetEnv(jvm, (void **)&env, JNI_VERSION_1_6);

	(*env)->CallVoidMethod(env, obj, j_callback, resId);
}

/* Util functions */

static long _getTime(void) {
    struct timeval now;

    gettimeofday(&now, NULL);
    return (long) (now.tv_sec*1000 + now.tv_usec/1000);
}

static void _waitVsync(naAppState *instance) {
	pthread_mutex_lock(&instance->vsyncMutex);
	pthread_cond_wait(&instance->vsyncCond, &instance->vsyncMutex);
	pthread_mutex_unlock(&instance->vsyncMutex);
}

static void * update_thread(void * pRef) {
	naAppState *instance = (naAppState *) pRef;
	if(instance->userData) {
		while(!instance->done) {
			_waitVsync(instance);
			if(instance->done) {
				return;
			}
			/* Do not allow going back in time */
			if(instance->elapsedTime >= 0.f) {
				naUpdate(instance->userData, instance->elapsedTime, & instance->input);
			}
		}
	}
}

/*
 * Class:     jetdrone_nalib_NGLView
 * Method:    nCreate
 * Signature: (Ljetdrone/nalib/JNI;)I
 */
static jint nCreate(JNIEnv *env, jclass clazz, jobject cb) {
	naAppState *instance = malloc(sizeof(naAppState));
	if(instance) {
		instance->pause = 0;

		/* update freq */
		instance->lastTick = _getTime();
		instance->elapsedTime = 0.f;

		/* init pthreads */
		instance->done = FALSE;
		pthread_cond_init(&instance->vsyncCond, NULL);
		pthread_mutex_init(&instance->vsyncMutex, NULL);

		/* Clean User Input struct */
		instance->input.dx = 0.f;
		instance->input.dy = 0.f;
		instance->input.buttons = 0;
		instance->input.sensor[0] = 0.f;
		instance->input.sensor[1] = 0.f;
		instance->input.sensor[2] = 0.f;

		/* User Data */
		void * userData = naCreate(cb);
		if(userData) {
			instance->userData = userData;
			if(!pthread_create(&instance->updateThread, NULL, update_thread, instance)) {
				return (jint) ((jint *) instance);
			}
		}

		free(instance);
		return 0;
	}
	return 0;
}

/*
 * Class:     jetdrone_nalib_NGLView
 * Method:    nDestroy
 * Signature: (I)V
 */
static void nDestroy(JNIEnv *env, jclass clazz, jint pRef) {
	if(pRef) {
		naAppState *instance = (naAppState *) pRef;
		instance->done = TRUE;

		/* signal the thread that we're done */
		pthread_cond_signal(&instance->vsyncCond);

		if(instance->userData) {
			naDestroy(instance->userData);
			instance->userData = NULL;
		}

		/* Delete the current conditional and mutex */
		pthread_cond_destroy(&instance->vsyncCond);
		pthread_mutex_destroy(&instance->vsyncMutex);

		free(instance);
	}
}

/*
 * Class:     jetdrone_nalib_NGLView
 * Method:    nReset
 * Signature: (I)Z
 */
static jboolean nReset(JNIEnv *env, jclass clazz, jint pRef, jobject cb) {
	if(pRef) {
		naAppState *instance = (naAppState *) pRef;
		if(instance->userData) {
			return naReset(instance->userData, (void *) cb);
		}
	}
	LOG_DEBUG("NA","fast exit");
	return JNI_FALSE;
}

/*
 * Class:     jetdrone_nalib_NGLView
 * Method:    nResize
 * Signature: (III)Z
 */
static jboolean nResize(JNIEnv *env, jclass clazz, jint pRef, jint w, jint h) {
	if(pRef) {
		naAppState *instance = (naAppState *) pRef;
		if(instance->userData) {
			/* update freq */
			instance->lastTick = _getTime();
			instance->elapsedTime = 0.f;

			return naResize(instance->userData, w, h);
		}
	}
	return JNI_FALSE;
}

/*
 * Class:     jetdrone_nalib_NGLView
 * Method:    nRender
 * Signature: (I)Z
 */
static jboolean nRender(JNIEnv *env, jclass clazz, jint pRef, jint dx, jint dy, jint buttons, jfloat sensor_x, jfloat sensor_y, jfloat sensor_z) {
	if(pRef) {
		naAppState *instance = (naAppState *) pRef;
		
		if(instance->userData) {
			if (!instance->pause) {
				long curTick = _getTime();

				instance->elapsedTime = (curTick - instance->lastTick) / 1000.f;
				instance->lastTick = curTick;

				jboolean result = naRender(instance->userData);

				/* Update the input struct (for the next update) */
				instance->input.dx = dx;
				instance->input.dy = dy;
				instance->input.buttons = buttons;
				instance->input.sensor[0] = sensor_x;
				instance->input.sensor[1] = sensor_y;
				instance->input.sensor[2] = sensor_z;

				if(pthread_cond_signal(&instance->vsyncCond)) {
					return JNI_FALSE;
				}

				return result;
			}
		}
	}
	return JNI_FALSE;
}

/*
 * Class:     jetdrone_nalib_NGLView
 * Method:    nPause
 * Signature: (I)Z
 */
static jboolean nPause(JNIEnv *env, jclass clazz, jint pRef) {
	if(pRef) {
		naAppState *instance = (naAppState *) pRef;
		if (!instance->pause) {
			instance->pause = TRUE;
		}
		return JNI_TRUE;
	}
	return JNI_FALSE;
}

/*
 * Class:     jetdrone_nalib_NGLView
 * Method:    nResume
 * Signature: (I)Z
 */
static jboolean nResume(JNIEnv *env, jclass clazz, jint pRef) {
	if(pRef) {
		naAppState *instance = (naAppState *) pRef;
		
		if (instance->pause) {
			instance->lastTick = _getTime();
			instance->pause = FALSE;
		}
		return JNI_TRUE;
	}
	return JNI_FALSE;
}

int naLoadModel(void * obj, nModel *model, const char * fileName) {

	if(model) {
		naAppModel appModel;

		model->group_len = 0;
		model->group = NULL;
		model->index_len = 0;
		model->index = NULL;
		model->vertex = NULL;
		model->normal = NULL;
		model->uv = NULL;
		model->texId = 0;

		appModel.userModel = model;
		appModel.tex = NULL;
		appModel.tw = 0;
		appModel.th = 0;

		JNIEnv *env;
		(*jvm)->GetEnv(jvm, (void **)&env, JNI_VERSION_1_6);

		jstring assetName = (*env)->NewStringUTF(env, fileName);
		jint modelRef = (jint) ((jint *) &appModel);
		(*env)->CallVoidMethod(env, (jobject) obj, j_loadModel, modelRef, assetName);
		(*env)->DeleteLocalRef(env, assetName);

		if(appModel.tex) {
			glGenTextures(1, &model->texId);
			glBindTexture(GL_TEXTURE_2D, model->texId);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, appModel.tw, appModel.th, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, appModel.tex);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			/* delete de buffer since the data is uploaded into the GPU mem */
			free(appModel.tex);
		}

		return TRUE;
	}
	return FALSE;
}


/*
 * Class:     jetdrone_nalib_NGLView
 * Method:    nAlloc
 * Signature: (IIIII)Ljava/nio/ByteBuffer;
 */
static jobject nAlloc(JNIEnv *env, jclass clazz, jint pRef, jint type, jint size, jint width, jint height) {
	if(pRef) {
		naAppModel *appModel = (naAppModel *) pRef;
		int capacity = size * width * height;
		if(type == MODEL_GROUP_BUFFER) {
			if(appModel->userModel) {
				appModel->userModel->group = malloc(capacity);
				if(appModel->userModel->group) {
					appModel->userModel->group_len = width;
					return (*env)->NewDirectByteBuffer(env, appModel->userModel->group, capacity);
				}
			}
		} else
		if(type == MODEL_INDEX_BUFFER) {
			if(appModel->userModel) {
				appModel->userModel->index = malloc(capacity);
				if(appModel->userModel->index) {
					appModel->userModel->index_len = width;
					return (*env)->NewDirectByteBuffer(env, appModel->userModel->index, capacity);
				}
			}
		} else
		if(type == MODEL_VERTEX_BUFFER) {
			if(appModel->userModel) {
				appModel->userModel->vertex = malloc(capacity);
				if(appModel->userModel->vertex) return (*env)->NewDirectByteBuffer(env, appModel->userModel->vertex, capacity);
			}
		} else
		if(type == MODEL_NORMAL_BUFFER) {
			if(appModel->userModel) {
				appModel->userModel->normal = malloc(capacity);
				if(appModel->userModel->normal) return (*env)->NewDirectByteBuffer(env, appModel->userModel->normal, capacity);
			}
		} else
		if(type == MODEL_UV_BUFFER) {
			if(appModel->userModel) {
				appModel->userModel->uv = malloc(capacity);
				if(appModel->userModel->uv) return (*env)->NewDirectByteBuffer(env, appModel->userModel->uv, capacity);
			}
		} else
		if(type == MODEL_TEX_BUFFER) {
			if(appModel->userModel) {
				appModel->tex = malloc(capacity);
				if(appModel->tex) {
					appModel->tw = width;
					appModel->th = height;
					return (*env)->NewDirectByteBuffer(env, appModel->tex, capacity);
				}
			}
		}
	}
	return NULL;
}

extern jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    jclass cls;
    /* Cache JVM */
    jvm = vm;
    
    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
		return JNI_ERR;

	/* Class with native methods */
	cls = (*env)->FindClass(env, "jetdrone/nalib/NGLView");
	if(cls == NULL)
		return JNI_ERR;
	
	/* Native methods */
	JNINativeMethod methods[] = {
		// name, signature, function pointer
		{"nCreate", "(Ljetdrone/nalib/NGLView;)I", &nCreate},
		{"nDestroy", "(I)V", &nDestroy },
		{"nReset", "(ILjetdrone/nalib/NGLView;)Z", &nReset },
		{"nResize", "(III)Z", &nResize },
		{"nRender", "(IIIIFFF)Z", &nRender },
		{"nPause", "(I)Z", &nPause },
		{"nResume", "(I)Z", &nResume },
		{"nAlloc", "(IIIII)Ljava/nio/ByteBuffer;", &nAlloc }
	};

    /* register methods */
	(*env)->RegisterNatives(env, cls, methods, 8);

	/* Compute and cache the method CB ID */
    j_callback = (*env)->GetMethodID(env, cls, "callback", "(I)V");
    if(j_callback == NULL) {
		return JNI_ERR;
    }
    j_loadModel = (*env)->GetMethodID(env, cls, "loadModel", "(ILjava/lang/String;)V");
    if(j_loadModel == NULL) {
		return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}
