#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace cv;

int main()
{

  std::vector<cv::String> fn;
  
  cv::glob("*.jpg", fn, false);

  size_t count = fn.size(); 
  
  if(count < 1)
  {
    std::cout << "image not found";
    return -1;
  }
  
  int height = 1000;
  int step = 1000;
  int width = step;

  Mat img;

  std::string image_path = samples::findFile(fn[0]);
  img = imread(image_path, IMREAD_COLOR);
  std::cout << "Read img: " << img.size() << std::endl;

  waitKey(0);

  namedWindow ((fn[0] + "0").c_str(), cv::WINDOW_NORMAL);
  resizeWindow((fn[0] + "0"), step, step);
  moveWindow  ((fn[0] + "0"), 501, 501);
  imshow      ((fn[0] + "0"), img);

  waitKey(0);

  namedWindow ((fn[0] + "1").c_str(), cv::WINDOW_NORMAL);
  resizeWindow((fn[0] + "1"), step, step);
  moveWindow  ((fn[0] + "1"), -50, -50);
  imshow      ((fn[0] + "1"), img);

  waitKey(0);

  namedWindow ((fn[0] + "2").c_str(), cv::WINDOW_NORMAL);
  resizeWindow((fn[0] + "2"), step, step);
  moveWindow  ((fn[0] + "2"), -50, 1000);
  imshow      ((fn[0] + "2"), img);

  waitKey(0);

  namedWindow ((fn[0] + "3").c_str(), cv::WINDOW_NORMAL);
  resizeWindow((fn[0] + "3"), step, step);
  moveWindow  ((fn[0] + "3"), 1000, 1000);
  imshow      ((fn[0] + "3"), img);

  waitKey(0);

  namedWindow ((fn[0] + "4").c_str(), cv::WINDOW_NORMAL);
  resizeWindow((fn[0] + "4"), step, step);
  moveWindow  ((fn[0] + "4"), 1000, -50);
  imshow      ((fn[0] + "4"), img);

  waitKey(0);
}
