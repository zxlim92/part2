#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <list>
#include "wdigraph.h"
#include "dijkstra.h"
#include "serialport.h"

SerialPort Serial("/dev/ttyACM0");
struct Point {
    long long lat, lon;
};

// returns the manhattan distance between two points
long long manhattan(const Point& pt1, const Point& pt2) {
  long long dLat = pt1.lat - pt2.lat, dLon = pt1.lon - pt2.lon;
  return abs(dLat) + abs(dLon);
}

// finds the id of the point that is closest to the given point "pt"
int findClosest(const Point& pt, const unordered_map<int, Point>& points) {
  pair<int, Point> best = *points.begin();

  for (const auto& check : points) {
    if (manhattan(pt, check.second) < manhattan(pt, best.second)) {
      best = check;
    }
  }
  return best.first;
}

// read the graph from the file that has the same format as the "Edmonton graph" file
void readGraph(const string& filename, WDigraph& g, unordered_map<int, Point>& points) {
  ifstream fin(filename);
  string line;

  while (getline(fin, line)) {
    // split the string around the commas, there will be 4 substrings either way
    string p[4];
    int at = 0;
    for (auto c : line) {
      if (c == ',') {
        // start new string
        ++at;
      }
      else {
        // append character to the string we are building
        p[at] += c;
      }
    }

    if (at != 3) {
      // empty line
      break;
    }

    if (p[0] == "V") {
      // new Point
      int id = stoi(p[1]);
      assert(id == stoll(p[1])); // sanity check: asserts if some id is not 32-bit
      points[id].lat = static_cast<long long>(stod(p[2])*100000);
      points[id].lon = static_cast<long long>(stod(p[3])*100000);
      g.addVertex(id);
    }
    else {
      // new directed edge
      int u = stoi(p[1]), v = stoi(p[2]);
      g.addEdge(u, v, manhattan(points[u], points[v]));
    }
  }
}

// keep in mind that in part 1, the program should only handle 1 request
// in part 2, you need to listen for a new request the moment you are done
// handling one request
int main() {
  WDigraph graph;
  unordered_map<int, Point> points;

  // build the graph
  readGraph("edmonton-roads-2.0.1.txt", graph, points);

  // read a request
  while(1) {
    Point sPoint, ePoint;
    string input = Serial.readline(); // read input in serial 
    if(input[0] == 'R') { // get start and end coordinates needed after R
      int i = 2;
      while(input.substr(i,1) != " ") {
        i++;
      }
      string temp = input.substr(2, i - 2);
      sPoint.lat = stoll(temp);
      int j = i+1;
      while(input.substr(j,1) != " ") {
        j++;
      }
      temp = input.substr(i+1,j - i - 1);
      sPoint.lon = stoll(temp);
      int k = j + 1;
      while(input.substr(k,1) != " ") {
        k++;
      }
      temp = input.substr(j+1,k - j - 1);
      ePoint.lat = stoll(temp);
      temp = input.substr(k+1,temp.size() - k -1);
      ePoint.lon = stoll(temp);
      int start = findClosest(sPoint, points), end = findClosest(ePoint, points); // get closest way points to the start longitude and lattitude

      // run dijkstra's, this is the unoptimized version that does not stop
      // when the end is reached but it is still fast enough
      unordered_map<int, PIL> tree;
      dijkstra(graph, start, tree);

      // no path
      if (tree.find(end) == tree.end()) {
          Serial.writeline("N 0\n");
          cout << "N 0" << endl;
      }
      else {
        // read off the path by stepping back through the search tree
        list<int> path;
        while (end != start) {
          path.push_front(end);
          end = tree[end].first;
        }
        path.push_front(start);

        // output the path
        
        string size = to_string(path.size()); // convert to string 
        temp = "";
        temp.append("N "); 
        temp.append(size);
        temp.append("\n");
        Serial.writeline(temp);
        cout << "N " << path.size() << endl;
        for (int v : path) {
          input = Serial.readline();
          if(input[0] == 'A') {
            temp = ""; // string variable to build up the string output before writing it on the serial 
            temp.append("W ");
            string lat = to_string(points[v].lat);
            string lon = to_string(points[v].lon);
            temp.append(lat);
            temp.append(" ");
            temp.append(lon);
            temp.append("\n");
            Serial.writeline(temp);
            cout << "W " << points[v].lat << " " << points[v].lon << endl;
          }
          
        }
        input = Serial.readline();
        if(input[0] == 'A') { // end of waypoints
          Serial.writeline("E\n");
          cout << "E" << endl;
        }
      }
    }
  }




  return 0;
}
