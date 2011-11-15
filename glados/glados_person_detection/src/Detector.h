#ifndef DETECTOR_EFGH7FNV
#define DETECTOR_EFGH7FNV

#include <ros/ros.h>
#include <geometry_msgs/Point.h>
#include <cv_bridge/CvBridge.h>

class Detector {
public:
  Detector(ros::Publisher *pub, bool doDisplay);
  ~Detector();
  void detect(const sensor_msgs::ImageConstPtr &msg);

private: // functions
  void publishDetection(CvRect *r);
  void displayDetection(IplImage *img, CvRect *r, float scale_x, float scale_y);

private: // data
  ros::Publisher *pub;
  sensor_msgs::CvBridge bridge;
  bool doDisplay;

  CvMemStorage *storage;
  IplImage *small_img;

  // fps stuff
  unsigned int fps_count;
  float fps_freq;
  ros::Time fps_start;

  static const unsigned int SMALL_WIDTH;
  static const unsigned int SMALL_HEIGHT;
};

#endif /* end of include guard: DETECTOR_EFGH7FNV */
