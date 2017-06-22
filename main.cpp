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

  /* Initialize */
  cap.open(0);

  printf("Hello cpp\n");

  if(!cap.isOpened())
  {
    printf("Failed to open camera\n");
  }

  for(;;)
  {
    cap >> im;
    if(!im.empty())
    {
      imshow("Image", im);
    }

    printf("Hello 123\n");
    waitKey(33);//usleep(1000); 
  }

  return 0;
}
