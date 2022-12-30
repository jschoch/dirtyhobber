# Bare Bones ESP32 Gear Hobbing Controller

This quick and dirty controller is designed for an esp32 devkit v4 board (az_delivery) using an optical encoder and an external stepper or servo controller.

This requires minium of 4 pins for step, dir, encoder_a, encoder_b signals.  This is the simplest and most basic controller and it may have bugs.  Use at your own risk.  PR's and questions (via issues) welcome.


![16_TPI_tap_makes_aluminum_gears](https://user-images.githubusercontent.com/20271/210111404-7da5b2fa-a23e-47b4-830e-201d5fcad23b.gif)

# Configuration: 

The `main.cpp` file contains most of the configuration
The items below need to be updated in the file.
```c++
int motor_steps = 200;
int microstepping = 8;
int reduction = 40;  // This is the indexer gear reduction 
int hob_encoder_resolution = 2400;
bool dir = 1; // this controls the motor direction
int led = 25; // the LED pin 

#define dirPinStepper 12
#define enablePinStepper 26
#define stepPinStepper 13
int stepper_speed = 150000;
```

Encoder.cpp requires the A and B encoder pins as well as the pulses per revolution.  PPR is the 3rd argument of the initializer for the Encoder class.

```c++

int EA = 17;
int EB = 16;

Encoder encoder = Encoder(EA, EB, 600);
```


# Running:

To configure the tooth count connect to serial and paste `teeth=N` where N is the number of teeth for the gear.



Board

Schematic
![image](https://user-images.githubusercontent.com/20271/210110375-24cee82a-b212-4c8b-a436-67d4a0aba020.png)

PCB

![image](https://user-images.githubusercontent.com/20271/210110407-187ac084-289c-4197-9a14-220b1e4ac64f.png)
