#include "Utilities.h"


void println_sensor(String name, float adc,float reading){
    
    char buffer[100];
    sprintf(buffer,"%s: %f,  %fV\n",name,adc,reading);
    Serial.print(buffer);
    
}

void queue_safepush(queue<float> &data,float reading,int maxlength){
    data.push(reading);
    if (data.size() > maxlength){
        data.pop();
    }
}
