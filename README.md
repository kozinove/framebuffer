# framebuffer

## Стркутура репозитория

* `build_riscv_fb.sh` - скрипт сборки opencv для RISC-V
* `images` - картинки использованные в примерах
* `sample_image_show` - пример отображения изображений во framebuffer
* `test_fb_change` - тест на возможность изменить разрешение framebuffer
* `test_fb_working_time` - тест скорости работы framebuffer
* `sample_fb_opencv_highgui` - тест с интеграцией framebuffer в OpenCV модуль highgui

## Зависимости 

* cmake
* кросс-компилятор gcc для RISC-V процессоров

## Сборка OpenCV c поддержкой framebuffer

OpenCV с текущей интеграцией framebuffer выложена в репозитории `https://github.com/itlab-vision/opencv_framebuffer` (ветка `framebuffer`).  

В fork добавлен ключ `WITH_FRAMEBUFFER`. При значенн ON включется поддержка framebuffer.

Сборка пакета.

```bash
git clone https://github.com/itlab-vision/opencv_framebuffer.git
git checkout framebuffer
bash build_riscv_fb.sh
```
В результате сборки образуются директории
* `opencv-build-fb-openmp` - времменые файлы сборки пакета
* `opencv-install-fb-openmp` - собранная библиотека

## Компиляция и запуск sample_image_show

Компиляция:
```bash
cd sample_image_show
mkdir build
cd build
cmake  -DCMAKE_TOOLCHAIN_FILE=../riscv64-071-gcc.toolchain.cmake -DOpenCV_DIR=<opencv install prefix>/lib/cmake/opencv4 ../
make
```

Пример запуска на LicheePI 4A

```bash
sudo ./image_show starry_night.jpg /dev/input/event1

Image :starry_night.jpg
Input dev path: /dev/input/event1
= Framebuffer's width, height, bits per pix:
1920 1080 32

= Framebuffer's offsets, line length:
0 0 7680

= Initial image width and heigth: 2544 2027

= Image width / heigth: 1.25506
= Image count of channels: 3
= Resized image width and heigth: 1355 1080
```

При запуске приложения картинка отображается на экране. 

По нажатию пробела приложение завершает исполнение


## Компиляция и запуск test_fb_change

Компиляция:
```bash
cd test_fb_change
mkdir build
cd build
cmake  -DCMAKE_TOOLCHAIN_FILE=../riscv64-071-gcc.toolchain.cmake -DOpenCV_DIR=<opencv install prefix>/lib/cmake/opencv4 ../
make
```

Пример запуска на LicheePI 4A

```bash
sipeed@lpi4a:~$ sudo ./test_fb_change starry_night.jpg
Original resolution 1920 1080
Current resolution (test 1) 1024 768
= Initial image width and heigth: 2544 2027

= Image width / heigth: 1.25506
= Image count of channels: 3
= Resized image width and heigth: 963 768

q<ввод>

Original resolution 1920 1080 1920 1080
```
Наблюдаемые эффекты: 
* Параметры меняются и сохраняются при новом открытии framebuffer.
* Реальное разрешение не меняется. При выводе картинка занимает только часть экрана.

**Вывод.** Лучше менять изображение под framebuffer.

## Компиляция и запуск test_fb_working_time

Компиляция:
```bash
cd test_fb_working_time
mkdir build
cd build
cmake  -DCMAKE_TOOLCHAIN_FILE=../riscv64-071-gcc.toolchain.cmake -DOpenCV_DIR=<opencv install prefix>/lib/cmake/opencv4 ../
make
```

Пример запуска на LicheePI 4A

```bash
sipeed@lpi4a:~$ sudo ./test_fb_working_time starry_night.jpg
= Initial image width and heigth:
2544 2027

= Image width / heigth:
1.25506
= Image count of channels:
3
= Resized image width and heigth: 1355 1080

Time (warm-up) :11.7626
Time (test 1) :9.84324
Time (test 2) :11.7439
```

## Компиляция и запуск sample_fb_opencv_highgui

Компиляция:
```bash
cd sample_fb_opencv_highgui
mkdir build
cd build
cmake  -DCMAKE_TOOLCHAIN_FILE=../riscv64-071-gcc.toolchain.cmake -DOpenCV_DIR=<opencv install prefix>/lib/cmake/opencv4 ../
make
```

Пример запуска на LicheePI 4A

* При запуске в текушей директории было найдено и прочитано два изображения 
```bash
sipeed@lpi4a:~$ sudo ./sample_fb_opencv_highgui
Read img: [1600 x 1116]
Read img: [2544 x 2027]
```

* При первом обращении к imshow создается backend Framebuffer если не удалось загрузить другие (в логе ниже последние две строки)

```bash
[DEBUG:0@0.294] global backend.cpp:120 createDefaultUIBackend UI: Initializing backend...
[DEBUG:0@0.294] global registry.impl.hpp:92 UIBackendRegistry UI: Builtin backends(4): GTK(1000); GTK3(990); GTK2(980); Framebuffer(970)
[DEBUG:0@0.294] global registry.impl.hpp:117 UIBackendRegistry UI: Available backends(4): GTK(1000); GTK3(990); GTK2(980); Framebuffer(970)
[ INFO:0@0.294] global registry.impl.hpp:119 UIBackendRegistry UI: Enabled backends(4, sorted by priority): GTK(1000); GTK3(990); GTK2(980); Framebuffer(970)
[DEBUG:0@0.294] global backend.cpp:78 createUIBackend UI: trying backend: GTK (priority=1000)
[DEBUG:0@0.294] global plugin_wrapper.impl.hpp:220 getPluginCandidates UI: GTK plugin's glob is 'libopencv_highgui_gtk*.so', 1 location(s)
[DEBUG:0@0.295] global plugin_wrapper.impl.hpp:230 getPluginCandidates     - .: 0
[DEBUG:0@0.295] global plugin_wrapper.impl.hpp:234 getPluginCandidates Found 0 plugin(s) for GTK
[DEBUG:0@0.295] global backend.cpp:78 createUIBackend UI: trying backend: GTK3 (priority=990)
[DEBUG:0@0.295] global plugin_wrapper.impl.hpp:220 getPluginCandidates UI: GTK3 plugin's glob is 'libopencv_highgui_gtk3*.so', 1 location(s)
[DEBUG:0@0.295] global plugin_wrapper.impl.hpp:230 getPluginCandidates     - .: 0
[DEBUG:0@0.295] global plugin_wrapper.impl.hpp:234 getPluginCandidates Found 0 plugin(s) for GTK3
[DEBUG:0@0.295] global backend.cpp:78 createUIBackend UI: trying backend: GTK2 (priority=980)
[DEBUG:0@0.295] global plugin_wrapper.impl.hpp:220 getPluginCandidates UI: GTK2 plugin's glob is 'libopencv_highgui_gtk2*.so', 1 location(s)
[DEBUG:0@0.295] global plugin_wrapper.impl.hpp:230 getPluginCandidates     - .: 0
[DEBUG:0@0.295] global plugin_wrapper.impl.hpp:234 getPluginCandidates Found 0 plugin(s) for GTK2
[DEBUG:0@0.295] global backend.cpp:78 createUIBackend UI: trying backend: Framebuffer (priority=970)
[ INFO:0@0.295] global backend.cpp:90 createUIBackend UI: using backend: Framebuffer (priority=970)

```

* imshow создает окно, форматирует изображение и отображеет изображение на экране

```bash
FramebufferBackend::createWindow(test, 1)
FramebufferWindow()
FramebufferWindow():: id 4
= Framebuffer's width, height, bits per pix:
1920 1080 32

= Framebuffer's offsets, line length:
0 0 7680

FramebufferWindow::imshow(InputArray image)
<лог от resize>
= Recized image width and heigth:
1548 1080
```

* из backend вызывается ожидание нажатия клавиши. При нажатии стрелок вверх и вниз переключаются изображения. По пробелу приложение завершает основной цикл.

```bash
FramebufferBackend::waitKeyEx(int delay 0)
ch 1 27
ch 2 91
ch 2 66
waitKeyEx:: code 66
key 66 curr_img 1 ./starry_night.jpg
isActive()
FramebufferWindow::imshow(InputArray image)
InputArray image:: size[2544 x 2027]
= Recized image width and heigth:
1355 1080

FramebufferBackend::waitKeyEx(int delay 0)
ch 1 27
ch 2 91
ch 2 65
waitKeyEx:: code 65
key 65 curr_img 0 ./Sunflower_from_Silesia2.jpg
isActive()
FramebufferWindow::imshow(InputArray image)
InputArray image:: size[1600 x 1116]
= Recized image width and heigth:
1548 1080

FramebufferBackend::waitKeyEx(int delay 0)
ch 1 32
waitKeyEx:: code 32
key 32 curr_img 0 ./Sunflower_from_Silesia2.jpg
```

* из backend вызывается ожидание нажатия клавиши с временем ожидания 3 секунды. Если клавиша не была нажата, то возвращается -1. 

```bash
FramebufferBackend::waitKeyEx(int delay 3000)
waitKeyEx:: code -1
key (exit) -1
```

* иначе код клавишы

```bash
FramebufferBackend::waitKeyEx(int delay 3000)
^[f_kbhit 1
d ch 1 27
waitKeyEx:: code 27
key (exit) 27
```
