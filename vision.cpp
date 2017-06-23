#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "vision.hpp"

using namespace std;
using namespace cv;

const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

// HSV Filter value
int H_MIN = 51;
int H_MAX = 94;
int S_MIN = 66;
int S_MAX = 175;
int V_MIN = 77;
int V_MAX = 204;

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
