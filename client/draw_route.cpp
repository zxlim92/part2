#include "draw_route.h"
#include "map_drawing.h"

extern shared_vars shared;

void draw_route() {
  // implement this!
  if(shared.num_waypoints <= 1) {
    //there's nothing to show
    status_message("Nothing to show");
  }
  else {
    for (int i = 0; i < shared.num_waypoints - 1; i++) {
        // taken 2 ends of the line
        lon_lat_32 start = shared.waypoints[i];
        lon_lat_32 end = shared.waypoints[i + 1];
        // convert it to the screen
        int16_t start_x = (int16_t)(longitude_to_x(shared.map_number, start.lon) - shared.map_coords.x);
        int16_t start_y = (int16_t)(latitude_to_y(shared.map_number, start.lat) - shared.map_coords.y);
        int16_t end_x   = (int16_t)(longitude_to_x(shared.map_number, end.lon) - shared.map_coords.x);
        int16_t end_y   = (int16_t)(latitude_to_y(shared.map_number, end.lat) - shared.map_coords.y);
        shared.tft->drawLine(start_x, start_y, end_x, end_y, TFT_BLUE);
    }
  }
}
