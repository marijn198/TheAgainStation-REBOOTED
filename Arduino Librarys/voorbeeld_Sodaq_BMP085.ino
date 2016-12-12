#include <Wire.h>
#include <Sodaq_BMP085.h>
#include <Sodaq_SHT2x.h> 
// Autonomo Serial Monitor
#define debugSerial SerialUSB 
//TPH BMP sensor
Sodaq_BMP085 bmp; 
void setup() {  
    // put your setup code here, to run once:  debugSerial.begin(57600);  
    debugSerial.println("Sending payload: TempSHT21T, TempBMP, PressureBMP, HumiditySHT21T");  
    setupTPH();
} 

void loop() {  
    // put your main code here, to run repeatedly:  
    String reading = takeTPHReading();  
    debugSerial.println(reading);
} 
    void setupTPH(){  
        //Initialise the wire protocol for the TPH sensors  Wire.begin();  
        //Initialise the TPH BMP sensor  
        bmp.begin();
    } 
    String takeTPHReading(){  
        //Create a String type data record in csv format  
        //TempSHT21, TempBMP, PressureBMP, HumiditySHT21  
       String data = String(SHT2x.GetTemperature())  + ", ";  
       //BMPTemp is commented out, the data will be to long if you also send batt volt.  
        data += String(bmp.readTemperature()) + ", ";  
        data += String(bmp.readPressure() / 100)  + ", ";  
       data += String(SHT2x.GetHumidity());  
       return data;
    }
}
