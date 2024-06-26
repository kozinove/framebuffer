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
#include <termios.h>

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
    const char* input_ev_val = getenv("KEYBOARD_INPUT_EVENT_PATH");
    if (input_ev_val == nullptr) {
        input_ev_val = "/dev/input/event3";
    }
    fd = open(input_ev_val, O_RDONLY);
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

void initTermios(int fd, struct termios* old, struct termios* current) 
{
    tcgetattr(fd, old);               // grab old terminal i/o settings
    *current = *old;                    // make new settings same as old settings
    current->c_lflag &= ~ISIG;
    current->c_lflag &= ~ICANON;       // disable buffered i/o 
    tcsetattr(fd, TCSANOW, current);  // use these new terminal i/o settings now
}

void resetTermios(int fd, struct termios* old) 
{
    tcsetattr(fd, TCSANOW, old);
}

bool kbhit(int fd, struct termios* old, struct termios* current)
{
    int byteswaiting=0;
    //initTermios(fd, old, current);
    if ( ioctl(fd, FIONREAD, &byteswaiting) < 0)
    {
        std::cout << "BAD_IOCTL" << "\n";
    }
    //resetTermios(fd, old);

    return byteswaiting > 0;
}

int WaitKey(int delay)
{
    struct termios old, current;
    int open_fd = input_open();
    if (open_fd == 1) {
        std::cerr << "ERROR_INPUT_EVENT_OPEN\n";
        return -2;
    }

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
        tv.tv_usec = (delay % 1000) * 1000; // microsec
        res = select(open_fd + 1, &fds, NULL, NULL, &tv);
    } else {
        close(open_fd);
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

    struct input_event ev;
    ssize_t r;
    int bytes;
    // Проверка наличия данных для чтения
    bytes = kbhit(open_fd, &old, &current);
    std::cout << bytes << "!!!\n";
    std::cout << open_fd << "===============================\n";
    do {
        // Считывание события
        // Проверка наличия данных для чтения
        bytes = kbhit(open_fd, &old, &current);
        std::cout << bytes << "!_!_!_\n";
        r = read(open_fd, &ev, sizeof(ev));

        if (r == -1) {
            std::cerr << "ERROR_READING_EVENTS\n";
            close(open_fd);
            return -1;
        }
        bytes = kbhit(open_fd, &old, &current);
        std::cout << bytes << "|-|-|-|\n";
    } while (ev.type != EV_KEY || ev.value != 0);
    
    close(open_fd);
    return ev.code;
}

int main(int argc, char* argv[])
{
    if (argc != 3 && argc != 4) {
        std::cerr << "Use for run this code: "
                  << argv[0] 
                  << " <path_to_image> <delay> [input dev path]" 
                  << std::endl;
        return 1;
    }

    std::string image_path = std::string(argv[1]);
    int delay = std::stoi(argv[2]);

    std::string input_dev = "/dev/input/event3";
    if (argc == 4)
    {
      input_dev = std::string(argv[3]);
    }
    setenv("KEYBOARD_INPUT_EVENT_PATH", input_dev.c_str(), 1);
    
    std::cout << "Image :" << image_path << std::endl
              << "Input dev path: " << input_dev << std::endl;
    
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

        key = WaitKey(delay);

        // RESTORE BACKGROUNG
        ioctl(fb_fd, FBIOPUT_VSCREENINFO, &var_info); 

        if (key == -2) {
            std::cerr << "WAIT_KEY_ERROR\n";
            break;
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