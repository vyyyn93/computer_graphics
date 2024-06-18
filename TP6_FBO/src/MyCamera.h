#ifndef MyCamera_H
#define MyCamera_H
#include "My3DStruct.h"

class MyCamera
{
  private:
    vec3 camera_position; 
    vec3 camera_target;
    vec3 camera_up;
    bool pivot_mode;

  public:
    MyCamera(){
      this->camera_position = {0.0f, 0.0f, 0.0f};
      this->camera_target = {0.0f, 0.0f, 0.0f};
      this->camera_up = {0.0f, 1.0f, 0.0f};
      this->pivot_mode = true;
    } 

    void calculateCameraPosition(float distance, float azimuth, float elevation) {
      vec3 position;
      position.y = distance * sin(elevation);
      float radius = distance * cos(elevation);
      position.x = radius * cos(azimuth);
      position.z = radius * sin(azimuth);
      this->setCameraPosition(position);
    } 
    
    void setCameraPosition(vec3 new_position) {
      this->camera_position = new_position;
    }

    void setCameraTarget(vec3 new_position) {
      this->camera_target = new_position;
    }

    vec3 getCameraPosition() const{
      return this->camera_position;
    }
    
    vec3 getCameraTarget() const{
      return this->camera_target;
    }
    
    vec3 getCameraUp() const{
      return this->camera_up;
    }

    bool isPivotMode(){
      return this->pivot_mode;
    }

    void changeMode(){
      this->pivot_mode = ~this->pivot_mode;
    }
};

#endif