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

using namespace cv;
using namespace std;

// HSV Filter value
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

const int FRAME_WIDTH = 160;
const int FRAME_HEIGHT = 120;

const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

/**
 * @brief Create trackbars to configure HSV filter value
 */
void createTrackbars()
{
  namedWindow(trackbarWindowName,0);
  createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, NULL);
  createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, NULL);
  createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, NULL);
  createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, NULL);
  createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, NULL);
  createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, NULL);
}

int main(int argc, char *argv[])
{
  Mat im, imHSV, imBin;
  VideoCapture cap;
  int32_t a = 0, b = 0;

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
      imshow("Image", im);
    }
    b = getTickCount();

    printf("Processing Time = %f ms\n", 1000*((b-a)/getTickFrequency()) );
    waitKey(33);//usleep(1000); 
  }

  return 0;
}
