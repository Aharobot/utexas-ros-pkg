#include "main_window.h"

namespace camera_person_detector {

  using namespace Qt;

  MainWindow::MainWindow(int argc, char** argv, QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    init(argc,argv);
  }

  void MainWindow::init(int argc, char** argv) {
    ros::init(argc, argv, NODE);
    ros::NodeHandle *node = new ros::NodeHandle(), *nh_param = new ros::NodeHandle("~");
    Detector* detector = new Detector();
    detector->setCallback(boost::bind(&MainWindow::draw, this, _1, _2, _3));
    detector->run(*node, *nh_param);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(rosLoop()));
    timer->start(10);
    _frameCount = 0;
    _frameTime = ros::Time::now();
  }
  MainWindow::~MainWindow() {}

  cv::Scalar MainWindow::getColorFromId(unsigned id) {
    uchar r = (id * id % 255);
    uchar g = ((id + 1) * (id + 3)) % 255;
    uchar b = ((id + 5) * (id + 7)) % 255;
    return cv::Scalar(b,g,r);
  }

  void MainWindow::draw(std::vector<Detection>& detections, cv::Mat& image, cv::Mat& foreground) {
    _frameCount++;
    QString frameStr;
    if(_frameCount % FRAME_INTERVAL == 0) {
      double t = (ros::Time::now() - _frameTime).toSec();
      double rate = 0;
      if(t > 0)
        rate = FRAME_INTERVAL / t;
      frameStr = QString::number(rate, 'g', 2) + " Hz";
      ui.lblFrameRate->setText(frameStr);
      _frameTime = ros::Time::now();
    }
    ui.lblDetections->setText(QString::number(detections.size()));
    BOOST_FOREACH(Detection& detection, detections) {
      markDetection(detection,image);
      markDetection(detection,foreground);
    } 
    ui.imgScene->setImage(image);
    ui.imgForeground->setImage(foreground);
    displayStats(detections);
  }

  void MainWindow::displayStats(std::vector<Detection>& detections) {
    QString stats;
    int i = 0;
    BOOST_FOREACH(Detection& detection, detections) {
      i++;
      stats += "Detection " + QString::number(i) + "\n";
      stats += "ID: " + QString::number(detection.id) + "\n";
      stats += 
        QString("Feet Position: (%1,%2)\n")
        .arg(QString::number(detection.feet.x, 'f', 3))
        .arg(QString::number(detection.feet.y, 'f', 3));
      stats += QString("Height: %1\n").arg(QString::number(detection.height, 'f', 3));
      stats += "----------------\n";
    }
    ui.txtStats->setText(stats);
  }

  void MainWindow::markDetection(Detection& detection, cv::Mat& image) {
    cv::Point textPoint(detection.imageBox.x + detection.imageBox.width, detection.imageBox.y);
    bwi_msgs::BoundingBox bb = detection.imageBox;
    cv::Rect rect(bb.x,bb.y,bb.width,bb.height);
    cv::rectangle(image, rect, getColorFromId(detection.id), 3);
    std::stringstream ss; ss << detection.id;
    cv::putText(image, ss.str(), textPoint, 0, 0.5, cv::Scalar(255));
    cv::Point feet(detection.imageFeet.x, detection.imageFeet.y);
    cv::circle(image, feet, 1, cv::Scalar(128,128,0), 1);
  }

  void MainWindow::rosLoop() {
    ros::spinOnce();
  }
}