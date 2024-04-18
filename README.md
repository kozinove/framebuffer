# framebuffer

## Стркутура папока

* test_fb_working_time - тест скорости работы framebuffer


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


