#include "serial_handling.h"

extern shared_vars shared;
const uint16_t buf_size = 256;
char* buffer = (char *) malloc(bufsize);
uint16_t buf_len = 0;

void request(const lon_lat_32& start, const lon_lat_32& end) {
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
uint8_t process(char input) {
  while(1) {
    if(Serial.available()) {
      char in_char = Serial.read();
      if(in_char == '\n' || in_char == '\r') {
        return 1;
      }
      else {
        if(buf_len < buf_size - 1) {
          buffer[buf_len] = in_char;
          buf_len++;
          buffer[buff_len] = 0;
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
  Serial.setTimeout(10000);
  if(!Serial.available()) {
    request(start, end);
    return 0;
  }
  else {
    char input = Serial.read();
    if(input == 'N') {
      uint8_t n = process();
      if(n == 0) {
        return 0;;
      }
      else {
        for(int i = 1; i < buf_len; i++) {
          int temp = buffer[i] - '0';
          temp = temp * 10 * (buf_len - i - 1);
          shared.num_waypoints += temp;
        }
        Serial.println("A");
      }


    }
    for(int i = 0; i < shared.num_waypoints; i++) {
      if(Serial.available()) {
        input = Serial.read();
        if(input == 'W') {
          lon_lat_32 point;
          uint8_t n = process();
          if(n == 0) {
            return 0;
          }
          else {
            int i = 1;
            while(buffer[i] != ' ') {
              i++;
            }
            for(int j = i - 1; j >= 1; j--) {
              int temp = buffer[i - j] - '0';
              temp = temp * 10 * j;
              point.lat += temp;
              i++;
            }
            i++;
            for(int j = i; j < buf_len; j++) {
              int temp = buffer[j] - '0';
              temp = temp * 10 * (buf_len - j - 1);
              shared.num_waypoints += temp;
            }
            Serial.println("A");
          }

        }
      }
      else {
        return 0;
      }
    }
    if(Serial.available()) {
      input = Serial.read();
      if(input == 'E') {
        Serial.println("A");
        return 1;
      }
    }
    else {
      return 0;
    }
    
    
  }

  
}
