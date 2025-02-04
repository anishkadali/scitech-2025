#include "main.h"
#include "lemlib/api.hpp"
#include "globals.hpp"
#include "auton.hpp"
/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	chassis.calibrate(); // calibrate sensors
	// set intake and doinker to true, since they are activated by default
	intake_piston.set_value(true);
	doinker.set_value(true);
    // print position to brain screen
    pros::Task screen_task([&]() {
        while (true) {
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            // delay to save resources
            pros::delay(20);
        }
    });
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	tuning_move();
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	pros::Controller controller(pros::E_CONTROLLER_MASTER);
	//intake buttons
	pros::controller_digital_e_t INTAKE_UP = pros::E_CONTROLLER_DIGITAL_L1;
	pros::controller_digital_e_t INTAKE_DOWN = pros::E_CONTROLLER_DIGITAL_A;
	//ladybrown buttons to change predefined positions
	pros::controller_digital_e_t LB_MACRO_UP = pros::E_CONTROLLER_DIGITAL_UP;
	pros::controller_digital_e_t LB_MACRO_DOWN = pros::E_CONTROLLER_DIGITAL_DOWN;
	//mogo button
	pros::controller_digital_e_t MOGO_TOGGLE = pros::E_CONTROLLER_DIGITAL_L2;
	int intake_state = 0;
	int mogo_state = 0;
	int lb_state = 0;
	int lb_direction = 0;
	//lady brown arm positions
	double LB_LOADING = 200;
	double LB_SCORING = 750;
	ladybrown.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	ladybrown.tare_position();
	while (true) {
		// chassis control (https://lemlib.readthedocs.io/en/stable/tutorials/3_driver_control.html)
		// get left y and right y positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
        // move the robot (different options like arcade can be enabled if you look at the link)
        chassis.tank(leftY, rightY);
		// chassis control over

		// intake control, toggle
		if (controller.get_digital_new_press(INTAKE_UP)){
			// flip intake state from what it was before
			intake_state = !intake_state;
		}
		// reverse intake, hold
		if (controller.get_digital(INTAKE_DOWN)){
			intake.move_voltage(-12000);
		}
		else{
			// moves 0 if intake state is disabled, moves 12000 if its enabled
			intake.move_voltage(12000 * intake_state);
		}		
		// intake control over

		// mogo control
		if (controller.get_digital_new_press(MOGO_TOGGLE)){
			mogo_state = !mogo_state;
			mogo.set_value(mogo_state);
		}
		// mogo control over
		// ladybrown control, uses two buttons to cycle through positions
		if (controller.get_digital_new_press(LB_MACRO_UP)){
			if (lb_state == 2){
				ladybrown.move_absolute(LB_SCORING + 100, 75);
			}
			if (lb_state == 1){
				ladybrown.move_absolute(LB_SCORING, 75);
			}
			if (lb_state == 0){
				ladybrown.move_absolute(LB_LOADING, 75);
			}
			// always increment the state until 2
			lb_state = std::min(++lb_state, 2);
		}
		if (controller.get_digital_new_press(LB_MACRO_DOWN)){
			if (lb_state == 2){
				ladybrown.move_absolute(LB_LOADING, 50);
			}
			if (lb_state == 1){
				ladybrown.move_absolute(0, 50);
			}
			if (lb_state == 0){
				// do nothing if ladybrown is already down
			}
			// always decrement the state until 0
			lb_state = std::max(--lb_state, 0);
		}
		// delay to prevent tasks from starving
		pros::delay(20);
	}
}