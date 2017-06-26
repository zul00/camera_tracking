#ifndef __VISION_H
#define __VISION_H

//#include <iostream>
#include <stdint.h>

//void createTrackbars();
void drawObject(int x, int y, IplImage *im);
void morphOps(IplImage *in, IplImage *out);
//void trackFilteredObject(int &x, int &y, cv::Mat threshold, cv::Mat &cameraFeed);
int16_t visionConfig(CvCapture **cap);


//extern const std::string windowName;
//extern const std::string windowName1;
//extern const std::string windowName2;
//extern const std::string windowName3;
//extern const std::string trackbarWindowName;

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

#endif
