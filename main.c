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

#include "pan_model.h"
#include "tilt_model.h"

#define ENC_TILT  16
#define ENC_PAN   12
#define DIR_TILT  20
#define DUTY_TILT 22
#define DIR_PAN   24
#define DUTY_PAN  26

#define ENC_PAN_PULSES_PER_ROTATION 4000
#define ENC_TILT_PULSES_PER_ROTATION 900

double convert_counter_to_radian(long c, int16_t pulses_per_rotation);

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
  clock_t start = 0, end = 0, end_old = 0;

  int16_t enc_tilt_value;
  int16_t enc_pan_value;

  uint16_t pwm_tilt_direction = 0;
  uint16_t pwm_tilt_duty_cycle = 0;
  uint16_t pwm_pan_direction = 0;
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

    printf("VISION>\n");
    printf("PAN>>dis=%+4d;  rad=%+2.2f\n", 
        x-FRAME_WIDTH/2, pan);
    printf("TILT>>dis=%+4d; rad=%+2.2f\n",
        -y+FRAME_HEIGHT/2, tilt);


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

    printf("FEEDBACK>\n");
    printf ("RAD>>pan = %+1.2f; tilt = %+1.2f\n", pan_u[1], tilt_u[2]);

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

    printf("CONTROL>\n");
    printf("TILT>>duty = %4u; dir = %d\n",
        pwm_tilt_duty_cycle, pwm_tilt_direction);

    printf("PAN>>duty  = %4u; dir = %d\n",
        pwm_pan_duty_cycle, pwm_pan_direction);

    // Stop timer
    end = clock();
    printf("TIME>\n");
    printf("process = %3.2f ms; period = %3.2f\n",
        (1000.0*(end-start))/CLOCKS_PER_SEC,
        (1000.0*(end-end_old))/CLOCKS_PER_SEC
        );
    end_old = end;

#ifndef ARCH
    setGPMCValue(fd, pwm_tilt_direction, DIR_TILT);
    setGPMCValue(fd, pwm_tilt_duty_cycle, DUTY_TILT);
    setGPMCValue(fd, pwm_pan_direction, DIR_PAN);
    setGPMCValue(fd, pwm_pan_duty_cycle, DUTY_PAN);

    usleep(1000);
#else
    // Escape sequence
    char c=cvWaitKey(1);
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
