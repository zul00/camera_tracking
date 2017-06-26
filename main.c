/**
 * @file  main.c
 * @brief Control two joint JIWY module
 *
 * OpenCV reference:
 * https://www.youtube.com/watch?v=bSeFrPrqZ2A
 * by Kyle Hounslow
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "vision.h"

#include "gpmc_driver_c.h"

#define ENC_TILT  16
#define ENC_PAN   12
#define DIR_TILT  20
#define DUTY_TILT 22
#define DIR_PAN   24
#define DUTY_PAN  26

#include "xxmatrix.h"

#define ENC_PAN_PULSES_PER_ROTATION 4000
#define ENC_TILT_PULSES_PER_ROTATION 900

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

XXDouble tilt_step_size = 0.01;

double convert_counter_to_radian(long c, int16_t pulses_per_rotation);
void PanModelInitialize (void);
void PanCalculateDynamic (void);
void PanCalculateOutput (void);
void PanCopyInputsToVariables (XXDouble *u);
void PanCopyVariablesToOutputs (XXDouble *y);
void TiltModelInitialize (void);
void TiltCalculateDynamic (void);
void TiltCalculateOutput (void);
void TiltCopyInputsToVariables (XXDouble *u);
void TiltCopyVariablesToOutputs (XXDouble *y);

#define PI 3.14159265359
#define ANGLE PI/2

double getAngleHorizontal(int pixels)
{
  return (double) ANGLE/2*pixels/FRAME_WIDTH;  
}

double getAngleVertical(int pixels)
{
  return (double) ANGLE/2*pixels/FRAME_HEIGHT;
}

int main(int argc, char *argv[])
{
  CvCapture **cap;   // Store camera
  IplImage *im, *imHSV, *imBin, *imMorph;  // store frame
  CvSize size;

  int16_t x = 0, y = 0;
  double pan = 0, tilt = 0;
  clock_t start = 0, end = 0;

  int16_t enc_tilt_value;
  int16_t enc_pan_value;

  int16_t pwm_tilt_direction = 0;
  long pwm_tilt_duty_cycle = 0;
  int16_t pwm_pan_direction = 0;
  uint16_t pwm_pan_duty_cycle = 0;

  XXDouble pan_u [2 + 1];
  XXDouble pan_y [2 + 1];

  XXDouble tilt_u [3 + 1];
  XXDouble tilt_y [1 + 1];

  printf("Welcoming OpenCV to C!!!\n");


  /* Initialize */
  // Initialize vision
  cap = malloc(sizeof(CvCapture*));
  if (visionConfig(cap, 1) != 0)
  {
    exit(-1);
  }
  // Open GPMC
#ifndef ARCH
  if (2 != argc)
  {
    printf("Usage: %s <device_name>\n", argv[0]);
    return 1;
  }
  printf("Opening gpmc_fpga...\n");
  int fd = open(argv[1], 0);
  if (0 > fd)
  {
    printf("Error, could not open device: %s.\n", argv[1]);
    return 1;
  }
#endif
  // Initialize Variable 
  pan_u[0] = 0.0;   // Initialize pan value input
  pan_u[1] = 0.0;   // Initialize pan value position 
  pan_y[0] = 0.0;   // Initialize pan value correction 
  pan_y[1] = 0.0;   // Initialize pan value output 
  tilt_u[0] = 0.0;    // Initialize tilt value correction
  tilt_u[1] = 0.0;    // Initialize tilt value input
  tilt_u[2] = 0.0;    // Initialize tilt value position
  tilt_y[0] = 0.0;    // Initialize tilt value output

  // Initilize Control
  PanModelInitialize();  
  TiltModelInitialize();

  for(;;)
  {
    // Start timer
    start = clock();

    /* Image processing */
    // Load next frame
    im=cvQueryFrame(*cap);
    if(!im)
      break;

    // Convert color to HSV
    size.width  = im->width;
    size.height = im->height;
    imHSV = cvCreateImage(size, im->depth, im->nChannels);
    cvCvtColor(im, imHSV, CV_BGR2HSV);

    // Thresholding image
    imBin = cvCreateImage(size, IPL_DEPTH_8U, 1);
    cvInRangeS(imHSV, 
        cvScalar(H_MIN,S_MIN,V_MIN, 0),cvScalar(H_MAX,S_MAX,V_MAX, 0),imBin);

    // Morph binary image
    imMorph = cvCreateImage(size, IPL_DEPTH_8U, 1);
    morphOps(imBin, imMorph);

    // Show present frame
#ifdef SHOW_GUI
    cvShowImage("HSV", imHSV);
    cvShowImage("Bin", imBin);
    cvShowImage("Morph", imMorph);
#endif

    // Track Object
    trackFilteredObject(&x,&y, imMorph, im);

    pan = getAngleHorizontal(x-FRAME_WIDTH/2);
    tilt = getAngleVertical(-y+FRAME_HEIGHT/2);

    printf("Angle!! %.2f, %.2f\n",
        pan,
        tilt);

    // Stop timer
    end = clock();
    printf("Processing Time = %2.2f ms\n",
        (1000.0*(end-start))/CLOCKS_PER_SEC );

    pan_u[0] = pan;     // Initialize pan value input
    tilt_u[1] = tilt;   // Initialize tilt value input

    /* Get encoder counter value from pan and tilt */
#ifndef ARCH
    enc_tilt_value = getGPMCValue(fd, ENC_TILT);
    enc_pan_value = getGPMCValue(fd, ENC_PAN);
#else
    enc_tilt_value = 0;
    enc_pan_value = 0;
#endif

    //printf("ENC TILT = %d, ENC PAN = %d\n", enc_tilt_value, enc_pan_value);

    /* Convert pan encoder counter to radians and give as input to controller */
    pan_u[1] = convert_counter_to_radian(enc_pan_value, ENC_PAN_PULSES_PER_ROTATION); 
    tilt_u[2] = convert_counter_to_radian(enc_tilt_value, ENC_TILT_PULSES_PER_ROTATION); 

    printf ("pan_u = %.2f; tilt_u = %.2f\n", pan_u[1], tilt_u[2]);

    /* Pan Controller calculations */
    PanCopyInputsToVariables(pan_u);
    PanCalculateDynamic();
    PanCalculateOutput();
    PanCopyVariablesToOutputs(pan_y);

    /* Correction */
    tilt_u[0] = pan_y[0];

    /* Tilt Controller calculations */
    TiltCopyInputsToVariables(tilt_u);
    TiltCalculateDynamic();
    TiltCalculateOutput();
    TiltCopyVariablesToOutputs(tilt_y);

    if (pan_y[1] < 0)
    {
      pwm_pan_direction = 2;
      pwm_pan_duty_cycle = (uint16_t) (-2500.0 * pan_y[1]);
    }
    else
    {
      pwm_pan_direction = 1;
      pwm_pan_duty_cycle = (uint16_t) (2500.0 * pan_y[1]);
    }

    if (tilt_y[0] < 0)
    {
      pwm_tilt_direction = 2;
      pwm_tilt_duty_cycle = (uint16_t) (-2500.0 * tilt_y[0]);
    }
    else
    {
      pwm_tilt_direction = 1;
      pwm_tilt_duty_cycle = (uint16_t) (2500.0 * tilt_y[0]);
    }

    printf("\tTILT DUTY = %5u;",
        pwm_tilt_duty_cycle);

    printf("TILT DIR = %d\n",
        pwm_tilt_direction);

    printf("\tPAN DUTY = %5u;",
        pwm_pan_duty_cycle);

    printf("PAN DIR =  %d\n",
        pwm_pan_direction);

#ifndef ARCH
    setGPMCValue(fd, pwm_tilt_direction, DIR_TILT);
    setGPMCValue(fd, pwm_tilt_duty_cycle, DUTY_TILT);
    setGPMCValue(fd, pwm_pan_direction, DIR_PAN);
    setGPMCValue(fd, pwm_pan_duty_cycle, DUTY_PAN);

    usleep(1000);
#else
    // Escape sequence
    char c=cvWaitKey(33);
    if(c==27)
      break;
#endif
  }

  // CleanUp
  cvReleaseCapture(cap);
  cvDestroyAllWindows();
  free(cap);

  return 0;
}



/***********************************************************/

double convert_counter_to_radian(long c, int16_t pulses_per_rotation)
{
  return 2*M_PI/pulses_per_rotation*c;
}

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

void TiltModelInitialize (void)
{
  /* set the constants */

  /* set the parameters */
  tilt_P[0] = 0.0;    /* corrGain\K */
  tilt_P[1] = 100;    /* PID1\kp */
  tilt_P[2] = 0.5;    /* PID1\tauD */
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
