#include <Wire.h>
#include <MPU6050.h>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  Serial.println("MPU6050 initialized. Starting inference...");
}

void loop() {
  float ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Convert raw values to g (depends on your MPU6050 library)
  float accX = ax / 16384.0;
  float accY = ay / 16384.0;
  float accZ = az / 16384.0;

  // Préparer l’entrée pour le modèle Edge Impulse
  float input[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
  for (int i = 0; i < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; i += 3) {
    input[i] = accX;
    input[i + 1] = accY;
    input[i + 2] = accZ;
    delay(20); // Ajuste selon la fréquence du modèle (ex: 50Hz)
  }

  // Inference Edge Impulse
  ei_impulse_result_t result = { 0 };
  signal_t signal;
  int err = numpy::signal_from_buffer(input, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
  if (err != 0) {
    Serial.println("Signal error");
    return;
  }

  EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
  Serial.print("Prediction: ");
  for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
    Serial.print(result.classification[ix].label);
    Serial.print(": ");
    Serial.print(result.classification[ix].value, 3);
    Serial.print("  ");
  }
  Serial.println();
}
