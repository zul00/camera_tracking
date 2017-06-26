#ifndef __TILT_MODEL_H
#define __TILT_MODEL_H

#include "xxmatrix.h"

extern XXDouble tilt_C[];   /* constants */
extern XXDouble tilt_P[];   /* parameters */
extern XXDouble tilt_I[];   /* initial values */
extern XXDouble tilt_V[];  /* variables */
extern XXDouble tilt_s[];   /* states */
extern XXDouble tilt_R[];   /* rates (or new states) */
extern XXMatrix tilt_M[];   /* matrices */
extern XXDouble tilt_U[];   /* unnamed */
extern XXDouble tilt_workarray[];
extern XXDouble tilt_F[];   /* favorite parameters */
extern XXDouble tilt_f[];   /* favorite variables */

extern XXDouble tilt_step_size;

void TiltModelInitialize (void);
void TiltCalculateDynamic (void);
void TiltCalculateOutput (void);
void TiltCopyInputsToVariables (XXDouble *u);
void TiltCopyVariablesToOutputs (XXDouble *y);


#endif
