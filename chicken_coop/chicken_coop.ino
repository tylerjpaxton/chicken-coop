#include <Time.h>
#include <TimeLord.h>
#include <Stepper.h>

//Setup stepper moter
const int steps_per_revolution = 200;  // steps per revolution
int stepper_speed = 60; //stepper speed
Stepper door_stepper(steps_per_revolution, 8, 9, 10, 11); // initialize the stepper library on pins 8 through 11:

//Set TimeLord parameters
float const latitude = 42.784167; // latitude (south values negative)
float const longtitude = -83.245556; //longitude (west values negative)
int timezone = -5;

//set open and shut time paramaters
int open_buffer = -60; //time relative to sunrise for doors to open
int close_buffer = 60; //time relative to sunrset for doors to close
int door_change_requests_required = 10; //how many consecutive loops before door open/close command is sent
int door_change_requests = 0; //how many consecutive door open/close decision have been made

//Set door control parameters
boolean door_open = true; //current door status
int door_open_revolutions = 0;
int door_close_revolutions = 0;

//Set general parameters
boolean debug_on = true; //debug mode for printing to serial port
long delay_time = 60000; //millisecond delay between loops

//Set pins


/////////////////
// main setup  //
// //////////////
void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);    

  //set current time
  setTime(22,16,00,5,8,2016);

  //stepper speed set
  door_stepper.setSpeed(stepper_speed);

  //Print current status
  Serial.print("Current time: ");
  Serial.print(hour());
  Serial.print(":");
  Serial.println(minute());
  Serial.print("Debug: ");
  if(debug_on) Serial.println("On");
  if(!debug_on) Serial.println("Off");
  Serial.print("Door: ");
  if(door_open) Serial.println("Open");
  if(!door_open) Serial.println("Closed");
  
}

/////////////////
// main loop   //
// //////////////
void loop(void) {

  
  //store current time to minutes
  int day_minute = hour()*60 + minute();
  if(debug_on){
    Serial.print("Current minute: ");
    Serial.println(day_minute);
  }

  //add buffer to sunrise/sunset times and save
  int open_time = get_sun_minute("Sunrise") + open_buffer;
  int close_time = get_sun_minute("Sunset") + close_buffer;

  //degug open and close times
  if(debug_on){
    Serial.print("Open time: ");
    Serial.println(open_time);
    Serial.print("Close time: ");
    Serial.println(close_time);
  }

  
  //check current minutes against open close door times to decide if door needs to be toggled
  if(day_minute > close_time && door_open) {

    //increment door checks
    door_change_requests++;

    //debug logging
    if(debug_on){
      Serial.print("Door checks: ");
      Serial.print(door_change_requests);
      Serial.print("/");
      Serial.println(door_change_requests_required);
    }

    //send door command if decision has been consistent
    if(door_change_requests >= door_change_requests_required) {
      if (door_open) set_door_status(false);
      door_change_requests = 0;
    }
  }
   
  else if(day_minute > open_time && !door_open) {

    //increment successful door check
    door_change_requests++;

    //debug logging
    if(debug_on){
      Serial.print("Door checks: ");
      Serial.print(door_change_requests);
      Serial.print("/");
      Serial.println(door_change_requests_required);
    }

    //send door command if decision has been consistent
    if(door_change_requests >= door_change_requests_required) {
      if (!door_open) set_door_status(true);
      door_change_requests = 0;
    }
  }
  else {
    //debug logging
    if(debug_on){
      Serial.print("Door change requests: ");
      Serial.print(door_change_requests);
      Serial.print("/");
      Serial.println(door_change_requests_required);
    }
  }


  
  delay(delay_time);
}


///////////////////////
// get sun munites   //
// ////////////////////

//takes argument for sunrise or sunset and returns time in minutes
int get_sun_minute(String sun_type){
  TimeLord t; 
  t.TimeZone(-4 * 60); // tell TimeLord what timezone your RTC is synchronized to. You can ignore DST
                       // as long as the RTC never changes back and forth between DST and non-DST
  t.Position(latitude, longtitude); // tell TimeLord where in the world we are
  //set up timeloard for today
  byte today_sun[]  = {0, 0, 0, day(), month(), year()};

  if(sun_type == "Sunrise"){
    t.SunRise(today_sun);
  }
  else if(sun_type == "Sunset"){
    t.SunSet(today_sun);
  }
  else{
     if(debug_on) Serial.println("ERROR: Invalid Sun Type");
    return 0;
  }
  
    //output sunrise  info
  if(debug_on){
    Serial.print(sun_type);
    Serial.print(": ");
    Serial.print(today_sun[2]);
    Serial.print(":");
    Serial.println(today_sun[1]);
  }
  int sun_minute = today_sun[2]*60 + today_sun[1];

  return sun_minute;
  
}



///////////////////////////
// open and close door   //
// ////////////////////////
void set_door_status(boolean requested_door_status) {
  
  //check to see if already in requested status
  if(requested_door_status != door_open){

      //open or shut the door
      if(!door_open) {
        //turn motor to open
        if(debug_on){Serial.println("Opening door....");}
        for (int i=0; i <= door_open_revolutions; i++){
          // door_stepper.step(stepsPerRevolution);
        }   
        delay(10000); //simulate time for door opening
        door_open = true;
      }
      else if(door_open) {
        //turn motor to close
        if(debug_on){Serial.println("Closing door....");}
        for (int i=0; i <= door_close_revolutions; i++){
          // door_stepper.step(-stepsPerRevolution);
        }  
        delay(10000); //simulate time for door closing
        door_open = false;
      }
      
      if(debug_on){
        if(door_open) Serial.println("Door open.");
        else if(!door_open) Serial.println("Door closed.");
      }
  }
  else{
    if(debug_on){
      Serial.print("Door already ");
      if(door_open) Serial.println("open!");
      if(!door_open) Serial.println("closed!");
    }
  }


}

