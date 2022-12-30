# Gear Hobbing Controller

This controller is designed for an esp32 devkit v4 board (az_delivery) using an optical encoder and an external stepper or servo controller.

This requires minium of 4 pins for step, dir, encoder_a, encoder_b signals.  This is the simplest and most basic controller and it may have bugs.  Use at your own risk.  PR's and questions (via issues) welcome.

# Configuration: 

The `main.cpp` file contains most of the configuration

```c++
int64_t input_counter = 0;
int motor_steps = 200;
int microstepping = 8;
int reduction = 40;
int spindle_steps_per_rev = motor_steps * reduction * microstepping;
int gear_tooth_number = 78;
int hob_encoder_resolution = 2400;
double factor = spindle_steps_per_rev*1.0/(gear_tooth_number*hob_encoder_resolution);     
int step_queue = 0;
bool dir = 1;
double calculated_stepper_pulses = 0;
int delivered_stepper_pulses = 0;
uint64_t last_time = 0;
uint64_t  this_time = 0;
uint64_t run_time = 0;
uint64_t run_times[10] = {};
uint64_t lag = 0;
int led = 25;
//int accel = 2000;
int accel = 1000000;
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
