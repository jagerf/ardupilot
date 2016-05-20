/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Copter.h"



// drop_init - initialise land controller
bool Copter::drop_init(bool ignore_checks)
{

    drop_time_start = millis();


    return true;

}

// drop_run - runs the land controller
// should be called at 100hz or more
void Copter::drop_run()
{
    float target_roll = 0.0f, target_pitch = 0.0f;
    float target_yaw_rate = 0;
    float cmb_rate = 0;
    bool start = fase;

    // set motors to full range
    motors.set_desired_spool_state(AP_Motors::DESIRED_THROTTLE_UNLIMITED);

    if(!start){
        if((millis()-drop_time_start) <= 2000) {

            attitude_control.input_euler_angle_roll_pitch_euler_rate_yaw_smooth(target_roll, target_pitch,
                                                                                target_yaw_rate, get_smoothing_gain());

            pos_control.set_alt_target_from_climb_rate(cmb_rate, G_Dt, true);
            pos_control.update_z_controller();

        }else{
            start = true;
            motors.set_desired_spool_state(AP_Motors::DESIRED_SPIN_WHEN_ARMED);
            time_start = millis();
        }
    }else{
        if((millis()-drop_time_start) <= 3000) {

            // call attitude controller
            attitude_control.input_euler_angle_roll_pitch_euler_rate_yaw_smooth(target_roll, target_pitch,
                                                                                target_yaw_rate,
                                                                                get_smoothing_gain());

            pos_control.accel_to_throttle(g.drop_acc);

        }else{

            // send message to gcs and dataflash
            gcs_send_text(MAV_SEVERITY_INFO,"Parachute: Released");
            Log_Write_Event(DATA_PARACHUTE_RELEASED);

            // disarm motors
            init_disarm_motors();

            // release parachute
            parachute.release();

        }
    }



}
