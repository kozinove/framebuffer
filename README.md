# framebuffer

## Стркутура папока

* images - картинки использованные в примерах
* sample_image_show - пример отображения изображений во framebuffer
* test_fb_change - тест на возможность изменить разрешение framebuffer
* test_fb_working_time - тест скорости работы framebuffer

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
* Параметры меняются и сохраняются при новом открытии буфера.
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


