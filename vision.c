#include <stdlib.h>
#include <stdio.h>

#include <opencv2/core/fast_math.hpp>   // Must check in other platform
#include <opencv2/core/core_c.h>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "vision.h"

//using namespace std;
//using namespace cv;

//const string windowName = "Original Image";
//const string windowName1 = "HSV Image";
//const string windowName2 = "Thresholded Image";
//const string windowName3 = "After Morphological Operations";
const char* trackbarWindowName = "Trackbars";

// HSV Filter value
int H_MIN = 51;
int H_MAX = 94;
int S_MIN = 66;
int S_MAX = 175;
int V_MIN = 77;
int V_MAX = 204;


// Tracking Config
#define MAX_NUM_OBJECTS  50
#define MIN_OBJECT_AREA  20*20
#define MAX_OBJECT_AREA  FRAME_HEIGHT*FRAME_WIDTH/1.5

/**
 * @brief Create trackbars to configure HSV filter value
 */
void createTrackbars()
{
  cvNamedWindow(trackbarWindowName,0);
  cvCreateTrackbar( "H_MIN", trackbarWindowName, &H_MIN, 256, NULL);
  cvCreateTrackbar( "H_MAX", trackbarWindowName, &H_MAX, 256, NULL);
  cvCreateTrackbar( "S_MIN", trackbarWindowName, &S_MIN, 256, NULL);
  cvCreateTrackbar( "S_MAX", trackbarWindowName, &S_MAX, 256, NULL);
  cvCreateTrackbar( "V_MIN", trackbarWindowName, &V_MIN, 256, NULL);
  cvCreateTrackbar( "V_MAX", trackbarWindowName, &V_MAX, 256, NULL);
}

/**
 * @brief Configure vision
 */
int16_t visionConfig(CvCapture **cap)
{
  createTrackbars();

  // Open camera
  *cap = cvCaptureFromCAM(1);

  // Set capture parameter
  cvSetCaptureProperty(*cap, CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH );
  cvSetCaptureProperty(*cap, CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT );

  // Window for LiveFeed
  cvNamedWindow("LiveFeed", CV_WINDOW_AUTOSIZE);
  if(!*cap)
  {
    perror("Capture is NULL");
    return 1;
  }

  return 0;
}

/**
 * @brief Draw tracking point in Window
 */
void drawObject(int x, int y, IplImage *im)
{
  char text[40];
  CvFont font;

  // Draw circle
  cvCircle(im,cvPoint(x,y),10,cvScalar(0,255,0,0),2,8,0);

  // Prepare font
  cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.8, 0.8, 0, 1, 8);

  // Print text
  sprintf(text, "%d,%d", x, y);
  cvPutText(im,text,cvPoint(x,y+30),&font,cvScalar(0,255,0,0));
}

/**
 * @brief Get binary map
 */
void morphOps(IplImage *in, IplImage *out)
{
  IplConvKernel *erode_element;
  IplConvKernel *dilate_element;

  // Create elements
  erode_element = cvCreateStructuringElementEx(3, 3, 1, 1, 
      CV_SHAPE_RECT, NULL);
  dilate_element = cvCreateStructuringElementEx(8, 8, 1, 1, 
      CV_SHAPE_RECT, NULL);

  // Erode image
  cvErode(in, out, erode_element, 3);

  // Dilate image
  cvDilate(out, out, dilate_element, 1);
  cvMorphologyEx(out, out, out, dilate_element, CV_MOP_CLOSE, 1);
}

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
}

