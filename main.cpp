/**
 * @file  main.c
 * @brief Control two joint JIWY module
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
  Mat im;
  VideoCapture cap;
  int32_t a = 0, b = 0;

  /* Initialize */
  // Initialize camera
  cap.open(1);
  if(!cap.isOpened())
  {
    printf("Failed to open camera\n");
  }
  cap.set(CV_CAP_PROP_FRAME_WIDTH, 160);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, 120);

  printf("Hello cpp\n");

  for(;;)
  {
    a = getTickCount();
    cap >> im;
    if(!im.empty())
    {
      imshow("Image", im);
    }
    b = getTickCount();

    printf("Processing Time = %f ms\n", 1000*((b-a)/getTickFrequency()) );
    waitKey(33);//usleep(1000); 
  }

  return 0;
}
