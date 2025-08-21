#ifndef HARDWARE_PERIPHERAL_PID_H_
#define HARDWARE_PERIPHERAL_PID_H_


// PID control mode definitions
#define PID_MODE_MANUAL    0    // Manual mode
#define PID_MODE_AUTOMATIC 1    // Automatic mode

// PID control direction definitions
#define PID_DIRECT         0    // Direct control
#define PID_REVERSE        1    // Reverse control

// PID proportional term mode definitions
#define PID_P_ON_E         0    // Proportional term on error
#define PID_P_ON_M         1    // Proportional term on measurement

// PID controller structure
typedef struct {
  // PID parameters
  float kp;                  // Proportional coefficient
  float ki;                  // Integral coefficient
  float kd;                  // Derivative coefficient

  // Display parameters (original user input values)
  float dispKp;              // Display proportional coefficient
  float dispKi;              // Display integral coefficient
  float dispKd;              // Display derivative coefficient

  // Control parameters
  float setpoint;            // Setpoint value
  float input;               // Input value (feedback)
  float output;              // Output value
  float lastInput;           // Last input value
  float outputSum;           // Integral term accumulation

  // Output limits
  float outMin;              // Output minimum limit
  float outMax;              // Output maximum limit

  // Time parameters
  float sampleTime;          // Sample time (seconds)
  unsigned long lastTime;    // Last calculation time

  // Control mode
  int mode;                  // Control mode: manual/automatic
  int controllerDirection;   // Control direction: direct/reverse
  int pOn;                   // Proportional term mode: error/measurement
  int pOnE;                  // Proportional term on error flag

  // Anti-windup
  int inAuto;                // Automatic mode flag
  int justEvaluated;         // Just evaluated flag

} pid_controller_t;


// Basic PID functions
void pid_init(pid_controller_t *pid, float kp, float ki, float kd, float sampleTime);
float pid_compute(pid_controller_t *pid, float input, float setPoint);

void pid_set_tunings(pid_controller_t *pid, float kp, float ki, float kd);
void pid_set_tunings_with_mode(pid_controller_t *pid, float kp, float ki, float kd, int pOn);
void pid_set_output_limits(pid_controller_t *pid, float min, float max);
void pid_set_mode(pid_controller_t *pid, int mode);
void pid_set_controller_direction(pid_controller_t *pid, int direction);
void pid_set_sample_time(pid_controller_t *pid, float sampleTime);
void pid_reset(pid_controller_t *pid);

void pid_get_status(pid_controller_t *pid, float *error, float *integral, float *derivative);


#endif /* PID_H_ */
