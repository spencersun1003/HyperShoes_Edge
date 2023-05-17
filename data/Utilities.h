#ifndef Utilities_h
#define Utilities_h
#include <iostream>
#include <queue>
#include <Arduino.h>
// #ifndef Arduino_h
// #include "Arduino.h"
// #endif
using std::queue;

void println_sensor(String name, float adc,float reading0);
void queue_safepush(queue<float> &data,float reading,int maxlength);


#endif