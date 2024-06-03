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
  std::vector<cv::Mat> imgs;
  
  cv::glob("*.jpg", fn, false);

  size_t count = fn.size(); 
  int height = 1000;
  int step = height / count;
  int width = step;
  
  
  for (size_t i=0; i<count; i++)
  {
    std::string image_path = samples::findFile(fn[i]);
    Mat img = imread(image_path, IMREAD_COLOR);
    std::cout << "Read img: " << img.size() << std::endl;
    
    imgs.push_back(img);
  }


  cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_VERBOSE);

  for (size_t i=0; i<count; i++)
  {
    namedWindow(fn[i].c_str(), cv::WINDOW_NORMAL);
    resizeWindow(fn[i].c_str(), step, step);
    moveWindow(fn[i].c_str(), 0, step * i);
    imshow(fn[i].c_str(), imgs[i]);
  }
    
  waitKey(0);
}
