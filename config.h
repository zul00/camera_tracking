#ifndef __CONFIG_H
#define __CONFIG_H

/* Compilation config */
#define ARCH
//#define DEBIAN
//#define OVERO

#ifndef OVERO
#define SHOW_GUI
#endif

/* System config */
// PWM
#define MAX_PWM_PAN   2500.0
#define MAX_PWM_TILT  2500.0
// Encoder
#define ENC_PAN_PULSES_PER_ROTATION 4000
#define ENC_TILT_PULSES_PER_ROTATION 900
// Vision
#define CAM_ID  1

// https://www.learnopencv.com/approximate-focal-length-for-webcams-and-cell-phone-cameras/
// http://support.logitech.com/en_us/product/webcam-c250/specs
#define DFOV    1.10    // 63 deg
#define HFOV    0.91    // 52 deg
#define VFOV    0.70    // 40 deg
#define sinHFOV2  0.4394  // sin(HFOV/2)
#define sinVFOV2  0.3429  // sin(VFOV/2)

#define PERIOD_MS   10.0

#endif
