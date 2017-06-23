#ifndef __VISION_HPP
#define __VISION_HPP

#include <iostream>

void createTrackbars();
void drawObject(int x, int y, cv::Mat &frame);
void morphOps(cv::Mat &in, cv::Mat &out);
void trackFilteredObject(int &x, int &y, cv::Mat threshold, cv::Mat &cameraFeed);


extern const std::string windowName;
extern const std::string windowName1;
extern const std::string windowName2;
extern const std::string windowName3;
extern const std::string trackbarWindowName;

// HSV Filter value
extern int H_MIN;
extern int H_MAX;
extern int S_MIN;
extern int S_MAX;
extern int V_MIN;
extern int V_MAX;

#endif
