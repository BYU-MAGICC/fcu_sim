#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "mavlink.h"

#include "param.h"
#include "mixer.h"

//TODO temporary
#include <stdio.h>
#include <string.h>

// global variable definitions
params_t _params;

//// local function definitions
//static void init_param_int(param_id_t id, char name[PARAMS_NAME_LENGTH], int32_t value)
//{
//  memcpy(_params.names[id], name, PARAMS_NAME_LENGTH);
//  _params.values[id] = value;
//  _params.types[id] = PARAM_TYPE_INT32;
//}

//static void init_param_float(param_id_t id, char name[PARAMS_NAME_LENGTH], float value)
//{
//  memcpy(_params.names[id], name, PARAMS_NAME_LENGTH);
//  _params.values[id] = *((int32_t *) &value);
//  _params.types[id] = PARAM_TYPE_FLOAT;
//}

//// function definitions
//void init_params(void)
//{
//  if (!read_params())
//  {
//    set_param_defaults();
//    write_params();
//  }

//  for (param_id_t id = 0; id < PARAMS_COUNT; id++)
//    param_change_callback(id);
//}

//void set_param_defaults(void)
//{
//  printf("testing");
//  // temporary: replace with actual initialisation of rest of params
//  char temp_name[PARAMS_NAME_LENGTH];
//  for (param_id_t id = 0; id < PARAMS_COUNT; id++)
//  {
//    sprintf(temp_name, "TEMP_%c%c", 'A' + id/10, 'A' + id%10);
//    init_param_int(id, temp_name, id);
//  }

//  init_param_int(PARAM_BOARD_REVISION, "BOARD_REV", 4);
//  init_param_int(PARAM_HIL_ON, "HIL_ON", 1);

//  init_param_int(PARAM_BAUD_RATE, "BAUD_RATE", 921600);

//  init_param_int(PARAM_SYSTEM_ID, "SYS_ID", 1);
//  init_param_int(PARAM_STREAM_HEARTBEAT_RATE, "STRM_HRTBT", 1);

//  init_param_int(PARAM_STREAM_ATTITUDE_RATE, "STRM_ATTITUDE", 50);
//  init_param_int(PARAM_STREAM_IMU_RATE, "STRM_IMU", 500);
//  init_param_int(PARAM_STREAM_MAG_RATE, "STRM_MAG", 0);
//  init_param_int(PARAM_STREAM_BARO_RATE, "STRM_BARO", 50);
//  init_param_int(PARAM_STREAM_AIRSPEED_RATE, "STRM_AIRSPEED", 50);
//  init_param_int(PARAM_STREAM_GPS_RATE, "STRM_GPS", 0);
//  init_param_int(PARAM_STREAM_SONAR_RATE, "STRM_SONAR", 25);

//  init_param_int(PARAM_STREAM_SERVO_OUTPUT_RAW_RATE, "STRM_SERVO", 0);
//  init_param_int(PARAM_STREAM_RC_RAW_RATE, "STRM_RC", 0);

//  init_param_int(PARAM_DIFF_PRESS_UPDATE, "DIFF_PRESS_UP", 20000); // us
//  init_param_int(PARAM_BARO_UPDATE, "BARO_UPDATE", 20000);
//  init_param_int(PARAM_SONAR_UPDATE, "SONAR_UPDATE", 25000);
//  init_param_int(PARAM_MAG_UPDATE, "MAG_UPDATE", 20000);

//  init_param_int(PARAM_INIT_TIME, "FILTER_INIT_T", 3000); // ms
//  init_param_int(PARAM_FILTER_KP, "FILTER_KP", 1000); // munits
//  init_param_int(PARAM_FILTER_KI, "FILTER_KI", 100);  // munits
//  init_param_int(PARAM_STREAM_ADJUSTED_GYRO, "STRM_ADJUST_GYRO", 1);
//  init_param_float(PARAM_GYRO_X_BIAS, "GYRO_X_BIAS", 0.0f);
//  init_param_float(PARAM_GYRO_Y_BIAS, "GYRO_Y_BIAS", 0.0f);
//  init_param_float(PARAM_GYRO_Z_BIAS, "GYRO_Z_BIAS", 0.0f);
//  init_param_float(PARAM_ACC_X_BIAS,  "ACC_X_BIAS", 0.0f);
//  init_param_float(PARAM_ACC_Y_BIAS,  "ACC_Y_BIAS", 0.0f);
//  init_param_float(PARAM_ACC_Z_BIAS,  "ACC_Z_BIAS", 0.0f);
//  init_param_float(PARAM_ACC_X_TEMP_COMP,  "ACC_X_TEMP_COMP", 0.0f);
//  init_param_float(PARAM_ACC_Y_TEMP_COMP,  "ACC_Y_TEMP_COMP", 0.0f);
//  init_param_float(PARAM_ACC_Z_TEMP_COMP,  "ACC_Z_TEMP_COMP", 0.0f);

//  init_param_int(PARAM_MOTOR_PWM_SEND_RATE, "MOTOR_PWM_PERIOD", 50);
//  init_param_int(PARAM_MOTOR_IDLE_PWM, "MOTOR_IDLE_PWM", 1100);
//  init_param_int(PARAM_SPIN_MOTORS_WHEN_ARMED, "ARM_SPIN_MOTORS", true);
//  init_param_int(PARAM_RC_TYPE, "RC_TYPE", 1);
//  init_param_int(PARAM_RC_X_CHANNEL, "RC_X_CHN", 0);
//  init_param_int(PARAM_RC_Y_CHANNEL, "RC_Y_CHN", 1);
//  init_param_int(PARAM_RC_Z_CHANNEL, "RC_Z_CHN", 3);
//  init_param_int(PARAM_RC_F_CHANNEL, "RC_F_CHN", 2);

//  init_param_int(PARAM_RC_ATTITUDE_OVERRIDE_CHANNEL, "RC_ATT_OVRD_CHN", 4);
//  init_param_int(PARAM_RC_THROTTLE_OVERRIDE_CHANNEL, "RC_THR_OVRD_CHN", 4);
//  init_param_int(PARAM_RC_ATT_CONTROL_TYPE_CHANNEL,  "RC_ATT_CTRL_CHN", 5);
//  init_param_int(PARAM_RC_F_CONTROL_TYPE_CHANNEL,    "RC_F_CTRL_CHN", 5);

//  init_param_int(PARAM_RC_X_CENTER, "RC_X_CENTER", 1500);
//  init_param_int(PARAM_RC_Y_CENTER, "RC_Y_CENTER", 1500);
//  init_param_int(PARAM_RC_Z_CENTER, "RC_Z_CENTER", 1500);
//  init_param_int(PARAM_RC_F_BOTTOM, "RC_F_BOTTOM", 1000);
//  init_param_int(PARAM_RC_X_RANGE,  "RC_X_RANGE", 1000);
//  init_param_int(PARAM_RC_Y_RANGE,  "RC_Y_RANGE", 1000);
//  init_param_int(PARAM_RC_Z_RANGE,  "RC_Z_RANGE", 1000);
//  init_param_int(PARAM_RC_F_RANGE,  "RC_F_RANGE", 1000);

//  init_param_int(PARAM_RC_OVERRIDE_DEVIATION, "RC_OVRD_DEV", 100);
//  init_param_int(PARAM_OVERRIDE_LAG_TIME, "OVRD_LAG_TIME", 1000);
//  init_param_int(PARAM_RC_OVERRIDE_TAKE_MIN_THROTTLE, "MIN_THROTTLE", false);

//  init_param_int(PARAM_RC_MAX_ROLL_MRAD, "RC_MAX_ROLL", 786); // 45 deg
//  init_param_int(PARAM_RC_MAX_PITCH_MRAD, "RC_MAX_PITCH", 786);
//  init_param_int(PARAM_RC_MAX_ROLLRATE_MRAD_S, "RC_MAX_ROLLRATE", 12566); // 720 deg/s
//  init_param_int(PARAM_RC_MAX_PITCHRATE_MRAD_S, "RC_MAX_PITCHRATE", 12566); // 720 deg/s
//  init_param_int(PARAM_RC_MAX_YAWRATE_MRAD_S, "RC_MAX_YAWRATE", 6283); // 360 deg/s

//  init_param_int(PARAM_ARM_STICKS, "ARM_STICKS", true);
//  init_param_int(PARAM_ARM_CHANNEL, "ARM_CHANNEL", 5);
//  init_param_int(PARAM_ARM_THRESHOLD, "ARM_THRESHOLD", 150);

//  init_param_int(PARAM_MAX_COMMAND, "PARAM_MAX_CMD", 1000);

//  init_param_int(PARAM_PID_ROLL_RATE_P, "PID_ROLL_RATE_P", 10000);
//  init_param_int(PARAM_MAX_ROLL_RATE, "MAX_ROLL_RATE", 12566);

//  init_param_int(PARAM_PID_PITCH_RATE_P, "PID_PITCH_RATE_P", 10000);
//  init_param_int(PARAM_MAX_PITCH_RATE, "MAX_PITCH_RATE", 12566);

//  init_param_int(PARAM_PID_YAW_RATE_P, "PID_YAW_RATE_P", 100);
//  init_param_int(PARAM_PID_YAW_RATE_I, "PID_YAW_RATE_I", 0);
//  init_param_int(PARAM_MAX_YAW_RATE, "MAX_YAW_RATE", 6283);

//  init_param_int(PARAM_PID_ROLL_ANGLE_P, "PID_ROLL_ANG_P", 2000);
//  init_param_int(PARAM_PID_ROLL_ANGLE_I, "PID_ROLL_ANG_I", 0);
//  init_param_int(PARAM_PID_ROLL_ANGLE_D, "PID_ROLL_ANG_D", 0);
//  init_param_int(PARAM_MAX_ROLL_ANGLE, "MAX_ROLL_ANG", 786);

//  init_param_int(PARAM_PID_PITCH_ANGLE_P, "PID_PITCH_ANG_P", 2000);
//  init_param_int(PARAM_PID_PITCH_ANGLE_I, "PID_PITCH_ANG_I", 0);
//  init_param_int(PARAM_PID_PITCH_ANGLE_D, "PID_PITCH_ANG_D", 0);
//  init_param_int(PARAM_MAX_PITCH_ANGLE, "MAX_PITCH_ANG", 786);

//  init_param_int(PARAM_PID_ALT_P, "PID_ALT_P", 10);
//  init_param_int(PARAM_PID_ALT_I, "PID_ALT_I", 0);
//  init_param_int(PARAM_PID_ALT_D, "PID_ALT_D", 0);


//  init_param_int(PARAM_MIXER, "MIXER", QUADCOPTER_X);
//  init_param_int(PARAM_ELEVATOR_REVERSE, "ELEVATOR_REV", 0);
//  init_param_int(PARAM_AILERON_REVERSE, "AIL_REV", 0);
//  init_param_int(PARAM_RUDDER_REVERSE, "RUDDER_REV", 0);
//  init_param_int(PARAM_FIXED_WING, "FIXED_WING", false);
//}

//bool read_params(void)
//{
//  return false;
//}

//bool write_params(void)
//{
//  return false;
//}

//void param_change_callback(param_id_t id)
//{
//  return;
//}

//param_id_t lookup_param_id(const char name[PARAMS_NAME_LENGTH])
//{
//  for (param_id_t id = 0; id < PARAMS_COUNT; id++)
//  {
//    bool match = true;
//    for (uint8_t i = 0; i < PARAMS_NAME_LENGTH; i++)
//    {
//      // compare each character
//      if (name[i] != _params.names[id][i])
//      {
//        match = false;
//        break;
//      }

//      // stop comparing if end of string is reached
//      if (_params.names[id][i] == '\0')
//        break;
//    }

//    if (match)
//      return id;
//  }

//  return PARAMS_COUNT;
//}

//bool set_param_by_id(param_id_t id, int32_t value)
//{
//  if (id < PARAMS_COUNT && value != _params.values[id])
//  {
//    _params.values[id] = value;
//    param_change_callback(id);
//    return true;
//  }
//  return false;
//}

//bool set_param_by_name(const char name[PARAMS_NAME_LENGTH], int32_t value)
//{
//  uint8_t id = lookup_param_id(name);
//  return set_param_by_id(id, value);
//}

//bool set_param_by_id_float(param_id_t id, float value)
//{
//  return set_param_by_id(id, *(int32_t *) &value);
//}

//bool set_param_by_name_float(const char name[PARAMS_NAME_LENGTH], float value)
//{
//  return set_param_by_name(name, *(int32_t *) &value);
//}

//float get_param_float(param_id_t id)
//{
//  return *(float *) &_params.values[id];
//}

#ifdef __cplusplus
}
#endif
