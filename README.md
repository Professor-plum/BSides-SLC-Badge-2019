# BSides-SLC-Badge-2019
source code, design files and artwork used in the BSides SLC 2019 badge


The following libraries were utilized in this project

https://github.com/rogerclarkmelbourne/Arduino_STM32
https://github.com/PaulStoffregen/SerialFlash
https://github.com/PaulStoffregen/XPT2046_Touchscreen

After installing these libraries, one edit needs to be made:

```
vim ~/Documents/Arduino/libraries/SerialFlash/SerialFlashChip.cpp
```

On line 161, change `SPIPORT.transfer(p, rdlen);` to be `SPIPORT.read(p, rdlen);`. I'm not sure if this is a bug in SerialFlash, or if SerialFlash was just designed to use a different STM32 library.

Now, in the Arduino IDE, you can open `src/BsidesSLCBig/BsidesSLCBig.ino`. Compile using the following board settings:

```
Board: "Generic STM32F103C Series"
Variant: "STM32F103CB (20k RAM, 128k Flash)"
```
