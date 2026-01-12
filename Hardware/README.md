# Hardware
In this folder, you'll find STL files, Fusion 360 files, electronics and much more..

## What you'll need (aside from 3D printing)
A wooden base. Minimum dimensions of 240x240mm. But, in my case, I needed it a little longer for my breadboard, so I had 240x270mm. Make sure the width is 240mm and the length is at least 240mm, but it can be as long as you need. The depth should ideally be between 20-30mm since we will be screwing and nailing things on this base

Eight servo motors (SG90 or its metal equivalent that has the same dimensions). 

A breadboard with some jumper wires for circuitry.

Power adapter that is a steady 5 volts and at least 3 amps. Anything less will not work. 

An arduino, any arduino here will work, but I went with Arduino Nano since it is small and fits nicely on the breadboard.

HC-06 or HC-05 bluetooth module for communication between the phone and the robot.. oh, and, an android phone.

3mm wooden screws that are 15mm long (to screw things into the wooden frame) and extra small screws (the same type that comes with the servo).

## 3D printing, STL and Fusion 360
Let's look at how the final robot should look like:

![](../README_resources/structure.png)

We can see four actuators. Two of them are normal and two of them are mirrored. (the definition of mirrored and normal is just my own). The upper-right in the image above and the bottom left actuators is what I defined as normal. The other two is what I defined as mirrored. Depending on where you want the gear and the servo to end up, you might want either all mirrored, all normal or like I did, two mirrored and two normal. If you want 3 mirroed or 3 normals you aren't normal :)

### Actuators
Each actuator is built up of 5 parts (aside from the two servos):
1. Grabber - the claw that grabs the rubiks cube. Mirroring doesn't matter for this one.
2. Slider - This is what holds the grabber and its servo, its the part that slides back and fourth.
3. Channel top - The part that holds the slider in the channel that it slides on. 
4. Channel bot - The channel itself that the slider should slide across. Since it can be rotated 180 degrees, mirroring shoudn't matter for this too.
5. Gear - This is what is mounted onto the bottom servo, that grabs the slider and pushes it back and fourth, also no mirroring. 

I hope this image makes the parts more clear:

![](../README_resources/act.png)
