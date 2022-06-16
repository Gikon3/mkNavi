#include "compass.h"
#include "sensor.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Fusion.h"
#include "cmsis_os.h"

static Orientation mxAngle;

const float samplePeriod = 0.05f;
static const FusionVector3 gyroscopeSensitivity = {
    .axis.x = 0.00875f,
    .axis.y = 0.00875f,
    .axis.z = 0.00875f,
};
static const FusionVector3 accelerometerSensitivity = {
    .axis.x = 1.831E-4f,
    .axis.y = 1.831E-4f,
    .axis.z = 1.831E-4f,
};
static const FusionVector3 hardIronBias = {
    .axis.x = 0.0f,
    .axis.y = 0.0f,
    .axis.z = 0.0f,
};

Orientation const* compass()
{
  static FusionBias fusionBias;
  static FusionAhrs fusionAhrs;
  static int8_t initialFl = 0;
  if (!initialFl) {
    initialFl = 1;
    // Initialise gyroscope bias correction algorithm
    FusionBiasInitialise(&fusionBias, 0.5f, samplePeriod); // stationary threshold = 0.5 degrees per second
    // Initialise AHRS algorithm
    FusionAhrsInitialise(&fusionAhrs, 1.5f); // gain = 0.5
    // Set optional magnetic field limits
    FusionAhrsSetMagneticField(&fusionAhrs, 20.0f, 70.0f); // valid magnetic field range = 20 uT to 70 uT
  }

  // Calibrate gyroscope
  FusionVector3 uncalibratedGyroscope = {
      .axis.x = sens.gyr.raw.x,
      .axis.y = sens.gyr.raw.y,
      .axis.z = sens.gyr.raw.z,
  };
  FusionVector3 calibratedGyroscope = FusionCalibrationInertial(uncalibratedGyroscope,
                                                                FUSION_ROTATION_MATRIX_IDENTITY,
                                                                gyroscopeSensitivity,
                                                                FUSION_VECTOR3_ZERO);

  // Calibrate accelerometer
  FusionVector3 uncalibratedAccelerometer = {
      .axis.x = sens.acc.raw.x,
      .axis.y = sens.acc.raw.y,
      .axis.z = sens.acc.raw.z,
  };
  FusionVector3 calibratedAccelerometer = FusionCalibrationInertial(uncalibratedAccelerometer,
                                                                    FUSION_ROTATION_MATRIX_IDENTITY,
                                                                    accelerometerSensitivity,
                                                                    FUSION_VECTOR3_ZERO);

  // Calibrate magnetometer
  FusionVector3 uncalibratedMagnetometer = {
      .axis.x = sens.mag.ripe.y * 100.0,
      .axis.y = -sens.mag.ripe.x * 100.0,
      .axis.z = sens.mag.ripe.z * 100.0,
  };
  FusionVector3 calibratedMagnetometer = FusionCalibrationMagnetic(uncalibratedMagnetometer,
                                                                   FUSION_ROTATION_MATRIX_IDENTITY,
                                                                   hardIronBias);

  // Update gyroscope bias correction algorithm
  calibratedGyroscope = FusionBiasUpdate(&fusionBias, calibratedGyroscope);

  // Update AHRS algorithm
  FusionAhrsUpdate(&fusionAhrs, calibratedGyroscope, calibratedAccelerometer, calibratedMagnetometer, samplePeriod);

  // Print Euler angles
  FusionEulerAngles eulerAngles = FusionQuaternionToEulerAngles(FusionAhrsGetQuaternion(&fusionAhrs));
  mxAngle.roll = -eulerAngles.angle.roll;
  mxAngle.pitch = eulerAngles.angle.pitch;
  if (eulerAngles.angle.yaw <= 0.0) mxAngle.yaw = -(eulerAngles.angle.yaw + 180.0);
  else mxAngle.yaw = -(eulerAngles.angle.yaw - 180.0);

  return &mxAngle;
}
