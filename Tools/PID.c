#include "PID.h"
#include <math.h>

// Internal function declarations
static void pid_initialize(pid_controller_t *pid);


void pid_init(pid_controller_t *pid, float kp, float ki, float kd, float sampleTime) {
  pid->setpoint = 0.0f;
  pid->input = 0.0f;
  pid->output = 0.0f;
  pid->lastInput = 0.0f;
  pid->outputSum = 0.0f;

  pid->outMin = 0;
  pid->outMax = 255;

  pid->sampleTime = sampleTime;
  pid->lastTime = 0;

  pid->mode = PID_MODE_AUTOMATIC;
  pid->inAuto = 1;
  pid->justEvaluated = 0;

  // Initialize control direction and proportional term mode
  pid->controllerDirection = PID_DIRECT;
  pid->pOn = PID_P_ON_E;
  pid->pOnE = 1;

  pid_set_tunings(pid, kp, ki, kd);
}


float pid_compute(pid_controller_t *pid, float input, float setPoint) {
  if (!pid->inAuto) {
    return pid->output;  // 手动模式直接返回当前输出
  }
  
  // 更新输入值和设定值
  pid->input = input;
  pid->setpoint = setPoint;
  
  // 计算误差
  float error = setPoint - input;
  
  // 计算微分项（输入微分，避免设定值变化引起的微分冲击）
  float dInput = input - pid->lastInput;
  
  // 积分项
  pid->outputSum += pid->ki * error;
  
  // 积分限幅
  if (pid->outputSum > pid->outMax) {
    pid->outputSum = pid->outMax;
  } else if (pid->outputSum < pid->outMin) {
    pid->outputSum = pid->outMin;
  }
  
  // 比例项处理
  float output;
  if (pid->pOnE) {
    // 比例项在误差上
    output = pid->kp * error;
  } else {
    // 比例项在测量值上，需要从积分项中减去
    output = 0;
    pid->outputSum -= pid->kp * dInput;
  }
  
  // 计算总输出
  output += pid->outputSum - pid->kd * dInput;
  
  // 输出限幅
  if (output > pid->outMax) {
    output = pid->outMax;
  } else if (output < pid->outMin) {
    output = pid->outMin;
  }
  
  // 更新状态
  pid->lastInput = input;
  pid->output = output;
  pid->justEvaluated = 1;
  
  return output;
}


void pid_set_tunings(pid_controller_t *pid, float kp, float ki, float kd) {
  pid_set_tunings_with_mode(pid, kp, ki, kd, pid->pOn);
}

void pid_set_tunings_with_mode(pid_controller_t *pid, float kp, float ki, float kd, int pOn) {
  if (kp < 0 || ki < 0 || kd < 0) {
    return;  // 参数不能为负
  }
  
  pid->pOn = pOn;
  pid->pOnE = (pOn == PID_P_ON_E);
  
  // 保存显示用参数
  pid->dispKp = kp;
  pid->dispKi = ki;
  pid->dispKd = kd;
  
  // 计算采样时间比例因子
  float ratio = (float)pid->sampleTime;
  
  // 更新PID参数
  pid->kp = kp;
  pid->ki = ki * ratio;  // 积分项需要根据采样时间调整
  pid->kd = kd / ratio;  // 微分项需要根据采样时间调整
  
  // 根据控制方向调整参数
  if (pid->controllerDirection == PID_REVERSE) {
    pid->kp = (0 - pid->kp);
    pid->ki = (0 - pid->ki);
    pid->kd = (0 - pid->kd);
  }
}


void pid_set_output_limits(pid_controller_t *pid, float min, float max) {
  if (min >= max) {
    return;  // 下限必须小于上限
  }
  
  pid->outMin = min;
  pid->outMax = max;
  
  // 如果当前输出超出限幅，进行调整
  if (pid->inAuto) {
    if (pid->output > max) {
      pid->output = max;
    } else if (pid->output < min) {
      pid->output = min;
    }
    
    // 调整积分项，防止积分饱和
    if (pid->outputSum > max) {
      pid->outputSum = max;
    } else if (pid->outputSum < min) {
      pid->outputSum = min;
    }
  }
}


void pid_set_mode(pid_controller_t *pid, int mode) {
  int new_auto = (mode == PID_MODE_AUTOMATIC);
  
  // 如果模式没有改变，直接返回
  if (new_auto == pid->inAuto) {
    return;
  }
  
  pid->inAuto = new_auto;
  
  // 如果切换到自动模式，需要初始化
  if (new_auto) {
    pid_initialize(pid);
  }
}

void pid_set_controller_direction(pid_controller_t *pid, int direction) {
  if (pid->inAuto && direction != pid->controllerDirection) {
    pid->kp = (0 - pid->kp);
    pid->ki = (0 - pid->ki);
    pid->kd = (0 - pid->kd);
  }
  pid->controllerDirection = direction;
}


void pid_set_sample_time(pid_controller_t *pid, float sampleTime) {
  if (sampleTime > 0) {
    float ratio = sampleTime / pid->sampleTime;
    pid->ki *= ratio;
    pid->kd /= ratio;
    pid->sampleTime = sampleTime;
  }
}


void pid_reset(pid_controller_t *pid) {
  pid->outputSum = 0.0f;
  pid->lastInput = pid->input;
  pid->justEvaluated = 0;
}


void pid_get_status(pid_controller_t *pid, float *error, float *integral, float *derivative) {
  if (error) *error = pid->setpoint - pid->input;
  if (integral) *integral = pid->outputSum;
  if (derivative) *derivative = pid->input - pid->lastInput;
}


static void pid_initialize(pid_controller_t *pid) {
  pid->outputSum = pid->output;
  
  if (pid->outputSum > pid->outMax) {
    pid->outputSum = pid->outMax;
  } else if (pid->outputSum < pid->outMin) {
    pid->outputSum = pid->outMin;
  }
}
