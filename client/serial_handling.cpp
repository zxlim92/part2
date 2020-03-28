#include "serial_handling.h"

extern shared_vars shared;
const uint16_t buf_size = 256;
String buffer = "";
uint16_t buff_len = 0;


bool wait_timeout(uint8_t nbytes, long timeout) {
  unsigned long deadline = millis() + timeout;
  while(Serial.available() < nbytes && (timeout < 0 || millis() < deadline)) {
    delay(1);
  }
  return Serial.available() >= nbytes;
}
void request(const lon_lat_32& start, const lon_lat_32& end) {
  Serial.end();
  Serial.begin(9600);
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
      uint8_t n = process();
      if(n == 0) {
        return 0;
      }
      else {
        String temp = buffer.substring(1,buff_len);
        shared.num_waypoints = temp.toInt();
        Serial.println("A");
      }


    }
    for(int i = 0; i < shared.num_waypoints; i++) {
      if(wait_timeout(1,10000)) {
        input = Serial.read();
        if(input == 'W') {
          lon_lat_32 point;
          uint8_t n = process();
          if(n == 0) {
            return 0;
          }
          else {
            int j = 1;
            while(buffer.substring(j, j+1) != " ") {
              j++;
            }
            String temp = buffer.substring(1, j);
            point.lat = (int32_t)temp.toInt();
            temp = buffer.substring(j + 1, buff_len);
            point.lon = (int32_t)temp.toInt();
            shared.waypoints[i] = point;
            Serial.println("A"); 
          }

        }
      }
      else {
        return 0;
      }
    }
    if(wait_timeout(1,10000)) {
      input = Serial.read();
      if(input == 'E') {
        return 1;
      }
    }
    else {
      return 0;
    }
    
  }
  else {
    return 0;
    
  }

  
}
