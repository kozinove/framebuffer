# framebuffer

## Стркутура папока

* test_fb_change - тест на возможность изменить разрешение framebuffer
* test_fb_working_time - тест скорости работы framebuffer

## Компиляция и запуск test_fb_change

Компиляция:
```bash
cd test_fb_change
mkdir build
cd build
cmake  -DCMAKE_TOOLCHAIN_FILE=../riscv64-071-gcc.toolchain.cmake ../ -DOpenCV_DIR=<opencv install prefix>/lib/cmake/opencv4 ../
make
```

Пример запуска на LicheePI 4A

```bash
sipeed@lpi4a:~$ sudo ./test_fb_change starry_night.jpg
Original resolution 1024 786
Current resolution (test 1) 1024 786
= Initial image width and heigth: 2544 2027

= Image width / heigth: 1.25506
= Image count of channels: 3
= Resized image width and heigth: 986 786

Segmentation fault
```
Наблюдаемые эффекты: 
* Параметры меняются и сохраняются при новом открытии буфера.
* Реальное разрешение не меняется. При выводе картинка занимает только часть экрана.
* После вывода изображения приложение падает.


## Компиляция и запуск test_fb_working_time

Компиляция:
```bash
cd test_fb_working_time
mkdir build
cd build
cmake  -DCMAKE_TOOLCHAIN_FILE=../riscv64-071-gcc.toolchain.cmake ../ -DOpenCV_DIR=<opencv install prefix>/lib/cmake/opencv4 ../
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


