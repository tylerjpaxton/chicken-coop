#include <Time.h>
#include <TimeLord.h>

//Set TimeLord parameters
float const latitude = 42.784167; // what is our longitude (west values negative) and latitude (south values negative)
float const longtitude = -83.245556;
int timezone = -5;
TimeLord t; 

//Set general parameters
int open_buffer = 60;
int close_buffer = 60;
int door_checks_required = 10;
int door_checks_passed = 0;
boolean door_open = true;
boolean debug_on = true;
long delay_time = 60000;

//Set pins
int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the analog resistor divider

#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

 
void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);   
  // setTime(t);   //not sure what this is for??? 
  t.TimeZone(-4 * 60); // tell TimeLord what timezone your RTC is synchronized to. You can ignore DST
  // as long as the RTC never changes back and forth between DST and non-DST
  t.Position(latitude, longtitude); // tell TimeLord where in the world we are

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



 
void loop(void) {
  
  //set up timeloard for today
  byte today_sunrise[]  = {0, 0, 0, day(), month(), year()};
  byte today_sunset[]  = {0, 0, 0, day(), month(), year()};
  t.SunRise(today_sunrise);
  t.SunSet(today_sunset);
  

  //output sunrise and sunset info
  if(debug_on){
    Serial.print("Sunrise: ");
    Serial.print(today_sunrise[2]);
    Serial.print(":");
    Serial.println(today_sunrise[1]);
    Serial.print("Sunset: ");
    Serial.print(today_sunset[2]);
    Serial.print(":");
    Serial.println(today_sunset[1]);
  }
  
  //store current time to minutes
  int day_minute = hour()*60 + minute();
  if(debug_on){
    Serial.print("Current minute: ");
    Serial.println(day_minute);
  }
  
  //convert sunrise on sunset to minutes and store as open/close times with buffer
  int open_time = today_sunrise[2]*60 + today_sunrise[1] - open_buffer;
  int close_time = today_sunset[2]*60 + today_sunset[1] + close_buffer;

  if(debug_on){
    Serial.print("Open time: ");
    Serial.println(open_time);
    Serial.print("Close time: ");
    Serial.println(close_time);
  }
  
  //check current minutes against open close door times
  if(day_minute > close_time ) {
    door_checks_passed++;
    if(debug_on){
      Serial.print("Door checks: ");
      Serial.print(door_checks_passed);
      Serial.print("/");
      Serial.println(door_checks_required);
    }
    if(door_checks_passed >= door_checks_required) {
      close_door();
      door_checks_passed = 0;
    }
  }
  else if(day_minute > open_time) {
    door_checks_passed++;
    if(debug_on){
      Serial.print("Door checks: ");
      Serial.print(door_checks_passed);
      Serial.print("/");
      Serial.println(door_checks_required);
    }
    if(door_checks_passed >= door_checks_required) {
      open_door();
      door_checks_passed = 0;
    }
  }


  /*
  photocellReading = analogRead(photocellPin);  
 
  Serial.print("Analog reading = ");
  Serial.print(photocellReading);     // the raw analog reading
 
  // We'll have a few threshholds, qualitatively determined
  if (photocellReading < 10) {
    Serial.println(" - Dark");
  } else if (photocellReading < 200) {
    Serial.println(" - Dim");
  } else if (photocellReading < 500) {
    Serial.println(" - Light");
  } else if (photocellReading < 800) {
    Serial.println(" - Bright");
  } else {
    Serial.println(" - Very bright");
  }
  */
  delay(delay_time);
}

void open_door(void) {
  //
  if(!door_open) {
    //turn motor to open
    if(debug_on){Serial.println("Openeing door....");}
    delay(10000); //simulate time for door opening
  }
  //set door status
  door_open = true;
  if(debug_on){Serial.println("Door open.");}
}

void close_door(void) {
  //
  if(door_open) {
    //turn motor to close
    if(debug_on){Serial.println("Closing door....");}
    delay(10000); //simulate time for door closing
  }

  //set door status
  door_open = false;
  if(debug_on){Serial.println("Door closed.");}
}


