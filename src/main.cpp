#include <Arduino.h>
#include "./neotimer.h"
#include <esp_timer.h>
#include "encoder.h"
#include <SerialConfigCommand.h>

Neotimer print_timer = Neotimer(1000);
Neotimer update_timer = Neotimer(100);

SerialConfigCommand scc; //Define an instance of SerialConfigCommand

#include <driver/rmt.h>
#include <driver/periph_ctrl.h>
#define SUPPORT_ESP32_RMT
#include "FastAccelStepper.h"
FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

//  Configuration

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

#define dirPinStepper 12
#define enablePinStepper 26
#define stepPinStepper 13
int stepper_speed = 150000;





void processMotion()    //this is the interrupt routine for the floating point division algorithm
  {
    
    //input_counter++;                                                           // increments a counter for the number of spindle pulses received


    // For lathe ELS

    //calculated_stepper_pulses=round(factor*input_counter);                     // figure steps to go
    //calculated_stepper_pulses = factor * input_counter;

    // For Hob
    calculated_stepper_pulses = factor * encoder.pulse_counter;

    // Move
    if((calculated_stepper_pulses>stepper->getCurrentPosition())) 
       {
        stepper->moveTo(calculated_stepper_pulses);
      }
   }



// time a function to see how long it takes to run
void measure_important_function(void) {
    const unsigned MEASUREMENTS = 150;
    uint64_t start = esp_timer_get_time();

    for (int retries = 0; retries < MEASUREMENTS; retries++) {
        processMotion(); // This is the thing you need to measure
    }

    uint64_t end = esp_timer_get_time();

    printf("%u iterations took %llu tics (%llu tics per invocation)\n",
           MEASUREMENTS, (end - start), (end - start)/MEASUREMENTS);
    printf("\tfactor: %f calculated_stepper_pulses: %f input: %llu\n",factor,calculated_stepper_pulses,input_counter);
    
}

void print_status(){
  if(print_timer.repeat()){
    //measure_important_function();
    //Serial.println(" done");
    printf("factor: %f calculated_stepper_pulses: %f ",factor,calculated_stepper_pulses);
    printf(" encoder position: %llu prev: %llu dir: %d quad: %d ",encoder.pulse_counter,encoder.prev_pulse_counter,encoder.dir,encoder.quadrature);
    printf(" stepper position: %d a %d b %d \n",stepper->getCurrentPosition(),digitalRead(16),digitalRead(17));
    printf("\n\t lag: %llu teeth: %d accel: %d \n",lag, gear_tooth_number, accel);
  }
}

void check_lag(){
  //lag = stepper->getCurrentPosition() - calculated_stepper_pulses;
  lag = calculated_stepper_pulses - stepper->getCurrentPosition();
  if (lag > (factor * 3)){
    digitalWrite(led,HIGH);
  }
}

void response(){
  printf("Command ");
  if(scc.getCmdS() == "teeth" && scc.hasValue()){
    gear_tooth_number = (int)scc.getValueInt();
    printf(" got %d teeth\n",gear_tooth_number);
  }else if(scc.getCmdS() == "speed" && scc.hasValue()){
    stepper_speed = scc.getValueInt(); 
    stepper->setSpeedInHz(stepper_speed);
    printf("got %d speed",stepper_speed);
  }else if(scc.getCmdS() == "accel" && scc.hasValue()){
    accel = scc.getValueInt();
    stepper->setAcceleration(accel);
    stepper->applySpeedAcceleration();
    printf("got accel %d \n",accel );

  }else{
    Serial.print("Invalid command received! ");
    Serial.println(scc.getCmdS());
  }
  factor = spindle_steps_per_rev*1.0/(gear_tooth_number*hob_encoder_resolution);
}

void setup() {
  Serial.begin(115200);

  Serial.println("Goal 1: benchmark how long it takes to handle interrupt");
  Serial.println("Goal 2: figure out how to monitor CPU usage better");

  // for benchmark
  esp_err_t e = esp_timer_init();
  last_time = esp_timer_get_time();


  // Setup Encoder

  init_encoder();
  engine.init();
  //startVenc();

  // Setup stepper
  stepper = engine.stepperConnectToPin(stepPinStepper);
  if (stepper) {
    stepper->setDirectionPin(dirPinStepper);
    stepper->setEnablePin(enablePinStepper);
    //stepper->setAutoEnable(true);

    // If auto enable/disable need delays, just add (one or both):
    stepper->setDelayToEnable(15);

    stepper->setSpeedInHz(stepper_speed);  // the parameter is us/step !!!
    stepper->setAcceleration(accel);
    stepper->applySpeedAcceleration();
    scc.set(200, response); 
  }


  
}

void loop() {
  print_status();
  check_lag();

  if(update_timer.repeat()){
    scc.update();
  }
  
}
