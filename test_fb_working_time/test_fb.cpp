#include <iostream>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class TimerForAlg
{
  std::chrono::steady_clock::time_point start;
  std::chrono::steady_clock::time_point end;
  std::string s;
  public:
  TimerForAlg(std::string s)
  {
    this->s = s;
    start = std::chrono::steady_clock::now();
  }
    
  ~TimerForAlg()
  {
    end = std::chrono::steady_clock::now();
    auto diff = end - start; 
    std::cout << "Time (" << s << ") :" 
              << std::chrono::duration <double> (diff).count()
              << std::endl;
  }
};

int init_fb(fb_var_screeninfo& var_info, fb_fix_screeninfo& fix_info) {
  int fbfd = open("/dev/fb0", O_RDWR);
  if (fbfd == -1) {
    cerr << "Error opening framebuffer" << endl;
    exit(1);
  }

  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info)) {
    std::cerr << "ERROR_READING_FIX_INFO\n";
    exit(1);
  }
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &var_info)) {
    std::cerr << "EERROR_READING_VAR_INFO\n";
    exit(1);
  }
  return fbfd;
}

Mat init_image(string file_name) {
  Mat image = imread(file_name);
  if (image.empty()) {
    cerr << "Error loading image" << endl;
    exit(1);
  }

  return image;
}

void format_image(Mat &img, int fb_w, int fb_h)
{
  double aspect_ratio = static_cast<double>(img.cols) / img.rows;
  int new_width = fb_w;
  int new_height = static_cast<int>(fb_w / aspect_ratio);
  int cnt_channel = img.channels();

  std::cout << "= Initial image width and heigth:\n" << img.cols << " " << img.rows << "\n\n";
  std::cout << "= Image width / heigth:\n" << aspect_ratio << "\n";
  std::cout << "= Image count of channels:\n" << img.channels() << "\n";

  if (new_width > fb_w || new_height > fb_h) {
    if (aspect_ratio > static_cast<double>(fb_w) / fb_h) {
      new_width = fb_w;
      new_height = static_cast<int>(fb_w / aspect_ratio);
    } else {
      new_height = fb_h;
      new_width = static_cast<int>(fb_h * aspect_ratio);
    }
  }
  if (img.channels() == 3)
    cvtColor(img, img, COLOR_BGR2BGRA);

  cv::resize(img, img, cv::Size(new_width, new_height), INTER_LINEAR);
 
  std::cout << "= Resized image width and heigth: " 
            << img.cols << " " <<  img.rows << "\n" << std::endl;
  
}

unsigned char* map_fb(int fb_fd, fb_var_screeninfo& var_info, fb_fix_screeninfo& fix_info) {
    long int screensize = var_info.xres_virtual * var_info.yres_virtual * var_info.bits_per_pixel / 8;
    unsigned char* fbp = (unsigned char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fbp == MAP_FAILED) {
        std::cerr << "ERROR_MAP\n";
        exit(1);
    }

    return fbp;
}

void showImage(unsigned char* fbp, Mat& image, fb_var_screeninfo& var_info, fb_fix_screeninfo& fix_info) {   
    int yoffset = var_info.yoffset;
    for (int y = yoffset; y < image.rows + yoffset; y++) {
        std::memcpy(fbp + y * fix_info.line_length, 
                    image.ptr<cv::Vec4b>(y-yoffset), 
                    image.cols*image.channels());
    }
}

void test_1(Mat &image, string test_name) {
  TimerForAlg time(test_name);
  
  fb_var_screeninfo var_info;
  fb_fix_screeninfo fix_info;
  
  int fb_fd = init_fb(var_info, fix_info);
  unsigned char* fb_p = map_fb(fb_fd, var_info, fix_info);

  for (int i = 0; i < 1000; ++i) {
    showImage(fb_p, image, var_info, fix_info);
  }

  munmap(fb_p, var_info.xres_virtual * var_info.yres_virtual * var_info.bits_per_pixel / 8);
  close(fb_fd);
}

void test_2(Mat &image, string test_name) {
  TimerForAlg time(test_name);

  for (int i = 0; i < 1000; ++i) {
    fb_var_screeninfo var_info;
    fb_fix_screeninfo fix_info;
    int fb_fd = init_fb(var_info, fix_info);

    unsigned char* fb_p = map_fb(fb_fd, var_info, fix_info);

    showImage(fb_p, image, var_info, fix_info);

    munmap(fb_p, var_info.xres_virtual * var_info.yres_virtual * var_info.bits_per_pixel / 8);
    close(fb_fd);
  }
}

int main(int argc, char ** argv) {
  if(argc < 2)
  {
    printf("<exec> <image path>\n");
    return 0;
  }

  string file_name(argv[1]);
  Mat image = init_image(file_name);

  fb_var_screeninfo var_info;
  fb_fix_screeninfo fix_info;
  int fb_fd = init_fb(var_info, fix_info);
  
  int fb_w = var_info.xres;
  int fb_h = var_info.yres;
  format_image(image, fb_w, fb_h);

  close(fb_fd);

  test_2(image, "warm-up");
  
  test_1(image, "test 1");
  test_2(image, "test 2");
  

  return 0;
}