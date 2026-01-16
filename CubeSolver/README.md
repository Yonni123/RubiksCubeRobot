# Cube Solver

This is the Android application component of the Rubik's Cube Robot project. It provides a mobile interface for scanning a scrambled Rubik's cube using the device's camera, computing the optimal solving sequence with Kociemba's two-phase algorithm, and sending the solution to the Arduino-controlled robot for execution.

## Disclaimer
I must say that 90% of this app is fully vibe-coded and AI generated. I focused fully on designing the robot mechanics, 3D printing, CAD and the Arduino C++ code. I did NOT want to focus on learning Kotlin or Android Studio with its Compose UI. For these reasons, I let the AI generate most of that code, which breaks every best-practices you can think of. I simply did not care as long as it was working, but this app is an example of how you should NOT use AI in your work :)  But, in this README, I will explain how to use the app rather than how the code works.

## Installation
There should be an APK file somewhere in the Release of this github repo, just install it on any android device that has a camera and Bluetooth.

## Usage

1. **Setup**:
   - Ensure the Arduino robot is powered on and Bluetooth module is active
   - Before launching the app, open bluetooth tab and connect to the robot from there. It should be paired

2. **Bluetooth Connection**:
   - Launch the app and press Connect button at the top, it will show you a list of paired devices, choose the robot

3. **Scanning the Cube**:
   - After connection, go to SCAN tab, and press start scanning.
   - The robot will show you each side it wants to see, and you should scan it from the front of the robot.
   - After scanning all 6 sides, it will go to the confirmation phase, make sure all colors are correctly detected
   - The confirmation phase is important since the AI did not do a good job at color detection from images and I'm too lazy to fix it

4. **Solving**:
   - When scanning is done, you can go back to Home tab and you should see a Cube State along with a Solution
   - If your calibrations were perfect, you should be able to set the speed to 100% and press solve (I was able to)
   - Robot should just execute moves the solution :) If anything goes wrong press the ABORT button.

## Troubleshooting
If you need to troubleshoot anything, don't do it in the android app as it can not do much. Remove the HC-06 module, connect the arduino to your computer and give it SEQ and MOVE commands from the Arduino IDE Serial Monitor.

## Development
For code improvements or modifications, refer to the source code in `app/src/main/`.
