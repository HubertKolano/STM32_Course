# STM32_Course
Hi! This repo is made to track my progress and record most interesting things i learn through the course.
I am learning with the help of the Forbot blog who made this course: https://forbot.pl/blog/kurs-stm32-l4-wstep-spis-tresci-dla-kogo-jest-ten-kurs-id48575 
I follow them for a time, and would consider the blog one of best source of information regarding DIY electronics.

As for enviorement, I've decided to utilize Stm32CubeMX software for generating basic code framework with initialized MCU peripherals chosed via GUI.
To write code I am stm32pio plugin (https://github.com/ussserrr/stm32pio) to integrate it to VsCode PlatformIO enviorement, as I find it the most comfortable IDE to write code in.

Additionally i use different nucleo development board than the one used in course (nucleo-F303RE) but I didn't really notice to make it a big impact nor hinderance (if anything thats just another challange to work on).

# Lesson 1, 2 & 3
Pretty welcoming start with the STM32 platform. On the start I've learnt doing most basic thing like configuring I/O and UART interface (utilizing converter to comunicate with computer via USB) and basic clock and watchdog settings.
Most interesing things learned I'd like to point out:
> Functions that are meant to be overwriten exist (_attribute_((weak));), and you can use them to e.g. print messages to interface using printf(); functions, which is very handy.
> You can create an enum which one of it fields has a pointer to a function, that way you can with ease link certain actions like: button presses cause to do a specific functions.
> Use logarythmic scale to change frequency of visual effects occuring - it not very noticible to change 10Hz to 11Hz of leds flickering as from 1Hz to 2Hz. Changes shall be made proportionally.
> It is often usefull to implement some specific startup behaviour, so you can see if the mcu has been reset by watchdog.