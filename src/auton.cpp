#include "auton.hpp"
#include "globals.hpp"
#include "main.h"
#include "pros/adi.h"
#include "pros/motors.hpp"
#include "pros/rtos.hpp"
#include "lemlib/api.hpp"

void tuning_turn(){
    // set chassis to 0,0 facing 0 degrees relative to field, should adjust if robot is starting elsewhere
    chassis.setPose(0, 0, 0);
    chassis.turnToHeading(90, 4000);
    chassis.moveToPoint(24, 0, 4000);
}
void tuning_move(){
    chassis.setPose(0, 0, 0);
    chassis.moveToPose(24, 24, 90, 4000);
}
void red_left(){

}
void red_right(){

}
void blue_left(){

}
void blue_right(){

}
void skills(){

}