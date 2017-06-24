#include <opencv2/core/fast_math.hpp>   // Must check in other platform
#include <opencv2/core/core_c.h>
#include <opencv2/videoio/videoio_c.h>

#include "vision.h"

//using namespace std;
//using namespace cv;

//const string windowName = "Original Image";
//const string windowName1 = "HSV Image";
//const string windowName2 = "Thresholded Image";
//const string windowName3 = "After Morphological Operations";
//const string trackbarWindowName = "Trackbars";

// HSV Filter value
int H_MIN = 51;
int H_MAX = 94;
int S_MIN = 66;
int S_MAX = 175;
int V_MIN = 77;
int V_MAX = 204;

// Camera Config
#define FRAME_WIDTH   320
#define FRAME_HEIGHT  240

// Tracking Config
#define MAX_NUM_OBJECTS  50
#define MIN_OBJECT_AREA  20*20
#define MAX_OBJECT_AREA  FRAME_HEIGHT*FRAME_WIDTH/1.5

/**
 * @brief Configure vision
 */
int16_t visionConfig(CvCapture *cap)
{
  //createTrackbars();

  // Initialize camera
//  cap.open(1);
//  if(!cap.isOpened())
//  {
//    printf("Failed to open camera\n");
//    return -1;
//  }
//
//  cap.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
//  cap.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

  return 0;
}

///**
// * @brief Create trackbars to configure HSV filter value
// */
//void createTrackbars()
//{
//  namedWindow(trackbarWindowName,0);
//  createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, 256, NULL);
//  createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, 256, NULL);
//  createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, 256, NULL);
//  createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, 256, NULL);
//  createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, 256, NULL);
//  createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, 256, NULL);
//}
//
///**
// * @brief Draw tracking point in Window
// */
//void drawObject(int x, int y, Mat &frame)
//{
//  //use some of the openCV drawing functions to draw crosshairs
//  //on your tracked image!
//
//  //UPDATE:JUNE 18TH, 2013
//  //added 'if' and 'else' statements to prevent
//  //memory errors from writing off the screen (ie. (-25,-25) is not within the window!)
//
//  circle(frame,Point(x,y),20,Scalar(0,255,0),2);
//  if(y-25>0)
//    line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
//  else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
//  if(y+25<FRAME_HEIGHT)
//    line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
//  else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
//  if(x-25>0)
//    line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
//  else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
//  if(x+25<FRAME_WIDTH)
//    line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
//  else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);
//
// // putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);
//}
//
///**
// * @brief Get binary map
// */
//void morphOps(Mat &in, Mat &out)
//{
//  //create structuring element that will be used to "dilate" and "erode" image.
//  //the element chosen here is a 3px by 3px rectangle
//
//  Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
//  Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));
//
//  erode(in,out,erodeElement);
//  erode(out,out,erodeElement);
//
//  dilate(out,out,dilateElement);
//  //dilate(out,out,dilateElement);
//  
//  morphologyEx(out, out, MORPH_CLOSE, dilateElement);
//}
//
///**
// * @brief Object tracking
// * @param x
// * @param y
// * @param threshold
// * @param cameraFeed
// */
//void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed)
//{
//  Mat temp;
//  threshold.copyTo(temp);
//  //these two vectors needed for output of findContours
//  vector< vector<Point> > contours;
//  vector<Vec4i> hierarchy;
//  //find contours of filtered image using openCV findContours function
//  findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
//  //use moments method to find our filtered object
//  double refArea = 0;
//  bool objectFound = false;
//  if (hierarchy.size() > 0) 
//  {
//    int numObjects = hierarchy.size();
//    //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
//    if(numObjects<MAX_NUM_OBJECTS)
//    {
//      for (int index = 0; index >= 0; index = hierarchy[index][0]) 
//      { 
//        Moments moment = moments((cv::Mat)contours[index]);
//        double area = moment.m00;
//
//        if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea)
//        {
//          x = moment.m10/area;
//          y = moment.m01/area;
//          objectFound = true;
//          refArea = area;
//        }
//        else
//        {
//          objectFound = false;
//        }
//      }
//      //let user know you found an object
//      if(objectFound ==true)
//      {
//        putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
//        //draw object location on screen
//        drawObject(x,y,cameraFeed);}
//    }
//    else
//    {
//      putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
//    }
//  }
//}
//
