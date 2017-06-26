#include "pan_model.h"

/* Variable arrays pan */
XXDouble pan_C[0 + 1];    /* constants */
XXDouble pan_P[7 + 1];    /* parameters */
XXDouble pan_I[3 + 1];    /* initial values */
XXDouble pan_V[10 + 1];   /* variables */
XXDouble pan_s[3 + 1];    /* states */
XXDouble pan_R[3 + 1];    /* rates (or new states) */
XXMatrix pan_M[0 + 1];    /* matrices */
XXDouble pan_U[0 + 1];    /* unnamed */
XXDouble pan_workarray[0 + 1];
XXDouble pan_F[0 + 1];    /* favorite parameters */
XXDouble pan_f[0 + 1];    /* favorite variables */

XXDouble pan_step_size = 0.01;

void PanModelInitialize (void)
{
  /* set the constants */

  /* set the parameters */
  pan_P[0] = 0.0;   /* corrGain\K */
  pan_P[1] = 2.6;   /* PID1\kp */
  pan_P[2] = 0.05;    /* PID1\tauD */
  pan_P[3] = 0.17;    /* PID1\beta */
  pan_P[4] = 9.0;   /* PID1\tauI */
  pan_P[5] = -0.99; /* SignalLimiter2\minimum */
  pan_P[6] = 0.99;    /* SignalLimiter2\maximum */

  /* set the initial values */
  pan_I[0] = 0.0;   /* PID1\uD_previous_initial */
  pan_I[1] = 0.0;   /* PID1\error_previous_initial */
  pan_I[2] = 0.0;   /* PID1\uI_previous_initial */

  /* set the states */
  pan_s[0] = pan_I[0];    /* PID1\uD_previous */
  pan_s[1] = pan_I[1];    /* PID1\error_previous */
  pan_s[2] = pan_I[2];    /* PID1\uI_previous */

  /* set the matrices */

  /* set the favorites */
}

void PanCalculateDynamic (void)
{
  /* PID1\factor = 1 / (sampletime + PID1\tauD * PID1\beta); */
  pan_V[2] = 1.0 / (pan_step_size + pan_P[2] * pan_P[3]);

  /* PlusMinus1\plus1 = in; */
  pan_V[3] = pan_V[7];

  /* Splitter1\input = position; */
  pan_V[5] = pan_V[8];

  /* corrGain\corr = corrGain\K * Splitter1\input; */
  pan_V[0] = pan_P[0] * pan_V[5];

  /* PID1\error = PlusMinus1\plus1 - Splitter1\input; */
  pan_R[1] = pan_V[3] - pan_V[5];

  /* PID1\uD = PID1\factor * (((PID1\tauD * PID1\uD_previous) * PID1\beta + (PID1\tauD * PID1\kp) * (PID1\error - PID1\error_previous)) + (sampletime * PID1\kp) * PID1\error); */
  pan_R[0] = pan_V[2] * (((pan_P[2] * pan_s[0]) * pan_P[3] + (pan_P[2] * pan_P[1]) * (pan_R[1] - pan_s[1])) + (pan_step_size * pan_P[1]) * pan_R[1]);

  /* PID1\uI = PID1\uI_previous + (sampletime * PID1\uD) / PID1\tauI; */
  pan_R[2] = pan_s[2] + (pan_step_size * pan_R[0]) / pan_P[4];

  /* PID1\output = PID1\uI + PID1\uD; */
  pan_V[1] = pan_R[2] + pan_R[0];

  /* SignalLimiter2\output = (if PID1\output < SignalLimiter2\minimum then SignalLimiter2\minimum else (if PID1\output > SignalLimiter2\maximum then SignalLimiter2\maximum else PID1\output end) end); */
  pan_V[4] = ((pan_V[1] < pan_P[5]) ? 
      /* SignalLimiter2\minimum */
      pan_P[5]
      :
      /* (if PID1\output > SignalLimiter2\maximum then SignalLimiter2\maximum else PID1\output end) */
      ((pan_V[1] > pan_P[6]) ? 
       /* SignalLimiter2\maximum */
       pan_P[6]
       :
       /* PID1\output */
       pan_V[1]
      )
      );
}

void PanCalculateOutput (void)
{
  /* corr = corrGain\corr; */
  pan_V[6] = pan_V[0];

  /* out = SignalLimiter2\output; */
  pan_V[9] = pan_V[4];

  /* set the favorite variables */
}

void PanCopyInputsToVariables (XXDouble *u)
{
  pan_V[7] = u[0];    /* in */
  pan_V[8] = u[1];    /* position */
}

void PanCopyVariablesToOutputs (XXDouble *y)
{
  y[0] =  pan_V[6];   /* corr */
  y[1] =  pan_V[9];   /* out */
}
