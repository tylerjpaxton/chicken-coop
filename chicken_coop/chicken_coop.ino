#include <Time.h>
#include <TimeLord.h>

//Set TimeLord parameters
float const latitude = 42.784167; // latitude (south values negative)
float const longtitude = -83.245556; //longitude (west values negative)
int timezone = -5;


//Set general parameters
int open_buffer = -60; //time relative to sunrise for doors to open
int close_buffer = 60; //time relative to sunrset for doors to clsoe
int door_checks_required = 10; //how many consecutive loops before door open/close command is sent
int door_checks_passed = 0; //how many consecutive door open/close decision have been made
boolean door_open = false; //current door status
boolean debug_on = true; //debug mode for printing to serial port
long delay_time = 5000; //millisecond delay between loops

//Set pins


/////////////////
// main setup  //
// //////////////
void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);    


  //set current time
  setTime(11,26,00,5,8,2016);

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
  if(day_minute > close_time ) {

    //increment door checks
    door_checks_passed++;

    //debug logging
    if(debug_on){
      Serial.print("Door checks: ");
      Serial.print(door_checks_passed);
      Serial.print("/");
      Serial.println(door_checks_required);
    }

    //send door command if decision has been consistent
    if(door_checks_passed >= door_checks_required) {
      set_door_status(false);
      door_checks_passed = 0;
    }
  }
   
  else if(day_minute > open_time) {

    //increment successful door check
    door_checks_passed++;

    //debug logging
    if(debug_on){
      Serial.print("Door checks: ");
      Serial.print(door_checks_passed);
      Serial.print("/");
      Serial.println(door_checks_required);
    }

    //send door command if decision has been consistent
    if(door_checks_passed >= door_checks_required) {
      set_door_status(true);
      door_checks_passed = 0;
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

  if(sun_type = "Sunrise"){
    t.SunRise(today_sun);
  }
  else if(sun_type = "Sunrise"){
    t.SunSet(today_sun);
  }
  else{
     if(debug_on) Serial.println("ERROR: Invalid Sun Type");
    return 0;
  }
  
    //output sunrise  info
  if(debug_on){
    Serial.print("sun_type");
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
        if(debug_on){Serial.println("Openeing door....");}
        delay(10000); //simulate time for door opening
        door_open = true;
      }
      else if(door_open) {
        //turn motor to close
        if(debug_on){Serial.println("Closing door....");}
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

