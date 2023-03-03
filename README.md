# 2D Phased Array Ultrasonic Transmitter
For details on construction [see the report](Report/Report.md).

# Compilation
First have the [RPI Pico SDK](https://github.com/raspberrypi/pico-sdk) downloaded, and have its path set in the `PICO_SDK_PATH` environment variable.
```bash
mkdir -p Software/build
cd Software/Build
cmake ..
make -j$(($(grep -c ^processor /proc/cpuinfo)-1)) # Compile using all available cores -1.
```
