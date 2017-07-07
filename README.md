# Hitec Optima PPM

**Only attempt this modification if you know what you are doing and understand the risks.**

Repository forked from [https://github.com/Zero3nna/Optima-PPM] Go there for information about flashing.

For important infomation please visit the blog entry from [untestedprototype](http://untestedprototype.com/2012/08/hitec-optima-ppm/)

There are two options of work(thats some changes):

1. MIXED MODE
  * Regular PWM is output on channels of the receiver and on one selected (#define PPM) channel is output combined PPM.
  * Mixed mode if #define SELECTABLE 0
2. SELECTABLE MODE
  * Regular PWM on all channels or
  * Connect the signal pin to ground (with a bind plug) on selected channel (#define SELECTPIN). Enables CPPM on defined PPM channel.
  * Selectable mode if #define SELECTABLE 1
  
Some builds are available as .hex-file in releases

If you want to build the firmware yourself, you will need to set up a proper build environment and use the provided Rakefile.
