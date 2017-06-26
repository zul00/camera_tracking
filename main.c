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

#include <opencv2/core/fast_math.hpp>   // Must check in other platform
#include <opencv2/core/core_c.h>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "vision.h"

int main(int argc, char *argv[])
{
  CvCapture **cap;   // Store camera
  IplImage *im, *imHSV, *imBin, *imMorph;  // store frame
  CvSize size;

  int16_t x = 0, y = 0;
  clock_t start = 0, end = 0;

  printf("Welcoming OpenCV to C!!!\n");

  /* Initialize */
  // Initialize vision
  cap = malloc(sizeof(CvCapture*));
  if (visionConfig(cap) != 0)
  {
    exit(-1);
  }

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

    // Stop timer
    end = clock();
    printf("Processing Time = %2.2f ms\n",
        (1000.0*(end-start))/CLOCKS_PER_SEC );

    // Escape sequence
    char c=cvWaitKey(33);
    if(c==27)
      break;
  }

  // CleanUp
  cvReleaseCapture(cap);
  cvDestroyAllWindows();
  free(cap);

  return 0;
}
