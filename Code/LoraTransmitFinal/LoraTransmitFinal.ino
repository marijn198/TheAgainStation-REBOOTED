#include <rn2xx3.h>
#include <SoftwareSerial.h>
#include <dht11.h>
#define DHT11_PIN 4
#include <SFE_BMP180.h>
#include <Wire.h>

dht11 DHT;
SFE_BMP180 pressure;

SoftwareSerial mySerial(10, 11); // RX, TX

//create an instance of the rn2xx3 library, 
//giving the software serial as port to use
rn2xx3 myLora(mySerial);

int Temperature = 0;
int Humidity = 0;
int Pressure = 0;
//int Brightness = 0;
char status;
double T,P,p0,a;
int LDRPinnetje = A1; //analog pin 0

// the setup routine runs once when you press reset:
void setup() 
{
  //output LED pin
  pinMode(13, OUTPUT);
  led_on();
  
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600); //serial port to computer
  mySerial.begin(9600); //serial port to radio
  Serial.println(F("Startup"));

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem.

    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
  
  initialize_radio();

  //transmit a startup message
  myLora.tx(F("TTN Mapper on TTN Enschede node"));

  led_off();
  delay(2000);
}

void initialize_radio()
{
  //reset rn2483
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println(F("Communication with RN2xx3 unsuccesful. Power cycle the board."));
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println(F("When using OTAA, register this DevEUI: "));
  Serial.println(myLora.hweui());
  Serial.println(F("RN2xx3 firmware version:"));
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println(F("Trying to join TTN"));
  bool join_result = false;
  
  //ABP: initABP(String addr, String AppSKey, String NwkSKey);
  join_result = myLora.initABP("260114B0", "682C88F07C18825DA43C04C961585BCB", "E1DCD6169E7A10F9A2C023329C97F023");
  
  //OTAA: initOTAA(String AppEUI, String AppKey);
  //join_result = myLora.initOTAA("70B3D57ED00001A6", "A23C96EE13804963F8C2BD6285448198");

  while(!join_result)
  {
    Serial.println(F("Unable to join. Are your keys correct, and do you have TTN coverage?"));
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println(F("Successfully joined TTN"));
  
}

// the loop routine runs over and over again forever:
void loop() 
{

  int chk;
  Serial.print(F("DHT11, \t"));
  chk = DHT.read(DHT11_PIN);    // READ DATA
  switch (chk){
    case DHTLIB_OK:  
                Serial.print(F("OK,\t")); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print(F("Checksum error,\t")); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print(F("Time out error,\t")); 
                break;
    default: 
                Serial.print(F("Unknown error,\t")); 
                break;
  }
 // DISPLAT DATA
  Serial.print(DHT.humidity,1);
  Humidity = DHT.humidity;
  Serial.print(",\t");
  Serial.println(DHT.temperature,1);
  Temperature = DHT.temperature;

    


  // Loop here getting pressure readings every 10 seconds.

  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print(F("temperature: "));
      Serial.print(T,2);
      Serial.print(F(" deg C, "));
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(F(" deg F"));
      
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.print(F("absolute pressure: "));
          Serial.print(P,2);
          Serial.print(F(" mb, "));
          Serial.print(P*0.0295333727,2);
          Serial.println(F(" inHg"));
          //Serial.println("");

        }
        else Serial.println(F("error retrieving pressure measurement\n"));
      }
      else Serial.println(F("error starting pressure measurement\n"));
    }
    else Serial.println(F("error retrieving temperature measurement\n"));
  }
  else Serial.println(F("error starting temperature measurement\n"));
    
    int Brightness = analogRead(LDRPinnetje); 

    Serial.print(F("Brightness: "));
    Serial.println(Brightness);
    Serial.println("");
   
    led_on();

    Serial.println("TXing");
    Serial.println("");
    String temp = String(Temperature);
    String humid = String(Humidity);
    String pressure = String(P);
    String bright = String(Brightness);
    String Break1 = String('_');
    String Break2 = String('_');
    String Break3 = String('_');
    String all = (temp += Break1 += humid += Break2 += pressure += Break3 += bright);
    myLora.tx(all); //one byte, blocking function
    

    
    led_off();
    delay(200);
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}
