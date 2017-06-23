/**
 * @file  main.c
 * @brief Control two joint JIWY module
 *
 * OpenCV reference:
 * https://www.youtube.com/watch?v=bSeFrPrqZ2A
 * by Kyle Hounslow
 *
 */
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "vision.hpp"

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
  Mat im, imHSV, imBin, imThresholded;
  VideoCapture cap;
  int32_t a = 0, b = 0;
  int x=0, y=0;

  /* Initialize */
  if (visionConfig(cap) != 0)
  {
    exit(-1);
  }

  printf("Hello cpp\n");

  for(;;)
  {
    /* Vision Process */
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
