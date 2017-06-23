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
int H_MIN = 51;
int H_MAX = 94;
int S_MIN = 66;
int S_MAX = 175;
int V_MIN = 77;
int V_MAX = 204;

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
  createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, 256, NULL);
  createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, 256, NULL);
  createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, 256, NULL);
  createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, 256, NULL);
  createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, 256, NULL);
  createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, 256, NULL);
}


/**
 * @brief Get binary map
 */
void morphOps(Mat &in, Mat &out){

  //create structuring element that will be used to "dilate" and "erode" image.
  //the element chosen here is a 3px by 3px rectangle

  Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
  Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

  erode(in,out,erodeElement);
  erode(in,out,erodeElement);


  dilate(in,out,dilateElement);
  dilate(in,out,dilateElement);
}

int main(int argc, char *argv[])
{
  Mat im, imHSV, imBin, imThresholded;
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
      cvtColor(im,imHSV,COLOR_BGR2HSV);
      inRange(imHSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),imBin);
      morphOps(imBin, imThresholded);

      imshow(windowName,im);
      imshow(windowName1,imHSV);
      imshow(windowName2,imBin);
      imshow(windowName3,imThresholded);
    }
    b = getTickCount();

    printf("Processing Time = %f ms\n", 1000*((b-a)/getTickFrequency()) );
    waitKey(33);//usleep(1000); 
  }

  return 0;
}
