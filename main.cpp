/**
 * @file  main.c
 * @brief Control two joint JIWY module
 *
 * OpenCV reference:
 * https://www.youtube.com/watch?v=bSeFrPrqZ2A
 * by Kyle Hounslow
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "vision.hpp"

using namespace cv;
using namespace std;


const int FRAME_WIDTH = 320;
const int FRAME_HEIGHT = 240;

const int MAX_NUM_OBJECTS=50;

const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;


int main(int argc, char *argv[])
{
  Mat im, imHSV, imBin, imThresholded;
  VideoCapture cap;
  int32_t a = 0, b = 0;
  int x=0, y=0;

  /* Initialize */
  createTrackbars();
  // Initialize camera
  cap.open(1);
  if(!cap.isOpened())
  {
    printf("Failed to open camera\n");
  }
  cap.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

  printf("Hello cpp\n");

  for(;;)
  {
    a = getTickCount();

    cap.read(im);
    if(!im.empty())
    {
      cvtColor(im,imHSV,COLOR_BGR2HSV);
      inRange(imHSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),imBin);
      morphOps(imBin, imThresholded);
      trackFilteredObject(x,y,imThresholded,im);

      imshow(windowName,im);
      imshow(windowName1,imHSV);
      imshow(windowName2,imBin);
      imshow(windowName3,imThresholded);
    }
    b = getTickCount();

    printf("Processing Time = %f ms\n", 1000*((b-a)/getTickFrequency()) );
    printf("Position x = %d, y = %d\n", x, y);
    printf("Offset x = %d, y = %d\n", x-FRAME_WIDTH/2, -y+FRAME_HEIGHT/2);
    waitKey(33);//usleep(1000); 
  }

  return 0;
}
