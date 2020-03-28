#include "draw_route.h"
#include "map_drawing.h"

extern shared_vars shared;

void draw_route() {
  // implement this!
  for (int i = 0; i < shared.num_waypoints - 1; i++) {
      int8_t num = shared.map_number;  // current zoom elevel

      /*Get two points*/
      lon_lat_32 p1 = shared.waypoints[i];
      lon_lat_32 p2 = shared.waypoints[i + 1];

      /*Configure the points*/
      xy_pos p1_loc = xy_pos(longitude_to_x(num, p1.lon),
        latitude_to_y(num, p1.lat));
      p1_loc.x -= shared.map_coords.x;
      p1_loc.y -= shared.map_coords.y;

      xy_pos p2_loc = xy_pos(longitude_to_x(num, p2.lon),
        latitude_to_y(num, p2.lat));
      p2_loc.x -= shared.map_coords.x;
      p2_loc.y -= shared.map_coords.y;


      // draw the line from one point to the other
      shared.tft->drawLine(p1_loc.x, p1_loc.y, p2_loc.x,
                            p2_loc.y, ILI9341_BLUE);
  }
}
