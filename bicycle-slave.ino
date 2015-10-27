 //2015_8_19
//Run&Run Team, Sin Yu Seung
//Slave
//ATmega328P-PU on Board Program
//for more info - http://blog.naver.com/sinu8361

#include <LiquidCrystal.h> //for Lcd display
LiquidCrystal lcd(7, 6, 11, 12, 4, 3); //pin number here
#include <Wire.h> //for I2C
const int ledPin = 13;
volatile byte rpmcount; //for rpm_function
int rpm; //out Rotation Per Minute
//unsigned long timeold; //for calculate time
const int VsPin = 0; //Source Voltage sensing pin
const int VbPin = 1; //Battery Voltage sensing pin
double Vs; //Source Voltage
double Vb; //Battery Voltage
const int Vs_Rraito = 10; //Voltage sensing R ratio
const int Vb_Rraito = 10;
double I; //Amp_Source to Battery
const int I_calR = 1.2; //Amp sensing R
double W = 0; //power
double totalWatt = 0; //for average w_cal function
unsigned int average = 0;
double averageWatt;
double Wa; //for wh_cal function
double Wb;
unsigned long Ta;
unsigned long Tb;
double Ws;
unsigned long totalWs = 1;
bool State = false;
const int okPin = 2; //ok botton Pin
const int connectionPin = 3; //connection sensing Pin
bool connectionEnd = true;//for mode sweeping
bool disconnected = false;
int usetime = 0;
int remaintime = 0;
int timereceived = 0;
int alramPin = 10; //pin for alram spk
void rpm_fun()//for RPM calculation
{
   rpmcount++;
}
/*void RPM_cal()//RPM calculate function [TEST CODE]
{
   detachInterrupt(0);
   //rpm = 30*1000/(millis() - timeold)*rpmcount*2;
   rpm = 60000/(millis() - timeold)*rpmcount;
   timeold = millis();
   rpmcount = 0;
   attachInterrupt(1, rpm_fun, FALLING);//Caution!! Do not change this code (ATmega328P-PU's internal Hard Ware program)
}*/
void Power_cal(int VsPin, int VbPin)//Power calculate function
{
  Vs = analogRead(VsPin)/1024.0*5*Vs_Rraito;//V
  Vb = analogRead(VbPin)/1024.0*5*Vb_Rraito;
  I = (Vs - Vb)/I_calR;//A
  W = Vb * I;//W
}
void Wattage_cal(int W)//average w_cal function
{
  totalWatt += W;
  average++;
  averageWatt = totalWatt/average;
}
void wh_cal(int W)//wh_cal function
{
  State = !State;
  if(State)
  {
    Wa = W;
    Ta = millis();
  }
  else
  {
    Wb = W;
    Tb = millis();
    Ws = (Wa + Wb)/2*(Tb - Ta)/1000;
    totalWs += Ws;
  }
}
void remaintime_cal(int usetime)
{
  usetime = millis()/60000;
  remaintime = timereceived - usetime;
}
void Serialprint()//for Jason trasmission to OTG cable
{
  
   Serial.print("[");
   Serial.print(rpm);
   Serial.print(",");
   Serial.print(W);
   Serial.print(",");
   Serial.print(totalWs-1);
   Serial.println("]");
   
   /*Serial.println("{"); [TEST CODE]
   Serial.print("  \"Current RPM\":");
   Serial.print("\"");
   Serial.print(rpm);
   Serial.println("\"");
   Serial.print("  \"Watt\":");
   Serial.print("\"");
   Serial.print(W);
   Serial.println("\"");
   Serial.print("  \"Average Watt\":");
   Serial.print("\"");
   Serial.print(averageWatt);
   Serial.println("\"");
   Serial.println("}");
   
   Serial.print("ws = ");
   Serial.println(Ws);
   
   Serial.print("total ws = ");
   Serial.println(totalWs-1);
   */
}
void requestEvent()
{
  Wire.write(totalWs);
  totalWs = 0;
  delay(500);
}
void receiveEvent(int howMany)
{
  if(Wire.available());
  {
    timereceived = Wire.read();
  }
}
void alram()
{
  while(true)
  {
    digitalWrite(alramPin, HIGH);
    delay(100);
    digitalWrite(alramPin, LOW);
    delay(100);
  }
}
void setup()
 {
   lcd.begin(20, 4);
   attachInterrupt(1, rpm_fun, FALLING);//Pulse in 3Pin
   pinMode(ledPin, OUTPUT);
   rpmcount = 0;
   rpm = 0;
   //timeold = 0;
   Serial.begin(9600);
   Wire.begin(1);
   Wire.onRequest(requestEvent);
   Wire.onReceive(receiveEvent);
 }
 void loop()
 {
   if((analogRead(connectionPin)<255 && !connectionEnd) && disconnected)//sens the connection
   {
     detachInterrupt(0);
     lcd.clear();
     lcd.print("Want to connect?");
     lcd.setCursor(0, 1);
     lcd.print("Press Connection button");
     lcd.setCursor(0, 2);
     delay(100);
     if(totalWs == 0)//sens the Data reading code confurmed
     {
       lcd.clear();
       lcd.print("Connection progress");
       lcd.setCursor(0,1);
       lcd.print("Reset the value");
       delay(1000);
       //cal the wattage and use time
       //transmission code in here
       //Reset the value
       totalWatt = 0;
       average = 0;
       averageWatt = 0;
       totalWs = 1;
       usetime = 0;
       remaintime = 0;
       timereceived = 0;
       connectionEnd = true;
       disconnected = false;
       digitalWrite(ledPin, HIGH);
       //transmission latch close sig
       lcd.clear();
       lcd.print("Connection end");
       lcd.setCursor(0,1);
       lcd.print("Thanks for using!!");
       delay(4000);       
     }
   }
     if(analogRead(connectionPin)<225 && !disconnected)//ready to disconnection
     {
       lcd.clear();
       lcd.print("Run&Run Team");
       lcd.setCursor(0, 1);
       lcd.print("Bicycle Rent Service");
       delay(100);
       //display logo code here
       if(timereceived == !0)//sens the disconntion sig
       {
         disconnected = true;
         delay(1000);
         digitalWrite(ledPin, LOW);
         lcd.clear();
         lcd.print("You can disconnect");
       }
     }
   if(analogRead(connectionPin)>225 && disconnected)//disconnected, calculation
   {
     delay(1000);
     detachInterrupt(0);
     rpm = rpmcount * 60;
     connectionEnd = false;
     //RPM_cal();
     Power_cal(0, 1);
     Wattage_cal(W);
     wh_cal(W);
     //remaintime_cal(usetime);[TEST CODE]
     
     //delay(1000);
     /*lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("I Volt(V)=");
     lcd.print(Vs);
     lcd.setCursor(0, 1);
     lcd.print("O Volt(V)=");
     lcd.print(Vb);
     lcd.setCursor(0, 2);
     lcd.print("Current(A)=");
     lcd.print(I);
     lcd.setCursor(0, 3);
     lcd.print("Watt(W)=");
     lcd.print(W);*/
     
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Speed   = ");
     double Speed = rpm * 2 * 3.141592 * 0.254 * 60 / 1000;
     lcd.setCursor(10, 0);
     lcd.print(Speed);
     lcd.print(" Km/h");
     lcd.setCursor(0, 1);
     lcd.print("Voltage = ");
     lcd.setCursor(10, 1);
     lcd.print(Vs);
     lcd.print(" Volt");
     lcd.setCursor(0, 2);
     lcd.print("Power   = ");
     lcd.setCursor(10, 2);
     lcd.print(W);
     lcd.print(" W");
     lcd.setCursor(0, 3);
     lcd.print("Power_t = ");
     lcd.setCursor(10, 3);
     lcd.print(totalWs-1);
     lcd.print(" Ws");
     
     /*if(remaintime < 0)[TEST CODE]
     {
       alram();
     }*/
     Serialprint();
     rpmcount = 0;
     attachInterrupt(0, rpm_fun, FALLING);
  }
 }