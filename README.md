Enhanced firmware for SparkFun geiger counter
=============================================

Custom firmware for SparkFun geiger counter
(https://www.sparkfun.com/products/10742) with an attached SparkFun
LCD kit (https://www.sparkfun.com/products/9395). Designed to allow
the geiger counter to be used without a computer - I am using mine
with an external Mophie powerpack.

* Supports attached 16x2 LCD on TX pin
    First line: count/second and peak count/second
    Second line: count/minute and peak count/minute
* Count/minute calculated based on last 60 samples
* Uses interrupts for updating UART, etc; as few busy/wait loops as
possible

Known issues
------------
* LCD line will overflow if c/s or c/m greater than 999
* SparkFun SerLCD 2.5 control characters will need to be filtered out
if using computer to capture output via USB

Inspired by aaron's original firmware
(github.com/a1ronzo/SparkFun-Geiger-Counter) but re-written to be
almost entirely interrupt based instead of polling/delay loops.

All files are released under Creative Commons Attribution:
http://creativecommons.org/licenses/by/3.0/
