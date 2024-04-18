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

  std::cout << "= Initial image width and heigth: " << img.cols << " " << img.rows << "\n\n";
  std::cout << "= Image width / heigth: " << aspect_ratio << "\n";
  std::cout << "= Image count of channels: " << img.channels() << "\n";

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
    long int screensize = var_info.xres * var_info.yres * var_info.bits_per_pixel / 8;
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

void test_1(Mat &image) {
  fb_var_screeninfo var_info;
  fb_fix_screeninfo fix_info;
  
  int fb_fd = init_fb(var_info, fix_info);
  unsigned char* fb_p = map_fb(fb_fd, var_info, fix_info);


  cout << "Current resolution (test 1) " << var_info.xres << " " << var_info.yres << std::endl;

  format_image(image, var_info.xres, var_info.yres);
  showImage(fb_p, image, var_info, fix_info);

  munmap(fb_p, var_info.xres * var_info.yres * var_info.bits_per_pixel / 8);
  close(fb_fd);
}

int main(int argc, char ** argv) {
  if(argc < 2)
  {
    printf("<exec> <image path>\n");
    return 0;
  }

  string file_name(argv[1]);
  Mat image = init_image(file_name);

  
  fb_var_screeninfo var_info_orig;
  fb_fix_screeninfo fix_info_orig;
  int fb_fd = init_fb(var_info_orig, fix_info_orig);
  
  cout << "Original resolution " << var_info_orig.xres << " " << var_info_orig.yres << std::endl;
  
  fb_var_screeninfo var_info_new = var_info_orig;
  var_info_new.xres = 1024;
  var_info_new.yres = 786;
  
  if (ioctl(fb_fd, FBIOPUT_VSCREENINFO, &var_info_new)) {
    std::cerr << "EERROR_WRITING_VAR_INFO (resolution)\n";
    exit(1);
  }
  close(fb_fd);

  test_1(image);

  fb_var_screeninfo var_info;
  fb_fix_screeninfo fix_info;

  fb_fd = init_fb(var_info, fix_info);

  if (ioctl(fb_fd, FBIOPUT_VSCREENINFO, &var_info_orig)) {
    std::cerr << "EERROR_WRITING_VAR_INFO (orig)\n";
    exit(1);
  }
  close(fb_fd);


  return 0;
}