#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "vision.h"

// HSV Filter value
#ifdef ARCH
int H_MIN = 23;
int H_MAX = 84;
int S_MIN = 53;
int S_MAX = 171;
int V_MIN = 113;
int V_MAX = 184;
#else
int H_MIN = 28;
int H_MAX = 84;
int S_MIN = 106;
int S_MAX = 195;
int V_MIN = 92;
int V_MAX = 183;
#endif

// Tracking Config
#define MAX_NUM_OBJECTS  50
#define MIN_OBJECT_AREA  20*20
#define MAX_OBJECT_AREA  FRAME_HEIGHT*FRAME_WIDTH/1.5

/**
 * @brief Create trackbars to configure HSV filter value
 */
void createTrackbars()
{
#ifdef SHOW_GUI
  cvNamedWindow(TRACKBAR,0);
  cvCreateTrackbar( "H_MIN", TRACKBAR, &H_MIN, 256, NULL);
  cvCreateTrackbar( "H_MAX", TRACKBAR, &H_MAX, 256, NULL);
  cvCreateTrackbar( "S_MIN", TRACKBAR, &S_MIN, 256, NULL);
  cvCreateTrackbar( "S_MAX", TRACKBAR, &S_MAX, 256, NULL);
  cvCreateTrackbar( "V_MIN", TRACKBAR, &V_MIN, 256, NULL);
  cvCreateTrackbar( "V_MAX", TRACKBAR, &V_MAX, 256, NULL);
#endif
}

/**
 * @brief Configure vision
 */
int16_t visionConfig(CvCapture **cap, uint16_t cam_id)
{
  createTrackbars();

  // Open camera
  *cap = cvCaptureFromCAM(cam_id);

  // Set capture parameter
  cvSetCaptureProperty(*cap, CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH );
  cvSetCaptureProperty(*cap, CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT );

  // Window for LiveFeed
#ifdef SHOW_GUI
  cvNamedWindow(LIVE, CV_WINDOW_AUTOSIZE);
  cvNamedWindow(HSV, CV_WINDOW_AUTOSIZE);
  cvNamedWindow(BIN, CV_WINDOW_AUTOSIZE);
  cvNamedWindow(MORPH, CV_WINDOW_AUTOSIZE);
#endif
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
  cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.5, 0.5, 0, 1, 8);

  // Print text
#ifdef SHOW_GUI
  sprintf(text, "%d,%d", x, y);
  cvPutText(im,text,cvPoint(x,y+30),&font,cvScalar(0,255,0,0));
#endif
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
  cvDilate(out, out, dilate_element, 2);
  //cvMorphologyEx(out, out, out, dilate_element, CV_MOP_CLOSE, 1);
}

/**
 * @brief Object tracking
 * @param x
 * @param y
 * @param threshold
 * @param cameraFeed
 */
void trackFilteredObject(int16_t *x, int16_t *y, IplImage *in, IplImage *ref)
{
  CvMemStorage *storage;
  CvSeq *contours = 0;
  CvMoments mom;

  double refArea = 0;
  bool objectFound = false;
  double area = 0;

  // Prepare storage
  storage = cvCreateMemStorage(0);
  cvClearMemStorage(storage);

  // Find contour
  cvFindContours(in, storage, &contours, sizeof(CvContour),
      CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

  // Scan contour
  for(;contours;contours=contours->h_next)
  {
    // Get moment
    cvMoments(contours, &mom, 0);
    area = cvGetSpatialMoment(&mom, 0, 0); 

    // Check area size to determine validity
    if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea)
    {
      *x = cvGetSpatialMoment(&mom, 1, 0)/area;
      *y = cvGetSpatialMoment(&mom, 0, 1)/area;
      objectFound = true;
      refArea = area;
    }
    else
    {
      objectFound = false;
    }
  }

  // Do something if found
  if(objectFound)
  {
    //printf("Found!! %d, %d\n", *x-FRAME_WIDTH/2, -*y+FRAME_HEIGHT/2);
  }
  else
  {
    *x = FRAME_WIDTH/2;
    *y = FRAME_HEIGHT/2;
  }
#ifdef SHOW_GUI
  drawObject(*x,*y,ref);
  cvShowImage("LiveFeed", ref);
#endif
}
