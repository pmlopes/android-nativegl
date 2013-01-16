#ifndef _NA_H_
#define _NA_H_ 1

#define DEBUG 1

#ifdef __cplusplus
extern "C" {
#endif

#define BUTTON_FIRE  0x00001
#define BUTTON_BACK  0x00002
#define BUTTON_UP    0x00010
#define BUTTON_DOWN  0x00020
#define BUTTON_LEFT  0x00040
#define BUTTON_RIGHT 0x00080

typedef struct {
	float dx;
	float dy;
	int buttons;
	float sensor[3];
} nInput;

#ifndef FALSE
#  define FALSE 0
#endif

#ifndef TRUE
#  define TRUE 1
#endif

#ifndef NULL
#define NULL (void *) 0
#endif

#ifdef DEBUG
#include <android/log.h>
#define TRACE(tag) __android_log_print(ANDROID_LOG_DEBUG, tag, "%s:%d @%s", __FILE__, __LINE__, __FUNCTION__)

#define LOG_DEBUG(tag,a) __android_log_print(ANDROID_LOG_DEBUG, tag, a)
#define LOG_INFO(tag,a) __android_log_print(ANDROID_LOG_INFO, tag, a)
#define LOG_ERROR(tag,a) __android_log_print(ANDROID_LOG_ERROR, tag, a)

#define LOG_DEBUGF(tag,fmt,...) __android_log_print(ANDROID_LOG_DEBUG, tag, fmt, __VA_ARGS__)
#define LOG_INFOF(tag,fmt,...) __android_log_print(ANDROID_LOG_INFO, tag, fmt, __VA_ARGS__)
#define LOG_ERRORF(tag,fmt,...) __android_log_print(ANDROID_LOG_ERROR, tag, fmt, __VA_ARGS__)

#else
#define TRACE(tag)

#define LOG_DEBUG(tag,...)
#define LOG_INFO(tag,...)
#define LOG_ERROR(tag,...)

#define LOG_DEBUGF(tag,fmt,...)
#define LOG_INFOF(tag,fmt,...)
#define LOG_ERRORF(tag,fmt,...)
#endif

/* Utils */

/* TODO:
void naPerspective ( float fov, float aspect, float znear, float zfar ) ;
void naLookAt ( float eye_x, float eye_y, float eye_z, float center_x, float center_y, float center_z, float up_x, float up_y, float up_z ) ;
*/

/**
 * Utility method that will update the current input with analog data.
 * After calling this method dx and dy will contain a value between -1 and 1
 * and the buttons will be translated to digital representation of the analog
 * stick.
 *
 * @param userInput the current userInput struct
 */
void naAnalogInput( nInput * userInput ) ;

/**
 * Callback method to allow messages being passed from C to Java
 * @param obj Opaque type of the java object instance
 * @param resId the message resource id to pass back
 */
void naCallback( void * obj, int resId ) ;

/* The simple framework expects the application code to define these functions. */

/**
 * Called only once when the Activity is created. The returned value will be
 * passed back to all calls this allows you not to use global variables.
 * 
 * No GL calls should be made here since the context might not be ready, GL
 * calls should be done on the init method.
 * 
 * @param cb a reference to the current Java Object so you can do callbacks
 * @return userData a opaque pointer to your data (never should be NULL)
 */
void* naCreate ( void * cb ) ;

/**
 * Called when a new GL context is available (should be at least 1 time)
 * 
 * @param userData a opaque pointer to your data (it is always not NULL)
 * @param CB opaque type for the JVM callback function
 * @return BOOLEAN status of the operation
 */
int naReset ( void * userData, void * cb ) ;

/**
 * Notify the application that the rendering area was resized.
 *  - happens at start
 *  - happens when the user changes from landscape to portrait and vice versa
 * 
 * @param userData a opaque pointer to your data (it is always not NULL)
 * @param w new width
 * @param h new height
 * @return BOOLEAN status of the operation
 */
int naResize ( void * userData, int w, int h ) ;

/**
 * You should stop all you are doing and properly clean up because the
 * application will quit.
 * 
 * @param userData a opaque pointer to your data (it is always not NULL)
 */
void naDestroy ( void * userData ) ;

/**
 * The render function. As a rule of thumb do all GL calls here,
 * doing it outside the main render function can end in having no EGL context
 * available which can lead to crashes.
 * 
 * @param userData a opaque pointer to your data (it is always not NULL)
 * @return BOOLEAN status of the operation
 */
int naRender ( void * userData ) ;

/**
 * Update the App state here, do not call GL functions inside this method since
 * it is not guaranteed that the GL context is available.
 * 
 * @param userData a opaque pointer to your data (it is always not NULL)
 * @param elapsedTime time in seconds for each frame
 * @param userInput contains all input changes since last call
 */
void naUpdate ( void * userData, float elapsedTime, nInput * userInput ) ;

#ifdef __cplusplus
}
#endif
#endif
