#ifndef __VISION_H
#define __VISION_H

#include <stdint.h>

#define ARCH
#define SHOW_GUI

#ifdef ARCH    // Must check in other platform
#include <opencv2/core/fast_math.hpp>
#include <opencv2/videoio/videoio_c.h>
#endif

#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

//void createTrackbars();
void drawObject(int x, int y, IplImage *im);
void morphOps(IplImage *in, IplImage *out);
void trackFilteredObject(int16_t *x, int16_t *y, IplImage *in, IplImage *ref);
int16_t visionConfig(CvCapture **cap, uint16_t cam_id);

// HSV Filter value
extern int H_MIN;
extern int H_MAX;
extern int S_MIN;
extern int S_MAX;
extern int V_MIN;
extern int V_MAX;

// Camera Config
#define FRAME_WIDTH   160 
#define FRAME_HEIGHT  120

// Window Name
#define LIVE  "LiveFeed"
#define HSV   "HSV"
#define BIN   "Bin"
#define MORPH "Morph"
#define TRACKBAR "Trackbars"


#endif
