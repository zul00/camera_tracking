/**
 * @file  main.c
 * @brief Control two joint JIWY module
 * @author Zulkarnaen & Tom Hogenkamp
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
#include <math.h>

#include "vision.h"

#include "gpmc_driver_c.h"

#include "pan_model.h"
#include "tilt_model.h"

#include "config.h"

#define ENC_TILT  16
#define ENC_PAN   12
#define DIR_TILT  20
#define DUTY_TILT 22
#define DIR_PAN   24
#define DUTY_PAN  26

#define ENC_PAN_PULSES_PER_ROTATION 4000
#define ENC_TILT_PULSES_PER_ROTATION 900

double enc_ctr2rad(long c, int16_t pulses_per_rotation);

#define DFOH  0.91   // 52 deg
#define DFOV  0.70   // 40 deg

#define tanDFOH 0.4892  // tan(DFOH/2)
#define tanDFOV 0.3650  // tan(DFOV/2)

/**
 * @brief Change horizontal pixel distance to angle in radian
 */
double px2rad_hor(int pixels)
{
  return (double) atan2((pixels*tanDFOH), FRAME_WIDTH/2);  
}

/**
 * @brief Change vertical pixel distance to angle in radian
 */
double px2rad_ver(int pixels)
{
  return (double) atan2((pixels*tanDFOV), FRAME_HEIGHT/2);
}

#ifdef OVERO
int main(int argc, char *argv[])
#else
int main(void)
#endif
{
  CvCapture **cap;   // Store camera
  IplImage *im, *imHSV, *imBin, *imMorph;  // store frame
  CvSize size;

  int16_t x = 0, y = 0;
  double pan_rad = 0, tilt_rad = 0;
  clock_t start = 0, end = 0, start_old = 0, period = 0;

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
  if (visionConfig(cap, 0) != 0)
  {
    exit(-1);
  }
  // Open GPMC
#ifdef OVERO
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
    /* -SCHEDULER- */
    do
    {
      // Read starting clock
      start = clock();
    }while(1000*(start-start_old)/CLOCKS_PER_SEC < PERIOD_MS);
    period = start - start_old;
    start_old = start;

    /* -IMAGE PROCESSING- */
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

    // Convert distance to radian
    pan_rad  = px2rad_hor(x-FRAME_WIDTH/2);
    tilt_rad = px2rad_ver(-y+FRAME_HEIGHT/2);

    printf("VISION>\n");
    printf("PAN>>dis =%+4d; rad=%+2.2f\n", 
        x-FRAME_WIDTH/2, pan_rad);
    printf("TILT>>dis=%+4d; rad=%+2.2f\n",
        -y+FRAME_HEIGHT/2, tilt_rad);

    /* -FEEDBACK- */
#ifdef OVERO
    enc_tilt_value = getGPMCValue(fd, ENC_TILT);
    enc_pan_value = getGPMCValue(fd, ENC_PAN);
#else
    enc_tilt_value = 0;
    enc_pan_value = 0;
#endif

    // Convert pan encoder counter to radians
    pan_u[1] = enc_ctr2rad(enc_pan_value, ENC_PAN_PULSES_PER_ROTATION); 
    tilt_u[2] = enc_ctr2rad(enc_tilt_value, ENC_TILT_PULSES_PER_ROTATION); 

    printf("FEEDBACK>\n");
    printf ("RAD>>pan = %+1.2f; tilt = %+1.2f\n", pan_u[1], tilt_u[2]);

    /* -CONTROLLER- */
    // Update setpoint
    pan_u[0] = pan_u[1] + pan_rad;    // Use calculated angle from image 
    tilt_u[1] = tilt_u[2] + tilt_rad; // processing as offset from current 
                                      // angle as set point

    // Pan Controller calculations
    PanCopyInputsToVariables(pan_u);
    PanCalculateDynamic();
    PanCalculateOutput();
    PanCopyVariablesToOutputs(pan_y);

    // Correction
    tilt_u[0] = pan_y[0];

    // Tilt Controller calculations
    TiltCopyInputsToVariables(tilt_u);
    TiltCalculateDynamic();
    TiltCalculateOutput();
    TiltCopyVariablesToOutputs(tilt_y);

    // Convert Output to PWM and DIR
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
      pwm_tilt_direction = 1;
      pwm_tilt_duty_cycle = (uint16_t) (-2500.0 * tilt_y[0]);
    }
    else
    {
      pwm_tilt_direction = 2;
      pwm_tilt_duty_cycle = (uint16_t) (2500.0 * tilt_y[0]);
    }

    pwm_tilt_duty_cycle = 0;

    printf("CONTROL>\n");
    printf("TILT>>duty = %4u; dir = %d\n",
        pwm_tilt_duty_cycle, pwm_tilt_direction);
    printf("PAN>>duty  = %4u; dir = %d\n",
        pwm_pan_duty_cycle, pwm_pan_direction);

#ifdef OVERO
    setGPMCValue(fd, pwm_tilt_direction, DIR_TILT);
    setGPMCValue(fd, pwm_tilt_duty_cycle, DUTY_TILT);
    setGPMCValue(fd, pwm_pan_direction, DIR_PAN);
    setGPMCValue(fd, pwm_pan_duty_cycle, DUTY_PAN);
#else
    // Escape sequence
    char c=cvWaitKey(1);
    if(c==27)
      break;
#endif

    // Stop timer and measure time
    end = clock();
    printf("TIME>\n");
    printf("process = %3.2f ms; period = %3.2f ms\n",
        (1000.0*(end-start))/CLOCKS_PER_SEC,
        (1000.0*(period))/CLOCKS_PER_SEC
        );
  }

  // CleanUp
  cvReleaseCapture(cap);
  cvDestroyAllWindows();
  free(cap);

  return 0;
}

/***********************************************************/

double enc_ctr2rad(long c, int16_t pulses_per_rotation)
{
  return 2*M_PI/pulses_per_rotation*c;
}
