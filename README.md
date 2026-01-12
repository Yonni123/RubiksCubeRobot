# Rubik's Cube Robot

An autonomous robotic system that can visually scan a scrambled Rubik's cube, compute the optimal solution, and solve it through precise servo-controlled manipulation.

## Project Overview

This project combines:
- **Hardware**: Arduino-controlled robotic arm with 8 servo motors
- **Software**: Android mobile app for camera-based cube detection and Bluetooth control
- **Algorithms**: Kociemba two-phase algorithm for optimal cube solving

The system uses computer vision to analyze all 6 faces of a Rubik's cube, calculates the minimal solving sequence, and then executes it robotically.

## Getting started
Each folder in this repo is responsible for a different thing, with its own README.md that explains it further. I recommend doing them in the following order:
### Hardware
This is where you'll find hardware things that you will need, along with STL files ready for 3D printing and instructions for which parts to 3D print and how to assemble them. You'll also find Fusion 360 files of the same STL files in case you want to tweak them yourself. At the end, you should have the robot ready to be programmed.

### Arduino
When you've built the robot, it is time to flash the arduino with the firmware and configure it. This directory has a README that will show how this is done, and explain the API interface of the robot and how to talk with it.

### Cube Solver
Since the entire robot with the arduino is just sitting and waiting for commands, this Cube Solver is an android app that I developed as an example for how to control the robot. It combines computer vision with the phone's camera to scan the cube with Kociemba's algorithm to find the optimal solution for the cube, and streams the solution to the robot, which executes it. 


## Workflow: Solving a Cube

1. **User starts app** → Connects to Arduino via Bluetooth (HC-06)
2. **Scan Phase**:
   - Opens Scan tab
   - Robot rotates to expose each face
   - User taps "Scan Face" for each of 6 faces
   - App captures and processes images
3. **Processing**:
   - Converts color detection to Kociemba format
   - Sends to two-phase solver (embedded `.jar`)
   - Returns optimal move sequence
4. **Solving**:
   - User sets speed
   - Taps "Solve"
   - App sends `MOVE` command with sequence
   - Arduino executes servo movements
   - Phone listens for IDLE/BUSY status updates
5. **Done**: Cube is solved, robot returns to rest position

## Future Improvements

- Make the scanning computer vision part better
- The SEQ command and the API in general takes up the entire RAM cuz I was lazy

---

**Created**: January 2026  
**Author**: Yonan  
**License**: Do whatever the fuck you want as long as you don't sell any of this or profit from it in any way
