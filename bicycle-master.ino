//2015_8_19
//Run&Run Team, Sin Yu Seung
//Master
//ATmega328P-PU on Board Program
//for more info - http://blog.naver.com/sinu8361

#include <LiquidCrystal.h> //for Lcd display
LiquidCrystal lcd(2, 3, 16, 15, 14, 13); //pin number here
#include <Wire.h> //for I2C
int resivetotalWh = 0;
int transmittusetime = 0;
int connectionbottonPin = 8; //connection button Pin
int connectionPin = 3; //connection sensing Pin
bool connectionEnd = true;//for mode sweeping
bool disconnected = false;
int oneHPin = 5; //use time set
int twoHPin = 6;
int treeHPin = 7;
int moneyperhour = 1000;//for money cal
int money = 0;
int moneyPin = 11;
int usetime = 0; //for remain time cal
int remaintime = 0;
int alramPin = 12; //pin for alram spk
int relayPin = 4;//pin for relay
int solenoideonePin = 9;
int solenoidetwoPin = 10;
void Read_Data() //I2C comunication
{
  Wire.requestFrom(1, 1);
  delay(500);
  if(Wire.available());
  {
    resivetotalWh = Wire.read(); //resive the data
  }
}
void Write_Data(int transmittusetime) //send the time data I2C
{
  Wire.beginTransmission(1);
  Wire.write(transmittusetime);
  delay(500);
  Wire.endTransmission();
}
void remaintime_cal(int usetime)//countdown function
{
  usetime = millis()/60000;
  remaintime = transmittusetime - usetime;
}
void alram()//for alram spk
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
   pinMode(oneHPin, INPUT);//set the botto pin
   pinMode(twoHPin, INPUT);
   pinMode(treeHPin, INPUT);
   pinMode(moneyPin, INPUT);
   pinMode(connectionbottonPin, INPUT);
   pinMode(alramPin, OUTPUT);//set the spk out
   pinMode(relayPin, OUTPUT);//set the relay out
   pinMode(solenoideonePin, OUTPUT);//set the connetion solenoide out
   pinMode(solenoidetwoPin, OUTPUT);
   lcd.begin(20, 4);
   Serial.begin(9600);//Serial test
   Wire.begin();
 }
void loop()
{
  if(analogRead(connectionPin)<225 && !disconnected)//ready to disconnection
  {
    lcd.clear();
    lcd.print("Run&Run Team");
    lcd.setCursor(0, 1);
    lcd.print("Bicycle Rent Service");
    lcd.setCursor(0, 2);
    lcd.print("Press Start button");
    lcd.setCursor(0, 3);
    lcd.print("to start");
    delay(100);
    if(digitalRead(oneHPin) == HIGH || digitalRead(twoHPin) == HIGH || (treeHPin) == HIGH)
    {
      delay(1000);
      for(int a=0; a < 60; a++)
      {
        lcd.clear();
        lcd.print("Please set the time");
        lcd.setCursor(0, 1);
        lcd.print("Press Start button");
        delay(100);
        if(digitalRead(oneHPin) == HIGH)
        {
          transmittusetime = 1;
          break;
        }
        else if(digitalRead(twoHPin) == HIGH)
        {
          transmittusetime = 2;
          break;
        }
        else if(digitalRead(treeHPin) == HIGH)
        {
          transmittusetime = 3;
          break;
        }
      }
      delay(100);
      if(transmittusetime == !0)
      {
        for(int b = 0; b < 60; b++)
        {
          lcd.clear();
          lcd.print("Please insert Money");
          lcd.setCursor(0, 1);
          lcd.print("Money = ");
          money = moneyperhour * transmittusetime;
          lcd.print(money);
          lcd.setCursor(0, 2);
          lcd.print("Press Money button");
          delay(100);
          if(digitalRead(moneyPin) == HIGH)
          {
            lcd.clear();
            lcd.print("Thanks for using");
            lcd.setCursor(0, 1);
            lcd.print("Disconnection");
            lcd.setCursor(0, 2);
            lcd.print("Process");
            lcd.setCursor(0, 3);
            lcd.print("Plese wait a sec");
            Write_Data(transmittusetime);//set time
            digitalWrite(solenoideonePin, HIGH);
            delay(1000);
            digitalWrite(solenoideonePin, LOW);
            digitalWrite(relayPin, LOW);
            delay(1000);
            lcd.clear();
            lcd.print("You can disconnect");
            disconnected = true;
            break;
          }
        }
      }
    }
  }
  if(analogRead(connectionPin)>225 && disconnected)//disconnected, calculation
  {
    connectionEnd = false;
    //remaintime_cal(usetime);
    lcd.clear();
    lcd.print("Run&Run Team");
    lcd.setCursor(0, 1);
    lcd.print("Circuit&Sketch made");
    lcd.setCursor(0, 2);
    lcd.print("by Sin Yu Seung");
    lcd.setCursor(0, 3);
    delay(100);
    /*if(remaintime < 0)
    {
      alram();
    }*/
  }
  if((analogRead(connectionPin)<255 && !connectionEnd) && disconnected)//sens the connection
  {
    lcd.clear();
    lcd.print("Press");
    lcd.setCursor(0, 1);
    lcd.print("Connection botton");
    delay(100);
    if(digitalRead(connectionbottonPin) == HIGH)//sens confurmed
    {
      lcd.clear();
      lcd.print("Connection process");
      lcd.setCursor(0,1);
      lcd.print("Please Wait a sec");
      digitalWrite(solenoidetwoPin, HIGH);
      delay(1000);
      digitalWrite(solenoidetwoPin, LOW);
      digitalWrite(relayPin, HIGH);
      Read_Data();//read data from G_module
      delay(1000);
      lcd.clear();
      lcd.print("Your Electircity is");
      lcd.setCursor(0, 1);
      lcd.print(resivetotalWh-1);
      lcd.print("Ws");
      delay(1000);
      lcd.setCursor(0, 2);
      lcd.print("Thanks for using!!");
      delay(3000);
      resivetotalWh = 0;//reset value
      usetime = 0;
      remaintime = 0;
      transmittusetime = 0;
      connectionEnd = true;
      disconnected = false;
    }
  }
}