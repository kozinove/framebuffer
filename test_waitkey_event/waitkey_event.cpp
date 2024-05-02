#include <iostream>

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

int image_read(const std::string& path, Mat& mat)
{
    mat = imread(path, IMREAD_COLOR);
    if (mat.empty())
    {
        std::cerr << "ERROR_READING_PICTURE\n";
        return 1;
    }
        
    return 0;
}

int fb_open_and_get_info(fb_var_screeninfo& vinfo, fb_fix_screeninfo& finfo)
{
    int fb_fd = open("/dev/fb0", O_RDWR);
    if (fb_fd == -1)
    {
        std::cerr << "ERROR_OPENING_FB\n";
        return -1;
    }

    // Get fixed screen information
    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo)) {
        std::cerr << "ERROR_READING_FIX_INFO\n";
        return -1;
    }

    // Get variable screen information
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo)) {
        std::cerr << "EERROR_READING_VAR_INFO\n";
        return -1;
    }

    return fb_fd;
}

int input_open()
{
    int fd;
    fd = open("/dev/input/event3", O_RDONLY);
    if (fd == -1) {
        std::cerr << "ERROR_OPENING_INPUT\n";
        return 1;
    }
    return fd;
}

void format_image(Mat &img, int fb_w, int fb_h)
{
  double aspect_ratio = static_cast<double>(img.cols) / img.rows;
  int new_width = fb_w;
  int new_height = static_cast<int>(fb_w / aspect_ratio);
  int cnt_channel = img.channels();

  std::cout << "= Initial image width and heigth: " 
            << img.cols << " " << img.rows << "\n\n";
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

void showImage(unsigned char* fbp, Mat& image, fb_var_screeninfo& var_info, fb_fix_screeninfo& fix_info) {   
    int yoffset = var_info.yoffset;
    int xoffset = var_info.xoffset;
    for (int y = yoffset; y < image.rows + yoffset; y++) {
        std::memcpy(fbp + y * fix_info.line_length + xoffset, 
                    image.ptr<cv::Vec4b>(y-yoffset), 
                    image.cols*image.channels());
    }
}

int WaitKey(int delay)
{
    int open_fd = input_open();

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(open_fd, &fds);

    int res;
    if (delay == 0)
    {
        res = select(open_fd + 1, &fds, NULL, NULL, NULL);
    } else if (delay > 0) {
        struct timeval tv;
        tv.tv_sec = delay / 1000; // sec
        tv.tv_usec = delay % 1000 * 1000; // microsec
        res = select(open_fd + 1, &fds, NULL, NULL, &tv);
    } else {
        std::cerr << "NEGATIVE_DELAY\n";
        return -2;
    }

    if (res == -1) // select error
    {
        std::cerr << "ERROR_SELECT\n";
        close(open_fd);
        return -2;
    }
    if (res == 0) // delay was ended
    {
        close(open_fd);
        return -1;
    }

    // waiting for a KEYDOWN!
    // struct input_event ev[6];
    // ssize_t r = read(open_fd, &ev, sizeof(ev));
    // if (r == -1) {
    //     std::cerr << "ERROR_READING_EVENTS\n";
    //     close(open_fd);
    //     return -1;
    // }
    // if (ev[1].type == EV_KEY && ev[1].value == 1) // key_board action AND keydown
    // {
    //     close(open_fd);
    //     return ev[1].code;
    // }
    // return -2;

    // waiting for a KEYUP

    struct input_event ev;
    do {
        ssize_t r = read(open_fd, &ev, sizeof(ev));
        if (r == -1) {
            std::cerr << "ERROR_READING_EVENTS\n";
            close(open_fd);
            return -1;
        }
    } while (ev.type != EV_KEY || ev.value != 0); // key_board action AND keyup
    
    close(open_fd);
    return ev.code;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Use for run this code: "
                  << argv[0] 
                  << " <path_to_image> <delay>" 
                  << std::endl;
        return 1;
    }

    std::string image_path = std::string(argv[1]);
    int delay = std::stoi(argv[2]);
    
    std::cout << "Image :" << image_path << std::endl;
    
    // FB OPENING
    fb_var_screeninfo var_info;
    fb_fix_screeninfo fix_info;
    int fb_fd = fb_open_and_get_info(var_info, fix_info);
    if (fb_fd == -1) return 1;

    int fb_w = var_info.xres;
    int fb_h = var_info.yres;
    int y_offset = var_info.yoffset;
    int x_offset = var_info.xoffset;
    int bpp = var_info.bits_per_pixel;
    int line_length = fix_info.line_length;

    std::cout << "= Framebuffer's width, height, bits per pix:\n" << fb_w << " " << fb_h << " " << bpp << "\n\n";
    std::cout << "= Framebuffer's offsets, line length:\n" << y_offset << " " << x_offset << " " << line_length << "\n\n";

    // image reading
    Mat img;
    if ( image_read(image_path, img) )  
        return 1; 

    format_image(img, fb_w, fb_h);

    int cnt_channel = img.channels();

    // MAP FB TO MEMORY
    long int screensize = var_info.xres_virtual * var_info.yres_virtual * bpp / 8;
    unsigned char* fbp = (unsigned char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fbp == MAP_FAILED) {
        std::cerr << "ERROR_MAP\n";
        return 1;
    }

    // SAVE BACKGROUNG
    Mat tmp_buff(img.rows, img.cols, CV_8UC4);
    for (int y = y_offset; y < img.rows + y_offset; y++)
    {
        std::memcpy(tmp_buff.ptr<cv::Vec4b>(y - y_offset), 
                    fbp + y * line_length + x_offset, 
                    img.cols * cnt_channel);
    }

    int key;
    do {
        // SHOW IMAGE
        showImage(fbp, img, var_info, fix_info);

        // WaitKey()
        key = WaitKey(delay);

        // RESTORE BACKGROUNG
        //showImage(fbp, tmp_buff, var_info, fix_info); // first variant
        // second variant (works in text format, does not work in the GUI
        ioctl(fb_fd, FBIOPUT_VSCREENINFO, &var_info); 

        if (key == -2) {
            std::cerr << "WAIT_KEY_ERROR\n";
        } else if (key == -1) {
            std::cout << "No key was pressed. The waiting time is over!\n";
        } else {
            std::cout << "The key with the number: " << key << " was pressed.\n";
        }
    } while (key != 1);

    munmap(fbp, screensize);
    close(fb_fd);

    return 0;
}