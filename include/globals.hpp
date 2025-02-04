#pragma once

#include "lemlib/asset.hpp"
#include "main.h"
#include "pros/adi.hpp"
#include "pros/motors.hpp"
#include "lemlib/api.hpp"
// motors
extern pros::Motor ladybrown;
extern int lb_state;
extern int intake_state;
extern pros::MotorGroup intake;
// pistons
extern pros::adi::DigitalOut mogo;
extern pros::adi::DigitalOut intake_piston;
extern pros::adi::DigitalOut doinker;
// sensors
extern pros::Rotation lb_sensor;
extern pros::IMU imu;
// user controller
extern pros::Controller controller;
// pid controller
extern lemlib::ControllerSettings linearController;
extern lemlib::ControllerSettings angularController;
extern lemlib::Chassis chassis;
extern lemlib::Drivetrain drivetrain;
extern lemlib::OdomSensors sensors;