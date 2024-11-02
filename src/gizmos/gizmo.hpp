#ifndef GIZMO_HPP
#define GIZMO_HPP

#include <string>

#include <sol/sol.hpp>

class Gizmo {
 public:
  Gizmo() {
    name = "";
    aux = x = y = rot = 0.0f;
  }
  void Draw() {};
  

 public:
  float aux;
  float rot;
  float x, y;
  std::string name;
};

#endif // GIZMO_HPP
