#ifndef _NAMODEL_H_
#define _NAMODEL_H_ 1

#include <GLES/gl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int group_len;
	int *group;
	int index_len;
	short *index;
	GLfixed *vertex;
	GLfixed *normal;
	GLfixed *uv;
	unsigned int texId;
} nModel;

/**
 * Loads a model from the asset manager
 *
 * @param model a initialized model struct to be filled by the loader
 *
 * @return true on success
 */
int naLoadModel( void * jObject, nModel * model, const char * fileName ) ;

/**
 * Frees all resources related to this model
 *
 */
void naFreeModel( nModel * model ) ;

/**
 * Draw the current model
 */
void naDrawModel( nModel * model ) ;

/**
 * Draw the current model
 */
void naDrawModelGroup( nModel * model, int bindBuffers, int groupId ) ;

#ifdef __cplusplus
}
#endif

#endif
