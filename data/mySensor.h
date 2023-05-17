#ifndef mySensor_h
#define mySensor_h
//#include "Arduino.h"
#include <queue>

#include <Adafruit_ADS1X15.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include "Utilities.h"
#include <Arduino.h>


class Sensor_Pressure{

    private:
        Adafruit_ADS1115 ads;
        queue<float> data0;
        queue<float> data1;
        queue<float> data2;
        queue<float> data3;
        queue<float> fsrdata;
        int data_maxlength = 100;
        const int fsr4Pin = 34;
        boolean autoCalibration=false; //at the beginning of the program, the pressure sensor will be calibrated automatically.
        //Then, this flag turned to false forever.

        void setupPressure() {
            if (!ads.begin(0x48, &Wire)) {
                Serial.println("Failed to initialize Pressure Sensor(ADS).");
                while (1);
            }
        }

    public:
        Sensor_Pressure(int data_maxlength){
            this->data_maxlength=data_maxlength;
            setupPressure();
        }

        float calculate_pressure(queue<float> &data){
            float sum=0;
            int queue_length=data.size();
            for (int i=0;i<queue_length;i++){
                // Serial.print("in calculate_pressure:i=");Serial.println(i);
                sum+=data.front();
                data.pop();
            }
            sum/=queue_length;
            // Serial.print("in calculate_pressure:sum=");Serial.println(sum);
            return sum;
        }

        String fetch_packedData(){
            Serial.println("Fetching Data");
            float reading0 = calculate_pressure(data0);
            float reading1 = calculate_pressure(data1);
            float reading2 = calculate_pressure(data2);
            float reading3 = calculate_pressure(data3);
            float reading4=calculate_pressure(fsrdata);
            Serial.println("Data Fetched.");

            String queryString = "reading0=" + String(reading0) 
                                + "&reading1=" + String(reading1)
                                + "&reading2=" + String(reading2)
                                + "&reading3=" + String(reading3)
                                + "&reading4=" + String(reading4);
            Serial.println("queryString generated.");
            println_sensor("0",0,reading0);
            println_sensor("1",0,reading1);
            println_sensor("2",0,reading2);
            println_sensor("3",0,reading3);
            println_sensor("4",0,reading4);
            return queryString;
        }
        void sample(){
            // Serial.println("Collecting Data");
            int16_t adc0, adc1, adc2, adc3;
            float volts0, volts1, volts2, volts3;
            float current_voltage = 3.3;

            adc0 = ads.readADC_SingleEnded(0);
            adc1 = ads.readADC_SingleEnded(1);
            adc2 = ads.readADC_SingleEnded(2);
            adc3 = ads.readADC_SingleEnded(3);


            volts0 = ads.computeVolts(adc0);
            volts1 = ads.computeVolts(adc1);
            volts2 = ads.computeVolts(adc2);
            volts3 = ads.computeVolts(adc3);

            // Serial.println("-----------------------------------------------------------");

            float fsr4Read = (float) analogRead(fsr4Pin);
            float fsr4voltage = fsr4Read * current_voltage / 4096;
            // Serial.print("voltage: ");Serial.println(fsr4voltage);

            float reading0, reading1, reading2, reading3, reading4;
            reading0 = adc0 * (current_voltage / 32767.0);
            reading1 = adc1 * (current_voltage / 32767.0);
            reading2 = adc2 * (current_voltage / 32767.0);
            reading3 = adc3 * (current_voltage / 32767.0);
            Serial.print("reading0: ");Serial.println(reading0);
            Serial.print("reading1: ");Serial.println(reading1);
            Serial.print("reading2: ");Serial.println(reading2);
            Serial.print("reading3: ");Serial.println(reading3);
            Serial.print("reading4: ");Serial.println(reading4);
            reading4 = fsr4voltage;

            if(!autoCalibration){
                reading0 = reading0 >= 0 ? reading0 : 0;
                reading1 = reading1 >= 0 ? reading1 : 0;
                reading2 = reading2 >= 0 ? reading2 : 0;
                reading3 = reading3 >= 0 ? reading3 : 0;
            }


            
            
            queue_safepush(data0,reading0,data_maxlength);
            queue_safepush(data1,reading1,data_maxlength);
            queue_safepush(data2,reading2,data_maxlength);
            queue_safepush(data3,reading3,data_maxlength);
            queue_safepush(fsrdata,reading4,data_maxlength);
            // Serial.print("Current queue length: ");Serial.println(data0.size());

            }
     

};

class Sensor_MPU{
    private:
        Adafruit_MPU6050 mpu;


    public:
        void setupMPU() {
            if (!mpu.begin(0x68, &Wire)) {
                Serial.println("Sensor init failed");
                while (1)
                yield();
            }
            //setupt motion detection
            mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
            mpu.setMotionDetectionThreshold(1);
            mpu.setMotionDetectionDuration(20);
            mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
            mpu.setInterruptPinPolarity(true);
            mpu.setMotionInterrupt(true);
        }

        void fetch_packedData(){
            sensors_event_t a, g, temp;
            mpu.getEvent(&a, &g, &temp);

            /* Print out the values */
            Serial.print("AccelX:"); Serial.print(a.acceleration.x); Serial.print(","); 
            Serial.print("AccelY:"); Serial.print(a.acceleration.y); Serial.print(",");
            Serial.print("AccelZ:"); Serial.print(a.acceleration.z); Serial.print(", ");
            Serial.print("GyroX:"); Serial.print(g.gyro.x); Serial.print(",");
            Serial.print("GyroY:"); Serial.print(g.gyro.y); Serial.print(",");
            Serial.print("GyroZ:"); Serial.print(g.gyro.z); Serial.println("");

            String queryString = "&accelx=" + String(a.acceleration.x)
                                    + "&accely=" + String(a.acceleration.y)
                                    + "&accelz=" + String(a.acceleration.z)
                                    + "&gyrox=" + String(g.gyro.x)
                                    + "&gyroy=" + String(g.gyro.y)
                                    + "&gyroz=" + String(g.gyro.z);
        }

};



#endif