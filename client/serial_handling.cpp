#include "serial_handling.h"

extern shared_vars shared;
const uint16_t buf_size = 256;

// setting up serial buffer
String buffer = "";
uint16_t buff_len = 0;


// this function is modelled exactly the same with wait_on_serial3() from cmput274
bool wait_timeout(uint8_t nbytes, long timeout) {
  unsigned long deadline = millis() + timeout;
  while(Serial.available() < nbytes && (timeout < 0 || millis() < deadline)) {
    delay(1);
  }
  return Serial.available() >= nbytes;
}

// sending request to Host for routing
void request(const lon_lat_32& start, const lon_lat_32& end) {
  // end the serial to clear out the buffer
  Serial.end();
  Serial.begin(9600);
  // start the request
  Serial.print("R ");
  Serial.print(start.lat);
  Serial.print(" ");
  Serial.print(start.lon);
  Serial.print(" ");
  Serial.print(end.lat);
  Serial.print(" ");
  Serial.print(end.lon);
  Serial.println();
}


// processing input by reading all the bytes and add them to the string, modelled similar
// to example from eclass, 
// return 0 if fails to process, 1 if succeed
uint8_t process() {
  buffer = "";
  buff_len = 0;
  while(1) {
    if(wait_timeout(1,1000)) {
      char in_char = Serial.read();
      if(in_char == '\n' || in_char == '\r') {
        return 1;
      }
      else {
        if(buff_len < buf_size - 1) {
          buffer += in_char;
          buff_len++;
        }
      }
    }
    else {
      return 0;
    }
  }
  return 1;

}
uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
  // Currently this does not communicate over the serial port.
  // It just stores a path length of 0. You should make it communicate with
  // the server over Serial using the protocol in the assignment description.

  // TODO: implement the communication protocol from the assignment
  shared.num_waypoints = 0;
  // for now, nothing is stored
  // 1 indicates a successful exchange, of course you should only output 1
  // in your final solution if the exchange was indeed successful
  // (otherwise, return 0 if the communication failed)
  request(start, end);
  if(wait_timeout(1,10000)) {
    char input = Serial.read();
    if(input == 'N') {
      status_message("Receiving...");
      uint8_t n = process();
      if(n == 0) {
        return 0;
      }
      else {
        // extract the number of waypoints in buffer
        String temp = buffer.substring(1,buff_len);
        shared.num_waypoints = temp.toInt();
        Serial.println("A");
      }
    }
    else {
      return 0;
    }
    // processing the location of waypoints
    for(int i = 0; i < shared.num_waypoints; i++) {
      if(wait_timeout(1,10000)) {
        input = Serial.read();
        if(input == 'W') {
          lon_lat_32 point;
          uint8_t n = process();
          if(n == 0) {
            // things went south
            return 0;
          }
          else {
            int j = 1;
            while(buffer.substring(j, j+1) != " ") {
              j++;
            }
            // extract lat and lon from the buffer
            String temp = buffer.substring(1, j);
            point.lat = (int32_t)temp.toInt();
            temp = buffer.substring(j + 1, buff_len);
            point.lon = (int32_t)temp.toInt();
            shared.waypoints[i] = point;
            Serial.println("A"); 
          }
        }
        else {
          return 0;
        }
      }
      else {
        // things went south
        return 0;
      }
    }
    if(wait_timeout(1,10000)) {
      input = Serial.read();
      // if it does not get E from Host
      if(input != 'E') return 0;
    }
    else {
      // things went south
      return 0;
    }
  }
  else {
    // things went south
    return 0;
    
  }
  return 1;
  
}
