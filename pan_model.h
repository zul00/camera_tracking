#ifndef __PAN_MODEL_H
#define __PAN_MODEL_H

#include "xxmatrix.h"

extern XXDouble pan_C[];    /* constants */
extern XXDouble pan_P[];    /* parameters */
extern XXDouble pan_I[];    /* initial values */
extern XXDouble pan_V[];   /* variables */
extern XXDouble pan_s[];    /* states */
extern XXDouble pan_R[];    /* rates (or new states) */
extern XXMatrix pan_M[];    /* matrices */
extern XXDouble pan_U[];    /* unnamed */
extern XXDouble pan_workarray[];
extern XXDouble pan_F[];    /* favorite parameters */
extern XXDouble pan_f[];    /* favorite variables */

extern XXDouble pan_step_size;

void PanModelInitialize (void);
void PanCalculateDynamic (void);
void PanCalculateOutput (void);
void PanCopyInputsToVariables (XXDouble *u);
void PanCopyVariablesToOutputs (XXDouble *y);

#endif
