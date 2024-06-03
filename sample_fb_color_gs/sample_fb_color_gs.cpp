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
  
  if(count < 1)
  {
    std::cout << "image not found";
    return -1;
  }
  
  int height = 500;
  int step = 500;
  int width = step;
  
  Mat img;
  
  std::string image_path = samples::findFile(fn[0]);
  img = imread(image_path, IMREAD_COLOR);
  std::cout << "Read img: " << img.size() << std::endl;
    
  imgs.push_back(img);
  img = imread(image_path, IMREAD_GRAYSCALE);
  std::cout << "Read img (GS): " << img.size() << std::endl;
  imgs.push_back(img);
  
  cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_VERBOSE);

  namedWindow((fn[0] + "color").c_str(), cv::WINDOW_NORMAL);
  resizeWindow((fn[0] + "color"), step, step);
  moveWindow((fn[0] + "color"), 0, 0);
  imshow((fn[0] + "color"), imgs[0]);

  namedWindow((fn[0] + "GS").c_str(), cv::WINDOW_NORMAL);
  resizeWindow((fn[0] + "GS"), step, step);
  moveWindow((fn[0] + "GS"), 0, step);
  imshow((fn[0] + "GS"), imgs[1]);
    
  waitKey(0);
}
