#The SignalVest™️

**Introduction**

The premise of this project was to mimic the signaling system of a vehicle and thereby allow bikers to convey their intentions to other vehicles on the road. In 2020 alone, over 130 000 bicyclists were injured in road accidents. With our project, we aimed to reduce road-side miscommunication by providing a more reliable method of communication in all lighting conditions. There are two main features on the vest that we have achieved, namely, turn signaling and brake signaling. The turn signal turns on at the push of a button and automatically shut off after completing the turn. Similarly, the brake signal automatically turns on when the vest detects deceleration and turns off when it does not.  

**Technical Details** 

The Signal Vest offers several features including turn signaling, automatic brake lights and automatic turn light cancellation. To implement the features, the project makes use of an Arduino microcontroller working in conjunction with LEDs, an accelerometer (the MPU-6050), and push buttons. 

The Arduino serves as the main control unit for all the components. All the processes run on a multi-threaded loop that allows data to be processed and for components to work concurrently. The processes include reading button input, checking acceleration, measuring heading, and animating the LEDs, each of which runs on its own timer. 

The accelerometer is responsible for reading translational and angular acceleration values. The program logic then uses the translational acceleration values to decide when to turn on the braking lights, while the angular acceleration helps compensate for any error margins. Additionally, by integrating the angular acceleration, the program is able to determine the heading and consequently it can automatically turn off any turn signals.

The push buttons are mainly responsible for controlling the turn signals. Since the program checks for button presses every 50ms, to avoid registering a click multiple times, we maintain an additional boolean to check for when the button is released.

The LEDs are used simply to display the different signals. However, in order to implement the signaling animations with the other functions we had to get around the issue of animating the lights concurrently. That is to say, the lights could not be animated inside its own for-loop, otherwise, it would halt the other processes. Ultimately, we adapted a state-based animation system that would keep track of the animation progress while other processes continued to run.

From a hardware perspective, all the components were brought together with a breadboard and wire connections. To provide a structure for the components, we created an electronics box out of foam. This central structure allowed us to keep all the components together and wire them together in a straight-forward manner. To allow for easier integration, we added a hinge that allowed us to open the box and adjust components whenever necessary.
