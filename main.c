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

#include <opencv2/core/fast_math.hpp>   // Must check in other platform
#include <opencv2/core/core_c.h>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "vision.h"

int main(int argc, char *argv[])
{
  CvCapture **cap;   // Store camera
  IplImage *im, *imHSV, *imBin;  // store frame
  CvSize size;

  printf("Welcoming OpenCV to C!!!\n");

  /* Initialize */
  cap = malloc(sizeof(CvCapture*));

  if (visionConfig(cap) != 0)
  {
    exit(-1);
  }

  for(;;)
  {
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
    cvInRangeS(imHSV, cvScalar(H_MIN,S_MIN,V_MIN, 0),cvScalar(H_MAX,S_MAX,V_MAX, 0),imBin);

    // Show present frame
    cvShowImage("LiveFeed", im);
    cvShowImage("HSV", imHSV);
    cvShowImage("Bin", imBin);

    // Escape sequence
    char c=cvWaitKey(33);
    if(c==27)
      break;
  }

  // CleanUp
  cvReleaseCapture(cap);
  cvDestroyAllWindows();
  free(cap);

//  Mat im, imHSV, imBin, imThresholded;
//  VideoCapture cap;
//  int32_t a = 0, b = 0;
//  int x=0, y=0;
//
//  /* Initialize */
//  if (visionConfig(cap) != 0)
//  {
//    exit(-1);
//  }
//
//  printf("Hello cpp\n");
//
//  for(;;)
//  {
//    /* Vision Process */
//    a = getTickCount();
//    cap.read(im);
//    if(!im.empty())
//    {
//      cvtColor(im,imHSV,COLOR_BGR2HSV);
//      inRange(imHSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),imBin);
//      morphOps(imBin, imThresholded);
//      trackFilteredObject(x,y,imThresholded,im);
//
//      imshow(windowName,im);
//      imshow(windowName1,imHSV);
//      imshow(windowName2,imBin);
//      imshow(windowName3,imThresholded);
//    }
//    b = getTickCount();
//
//    printf("Processing Time = %f ms\n", 1000*((b-a)/getTickFrequency()) );
//    printf("Position x = %d, y = %d\n", x, y);
//    printf("Offset x = %d, y = %d\n", x-FRAME_WIDTH/2, -y+FRAME_HEIGHT/2);
//    waitKey(33);//usleep(1000); 
//  }

  return 0;
}
