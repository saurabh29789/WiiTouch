# WiiTouch
IR-Based Mouse Navigation

WiiTouch is a IR-based navigation software. It allows any flat surface to be
used as a touch-based input to a computer. This works by using an IR-pen with 
the capability to switch the IR beam on and off as required, and a Nintendo Wii
remote.

# Third-Party Requirements
The software requires the following third-party libraries to be installed:

1. libcwiid
2. libsdl
3. libxdo
4. bluez

# How to Build

Make sure you have the third-party libraries required.
A simple 'make' in the root directory should build your application.

# How to Use

Switch on the device's bluetooth and keep the Nintendo Wiumote handy.
Launch the application and press the 1 + 2 buttons on the remote to have the
application detect the device
Use the IR Pen to calibrate your working area
Done

Now, you can emulate a
1. Single-left click by switching the IR on momentarily
2. Right click by       switching IR on for ~ 1 sec
3. Drag by              moving mouse within the calibrated area
3. Scroll by            moving mouse outside of calibrated area

HOME button - Calibrate the device
Button A    - Exits the application


