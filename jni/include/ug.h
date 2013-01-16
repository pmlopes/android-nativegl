#ifndef __ug_h__
#define __ug_h__

#include <GLES/gl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void ugSolidBox(GLfloat Width, GLfloat Depth, GLfloat Height);
extern void ugSolidConef(GLfloat base, GLfloat height, GLint slices, GLint stacks);
extern void ugSolidCubef(GLfloat size);
extern void ugSolidDisk(GLfloat inner_radius, GLfloat outer_radius, GLshort rings, GLshort slices);
extern void ugSolidSpheref(GLfloat radius, GLint slices, GLint stacks);
extern void ugSolidTorusf(GLfloat innerRadius, GLfloat outerRadius, GLint sides, GLint rings);
extern void ugSolidTube(GLfloat radius, GLfloat height, GLshort stacks, GLshort slices);

extern void ugWireBox(GLfloat Width, GLfloat Depth, GLfloat Height);
extern void ugWireConef(GLfloat base, GLfloat height, GLint slices, GLint stacks);
extern void ugWireCubef(GLfloat size);
extern void ugWireDisk(GLfloat inner_radius, GLfloat outer_radius, GLshort rings, GLshort slices);
extern void ugWireSpheref(GLfloat radius, GLint slices, GLint stacks);
extern void ugWireTorusf(GLfloat innerRadius, GLfloat outerRadius, GLint sides, GLint rings);
extern void ugWireTube(GLfloat radius, GLfloat height, GLshort stacks, GLshort slices);


extern void ugluPerspectivef(GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f);
extern void ugluPerspectivex(GLfixed fovy, GLfixed aspect, GLfixed n, GLfixed f);
extern void ugluLookAtf(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz);
extern void ugluLookAtx(GLfixed eyex, GLfixed eyey, GLfixed eyez, GLfixed centerx, GLfixed centery, GLfixed centerz, GLfixed upx, GLfixed upy, GLfixed upz);

#ifdef __cplusplus
};
#endif

#endif /*__ug_h__*/
