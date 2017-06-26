#ifndef __VISION_H
#define __VISION_H

#include <stdint.h>

//void createTrackbars();
void drawObject(int x, int y, IplImage *im);
void morphOps(IplImage *in, IplImage *out);
void trackFilteredObject(int16_t *x, int16_t *y, IplImage *in, IplImage *ref);
int16_t visionConfig(CvCapture **cap);

// HSV Filter value
extern int H_MIN;
extern int H_MAX;
extern int S_MIN;
extern int S_MAX;
extern int V_MIN;
extern int V_MAX;

// Camera Config
#define FRAME_WIDTH   320
#define FRAME_HEIGHT  240

// Window Name
#define LIVE  "LiveFeed"
#define HSV   "HSV"
#define BIN   "Bin"
#define MORPH "Morph"
#define TRACKBAR "Trackbars"

//#define SHOW_GUI

#endif
