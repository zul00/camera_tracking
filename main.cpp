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





/**
 * @brief Get binary map
 */
void morphOps(Mat &in, Mat &out){

  //create structuring element that will be used to "dilate" and "erode" image.
  //the element chosen here is a 3px by 3px rectangle

  Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
  Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

  erode(in,out,erodeElement);
  erode(out,out,erodeElement);

  dilate(out,out,dilateElement);
  //dilate(out,out,dilateElement);
  
  morphologyEx(out, out, MORPH_CLOSE, dilateElement);
}

void drawObject(int x, int y,Mat &frame)
{
  //use some of the openCV drawing functions to draw crosshairs
  //on your tracked image!

  //UPDATE:JUNE 18TH, 2013
  //added 'if' and 'else' statements to prevent
  //memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

  circle(frame,Point(x,y),20,Scalar(0,255,0),2);
  if(y-25>0)
    line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
  else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
  if(y+25<FRAME_HEIGHT)
    line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
  else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
  if(x-25>0)
    line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
  else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
  if(x+25<FRAME_WIDTH)
    line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
  else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);

 // putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);
}


void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed)
{
  Mat temp;
  threshold.copyTo(temp);
  //these two vectors needed for output of findContours
  vector< vector<Point> > contours;
  vector<Vec4i> hierarchy;
  //find contours of filtered image using openCV findContours function
  findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
  //use moments method to find our filtered object
  double refArea = 0;
  bool objectFound = false;
  if (hierarchy.size() > 0) 
  {
    int numObjects = hierarchy.size();
    //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
    if(numObjects<MAX_NUM_OBJECTS)
    {
      for (int index = 0; index >= 0; index = hierarchy[index][0]) 
      { 
        Moments moment = moments((cv::Mat)contours[index]);
        double area = moment.m00;

        if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea)
        {
          x = moment.m10/area;
          y = moment.m01/area;
          objectFound = true;
          refArea = area;
        }
        else
        {
          objectFound = false;
        }
      }
      //let user know you found an object
      if(objectFound ==true)
      {
        putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
        //draw object location on screen
        drawObject(x,y,cameraFeed);}
    }
    else
    {
      putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
    }
  }
}


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
