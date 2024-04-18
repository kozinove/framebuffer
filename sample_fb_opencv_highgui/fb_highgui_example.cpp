#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <vector>

using namespace cv;

int main()
{

  std::vector<cv::String> fn;
  std::vector<cv::Mat> imgs;
  
  cv::glob("*.jpg", fn, false);

  size_t count = fn.size(); 
  for (size_t i=0; i<count; i++)
  {
    std::string image_path = samples::findFile(fn[i]);
    Mat img = imread(image_path, IMREAD_COLOR);
    std::cout << "Read img: " << img.size() << std::endl;
    
    imgs.push_back(img);
  }


  cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_VERBOSE);
  
  int curr_img = 0;
  int key = 0;
  while(key != 32)
  {
    imshow("test", imgs[curr_img]);
    
    key = waitKey(0);
    
    if(key == 65)
    {
      curr_img = (curr_img + count - 1) % count;
    }
    if(key == 66)
    {
      curr_img = (curr_img + 1) % count;
    }
    std::cout << "key " << key 
              << " curr_img " << curr_img << " " << fn[curr_img] 
              << std::endl;

  }
}
