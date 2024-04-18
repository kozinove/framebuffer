# framebuffer

## Стркутура папока

* test_fb_working_time - тест скорости работы framebuffer


## Компиляция и запуск test_fb_working_time

```bash
cd test_fb_working_time
mkdir build
cd build
cmake  -DCMAKE_TOOLCHAIN_FILE=../riscv64-071-gcc.toolchain.cmake ../
make
cd ..
sudo build/test_fb_working_time starry_night.jpg
```


