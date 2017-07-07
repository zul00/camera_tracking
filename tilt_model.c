#include "xxmatrix.h"
#include "config.h"

/* Variable arrays tilt */
XXDouble tilt_C[0 + 1];   /* constants */
XXDouble tilt_P[7 + 1];   /* parameters */
XXDouble tilt_I[3 + 1];   /* initial values */
XXDouble tilt_V[12 + 1];  /* variables */
XXDouble tilt_s[3 + 1];   /* states */
XXDouble tilt_R[3 + 1];   /* rates (or new states) */
XXMatrix tilt_M[0 + 1];   /* matrices */
XXDouble tilt_U[0 + 1];   /* unnamed */
XXDouble tilt_workarray[0 + 1];
XXDouble tilt_F[0 + 1];   /* favorite parameters */
XXDouble tilt_f[0 + 1];   /* favorite variables */

XXDouble tilt_step_size = PERIOD_MS/1000;

void TiltModelInitialize (void)
{
  /* set the constants */

  /* set the parameters */
  tilt_P[0] = 0.0;    /* corrGain\K */
  tilt_P[1] = 0.1;    /* PID1\kp */
  tilt_P[2] = 0.05;    /* PID1\tauD */
  tilt_P[3] = 0.001;    /* PID1\beta */
  tilt_P[4] = 10.5;   /* PID1\tauI */
  tilt_P[5] = -0.99;    /* SignalLimiter2\minimum */
  tilt_P[6] = 0.99;   /* SignalLimiter2\maximum */

  /* set the initial values */
  tilt_I[0] = 0.0;    /* PID1\uD_previous_initial */
  tilt_I[1] = 0.0;    /* PID1\error_previous_initial */
  tilt_I[2] = 0.0;    /* PID1\uI_previous_initial */

  /* set the states */
  tilt_s[0] = tilt_I[0];    /* PID1\uD_previous */
  tilt_s[1] = tilt_I[1];    /* PID1\error_previous */
  tilt_s[2] = tilt_I[2];    /* PID1\uI_previous */

  /* set the matrices */

  /* set the favorites */
}

void TiltCalculateDynamic (void)
{
  /* PID1\factor = 1 / (sampletime + PID1\tauD * PID1\beta); */
  tilt_V[3] = 1.0 / (tilt_step_size + tilt_P[2] * tilt_P[3]);

  /* corrGain\input = corr; */
  tilt_V[0] = tilt_V[8];

  /* PlusMinus2\plus1 = in; */
  tilt_V[5] = tilt_V[9];

  /* PlusMinus2\minus1 = position; */
  tilt_V[6] = tilt_V[10];

  /* corrGain\output = corrGain\K * corrGain\input; */
  tilt_V[1] = tilt_P[0] * tilt_V[0];

  /* PID1\error = PlusMinus2\plus1 - PlusMinus2\minus1; */
  tilt_R[1] = tilt_V[5] - tilt_V[6];

  /* PID1\uD = PID1\factor * (((PID1\tauD * PID1\uD_previous) * PID1\beta + (PID1\tauD * PID1\kp) * (PID1\error - PID1\error_previous)) + (sampletime * PID1\kp) * PID1\error); */
  tilt_R[0] = tilt_V[3] * (((tilt_P[2] * tilt_s[0]) * tilt_P[3] + (tilt_P[2] * tilt_P[1]) * (tilt_R[1] - tilt_s[1])) + (tilt_step_size * tilt_P[1]) * tilt_R[1]);

  /* PID1\uI = PID1\uI_previous + (sampletime * PID1\uD) / PID1\tauI; */
  tilt_R[2] = tilt_s[2] + (tilt_step_size * tilt_R[0]) / tilt_P[4];

  /* PID1\output = PID1\uI + PID1\uD; */
  tilt_V[2] = tilt_R[2] + tilt_R[0];

  /* PlusMinus1\output = corrGain\output + PID1\output; */
  tilt_V[4] = tilt_V[1] + tilt_V[2];

  /* SignalLimiter2\output = (if PlusMinus1\output < SignalLimiter2\minimum then SignalLimiter2\minimum else (if PlusMinus1\output > SignalLimiter2\maximum then SignalLimiter2\maximum else PlusMinus1\output end) end); */
  tilt_V[7] = ((tilt_V[4] < tilt_P[5]) ? 
      /* SignalLimiter2\minimum */
      tilt_P[5]
      :
      /* (if PlusMinus1\output > SignalLimiter2\maximum then SignalLimiter2\maximum else PlusMinus1\output end) */
      ((tilt_V[4] > tilt_P[6]) ? 
       /* SignalLimiter2\maximum */
       tilt_P[6]
       :
       /* PlusMinus1\output */
       tilt_V[4]
      )
      );
}

void TiltCalculateOutput (void)
{
  /* out = SignalLimiter2\output; */
  tilt_V[11] = tilt_V[7];

  /* set the favorite variables */
}

void TiltCopyInputsToVariables (XXDouble *u)
{
  tilt_V[8] = u[0];   /* corr */
  tilt_V[9] = u[1];   /* in */
  tilt_V[10] = u[2];  /* position */
}

void TiltCopyVariablesToOutputs (XXDouble *y)
{
  y[0] =  tilt_V[11]; /* out */
}
