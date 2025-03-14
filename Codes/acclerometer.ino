#include<LiquidCrystal_I2C.h>
#include <AltSoftSerial.h>
#include <TinyGPS++.h>

#include <SoftwareSerial.h>
#include <math.h>

#include<Wire.h>
//must add i2c lcd address use i2c-scanner.ino file
LiquidCrystal_I2C lcd(0x27, 16, 2);
//--------------------------------------------------------------
//emergency phone number with country code
const String EMERGENCY_PHONE = "+91XXXXXXXXXX";
//--------------------------------------------------------------
//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim900(rxPin,txPin);
//--------------------------------------------------------------
//GPS Module RX pin to Arduino 9
//GPS Module TX pin to Arduino 8
AltSoftSerial neogps;
TinyGPSPlus gps;
//--------------------------------------------------------------
String sms_status,sender_number,received_date,msg;
String latitude, longitude;
//--------------------------------------------------------------
#define BUZZER 12
#define BUTTON 11
//--------------------------------------------------------------
#define xPin A1
#define yPin A2
#define zPin A3
//--------------------------------------------------------------

byte updateflag;

int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 2, devibrate = 75;
int magnitude = 0;
int sensitivity = 70;
double angle;
boolean impact_detected = false;
//Used to run impact routine every 2mS.
unsigned long time1;
unsigned long impact_time;
unsigned long alert_delay = 10000; //30 seconds

//___________________ setup() function______________________________

void setup()
{
  //--------------------------------------------------------------
  //Serial.println("Arduino serial initialize");
  Serial.begin(9600);
  //--------------------------------------------------------------
  //Serial.println("sim900L serial initialize"); 
  sim900.begin(9600);
  //--------------------------------------------------------------
  //Serial.println("NEO6M serial initialize");
  neogps.begin(9600);
  //--------------------------------------------------------------
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  //--------------------------------------------------------------
  //initialize lcd screen
  lcd.init();
  // turn on the backlight
  lcd.backlight();
  lcd.clear();
  //--------------------------------------------------------------
  sms_status = "";
  sender_number="";
  received_date="";
  msg="";
  //--------------------------------------------------------------
  sim900.println("AT"); //Check GSM Module
  delay(1000);
  //SendAT("AT", "OK", 2000); //Check GSM Module
  sim900.println("ATE1"); //Echo ON
  delay(1000);
  //SendAT("ATE1", "OK", 2000); //Echo ON
  sim900.println("AT+CPIN?"); //Check SIM ready
  delay(1000);
  //SendAT("AT+CPIN?", "READY", 2000); //Check SIM ready
  sim900.println("AT+CMGF=1"); //SMS text mode
  delay(1000);
  //SendAT("AT+CMGF=1", "OK", 2000); //SMS text mode
  sim900.println("AT+CNMI=1,1,0,0,0"); /// Decides how newly arrived SMS should be handled
  delay(1000);
  //SendAT("AT+CNMI=1,1,0,0,0", "OK", 2000); //set sms received format
  //AT +CNMI = 2,1,0,0,0 - AT +CNMI = 2,2,0,0,0 (both are same)
  //--------------------------------------------------------------
  time1 = micros(); 
  //Serial.print("time1 = "); Serial.println(time1);
  //--------------------------------------------------------------
  //read calibrated values. otherwise false impact will trigger
  //when you reset your Arduino. (By pressing reset button)
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
  //--------------------------------------------------------------
}




 //______________________void loop() is here_________________________________________

void loop()
{
  //--------------------------------------------------------------
  //call impact routine every 2mS
  if (micros() - time1 > 1999) Impact();
  //--------------------------------------------------------------
  if(updateflag > 0) 
  {
    updateflag=0;
    Serial.println("Impact detected!!");
    Serial.print("Magnitude:"); Serial.println(magnitude);

    getGps();
    digitalWrite(BUZZER, HIGH);
    impact_detected = true;
    impact_time = millis();
    
    lcd.clear();
    lcd.setCursor(0,0); //col=0 row=0
    lcd.print("Crash Detected");
    lcd.setCursor(0,1); //col=0 row=1
    lcd.print("Magnitude:"+String(magnitude));
  }
  //--------------------------------------------------------------
  if(impact_detected == true)
  {
    if(millis() - impact_time >= alert_delay) {
      digitalWrite(BUZZER, LOW);
      makeCall();
      delay(1000);
      sendAlert();
      impact_detected = false;
      impact_time = 0;
    }
  }
  
  if(digitalRead(BUTTON) == LOW){
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;
  }
  //--------------------------------------------------------------
  while(sim900.available()){
    parseData(sim900.readString());
  }
  //--------------------------------------------------------------
  while(Serial.available())  {
    sim900.println(Serial.readString());
  }
  //--------------------------------------------------------------


}





 //__________________Impact() function______________________________________

void Impact()
{
  //--------------------------------------------------------------
  time1 = micros(); // resets time value
  //--------------------------------------------------------------
  int oldx = xaxis; //store previous axis readings for comparison
  int oldy = yaxis;
  int oldz = zaxis;

  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
  
  //--------------------------------------------------------------
  //loop counter prevents false triggering. Vibration resets if there is an impact. Don't detect new changes until that "time" has passed.
  vibration--; 
  //Serial.print("Vibration = "); Serial.println(vibration);
  if(vibration < 0) vibration = 0;                                
  //Serial.println("Vibration Reset!");
  
  if(vibration > 0) return;
  //--------------------------------------------------------------
  deltx = xaxis - oldx;                                           
  delty = yaxis - oldy;
  deltz = zaxis - oldz;
  
  //Magnitude to calculate force of impact.
  magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));
  //-------------------------------------------------------------------------------
  if (magnitude >= sensitivity) //impact detected
  {
    updateflag=1;
    // reset anti-vibration counter
    vibration = devibrate;
  }
  //--------------------------------------------------------------------------------
  else
  {
    //if (magnitude > 15)
      //Serial.println(magnitude);
    //reset magnitude of impact to 0
    magnitude=0;
  }
  //--------------------------------------------------------------------------------
}





 //___________________parseData() function_____________________________________________
 
 
void parseData(String buff){
  Serial.println(buff);

  unsigned int len, index;
  //--------------------------------------------------------------
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //---------------------------------------------------------------------------
  if(buff != "OK"){
    //--------------------------------------------------------------
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
    
    buff.remove(0, index+2);
    //Serial.println(buff);
    //--------------------------------------------------------------
    if(cmd == "+CMTI"){
      //get newly arrived memory location and store it in temp
      //temp = 4
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length()); 
      temp = "AT+CMGR=" + temp + "\r"; 
      //AT+CMGR=4 i.e. get message stored at memory location 4
      sim900.println(temp); 
    }
    //--------------------------------------------------------------
    else if(cmd == "+CMGR"){
      //extractSms(buff);
      //Serial.println(buff.indexOf(EMERGENCY_PHONE));
      if(buff.indexOf(EMERGENCY_PHONE) > 1){
        buff.toLowerCase();
        //Serial.println(buff.indexOf("get gps"));
        if(buff.indexOf("get gps") > 1){
          getGps();
          String sms_data;
          sms_data = "GPS Location Data\r";
          sms_data += "http://maps.google.com/maps?q=loc:";
          sms_data += latitude + "," + longitude;
        
          sendSms(sms_data);
        }
      }
    }
    //--------------------------------------------------------------
  }
  else{
  //The result of AT Command is "OK"
  }
  //-----------------------------------------------------------------------------
}


 //____________________getGps() Function_____________________________________________________

void getGps()
{
  // Can take up to 60 seconds
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;){
    while (neogps.available()){
      if (gps.encode(neogps.read())){
        newData = true;
        break;
      }
    }
  }
  
  if (newData) //If newData is true
  {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
    newData = false;
  }
  else {
    Serial.println("No GPS data is available");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No GPS data is available");
    latitude = "";
    longitude = "";
  }

  Serial.print("Latitude= "); Serial.println(latitude);
  Serial.print("Lngitude= "); Serial.println(longitude);
}



//_____________sendAlert() function____________________________________

void sendAlert()
{
  String sms_data;
  sms_data = "Accident Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms(sms_data);
}



//____________________makeCall() function_______________________________

void makeCall()
{
  Serial.println("calling....");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("calling....");
  sim900.println("ATD"+EMERGENCY_PHONE+";");
  delay(20000); //20 sec delay
  sim900.println("ATH");
  delay(1000); //1 sec delay
}




 //_________________sendSms() function_________________________________

 
 void sendSms(String text)
{
  //return;
  sim900.print("AT+CMGF=1\r");
  delay(1000);
  sim900.print("AT+CMGS=\""+EMERGENCY_PHONE+"\"\r");
  delay(1000);
  sim900.print(text);
  delay(100);
  sim900.write(0x1A); //ascii code for ctrl-26 //sim900.println((char)26); //ascii code for ctrl-26
  delay(1000);
  Serial.println("SMS Sent Successfully.");
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("SMS Sent");
  lcd.setCursor(1,1);
  lcd.print("Successfully");
}






 //_____________________SendAT() function______________________________________________
 
boolean SendAT(String at_command, String expected_answer, unsigned int timeout){

    uint8_t x=0;
    boolean answer=0;
    String response;
    unsigned long previous;
    
    //Clean the input buffer
    while( sim900.available() > 0) sim900.read();

    sim900.println(at_command);
    
    x = 0;
    previous = millis();

    //this loop waits for the answer with time out
    do{
        //if there are data in the UART input buffer, reads it and checks for the asnwer
        if(sim900.available() != 0){
            response += sim900.read();
            x++;
            // check if the desired answer (OK) is in the response of the module
            if(response.indexOf(expected_answer) > 0){
                answer = 1;
                break;
            }
        }
    }while((answer == 0) && ((millis() - previous) < timeout));

  Serial.println(response);
  return answer;
}
