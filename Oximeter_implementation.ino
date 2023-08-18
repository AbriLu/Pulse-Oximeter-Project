#include <LiquidCrystal.h>
#include <string.h>

//#include <iostream>
byte customChar[8] = {  //creates the heart carater to print on the LCD display.
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // initialize the library with the numbers of the interface pins

int IR_Led_Pin = 10;
int Red_Led_Pin = 13;
//button variables
int button3 = 8;
int buttonState3 = 0;       
int lastButtonState3 = 0;
int val = 0;
//button variables end here
/////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  pinMode(button3,INPUT);
  pinMode(9,INPUT);
  pinMode(IR_Led_Pin,OUTPUT);
  pinMode(Red_Led_Pin,OUTPUT);
  pinMode(A1,INPUT);
  pinMode(A0,INPUT);
  lcd.begin(16, 2);
  lcd.createChar(0, customChar); 
  //lcd.write((byte)0);  // print the custom char at (2, 0) 
  lcd.clear();
}
/////////////////////////////////////////////////////////////
//buttons variables
int buttonState2 = 0;        
int lastButtonState2 = 0; 
int buttonState1 = 0;         
int lastButtonState1 = 0; 
int button2_navigate = 9;
int button1 = 10;
int navigate_count = 0;
int navigate_val = 0;
//buttons variables end here
void Red_LED_On(){//RED / IR LEDs control function
  digitalWrite(Red_Led_Pin,HIGH);
}
void Red_LED_Off(){//RED / IR LEDs control function
  digitalWrite(Red_Led_Pin,LOW);
}
void IR_LED_On(){//RED / IR LEDs control function
  digitalWrite(IR_Led_Pin,HIGH);
}
void IR_LED_Off(){//RED / IR LEDs control function
  digitalWrite(IR_Led_Pin,LOW);
}
//RED and IR LEDs control functions ends
float voltage_level(){//Photodiode voltage measurements controls
  return (analogRead(A1));
}
//Photodiode voltage measurements controls ends
class Time {  //class that enables the oximeteer device to keep track of various processes
  float Start = 0;
  public:
    void set_start_time(){Start = _time();}
    float _time(){return millis()/1000.0;}
    float duration (); 
};
float Time::duration(){  //function from the Time class that calcu;ates the duration of a process from the set start time using the set_start_time() function of the Time class.
  return (_time() - Start);
}
void waiting_animation(){  //Waiting animation when no finger is detected
  float detection_level = 300.0;
  while(voltage_level()<=detection_level){
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("No finger");
    lcd.setCursor(3,1);
    lcd.print("detected");
    delay(200);
    if(voltage_level()>detection_level){break;}
    lcd.print(".");
    delay(200);
    if(voltage_level()>detection_level){break;}
    lcd.print("..");
    delay(200);
    if(voltage_level()>detection_level){break;}
    lcd.print("...");
  }
  lcd.clear();
}
void calculate_heart_rate(){
  Time my;
  int count2 = 0;
  Time atime;
  Red_LED_Off();
  IR_LED_On();
  my.set_start_time();
  float my_ir_level = 0.0;
  int count1 = 0;
  float avg_h = 0;
  float sum_h = 0;
  float my_h[20];
  while(analogRead(A0) > 1.5){ }
  while(my.duration() < 2.0)
  {
    atime.set_start_time();
    while(atime.duration() < 0.01)
    {
      while(analogRead(A0) > 1.5){ }
      my_ir_level = analogRead(A0);
      my_h[count1] = my_ir_level;
      sum_h += my_h[count1];
      count1++;
    }
    while(analogRead(A0) > 1.5){ }
    avg_h = sum_h/count1;
    Serial.println(" ");
    Serial.println(avg_h*10);
    if (my.duration() > 3.0 && avg_h*10 > 3.0)
    {
      count2++;
    }
    lcd.setCursor(0,0);
    //lcd.print(avg_h);
    sum_h = 0;
    avg_h = 0;
    count1 = 0;
  }                                                                                                                                                                                                                                   
  lcd.setCursor(10,0);
  lcd.print("BPM:"); 
  lcd.print(count2);   //heart rate printing
}
class extreme_val{ //allows one to store the minimum or maximum value of all the values passed trough the function set_max() and set_min().
  public:
    float maximum = 0.0;
    float mininmum = 1024.0;
    void set_max(float max_input)
    {
      if (max_input > maximum){maximum = max_input;}
    }
    void set_min(float min_input)
    {
      if (min_input < mininmum){mininmum = min_input;}
    }
};
void calculate_SpO2(){//Function that calculates the SpO2%
  float R = 0.0;
  float SpO2 = 0.0;
  Time My;
  Time My_Red;
  Time My_IR;
  My.set_start_time();
  IR_LED_Off();
  Red_LED_On();
  float reading = 0;
  float Red[2000];
  float IR[2000];
  int count = 0;
  float red_sum = 0;
  float IR_sum = 0;
  float avg_red = 0;
  float avg_IR = 0;
  extreme_val red;
  extreme_val ir;
  float end_time = 7.0;          //time it takes to measure SpO2
  while(My.duration() < end_time)
  {
    ///////////////////////////////////// RED PART
    My_Red.set_start_time();
    IR_LED_Off();
    Red_LED_On();
    while(My_Red.duration()<=0.02)
    {
      if (voltage_level() < 300.0)
      {
        avg_red = 0;
        red_sum = 0;
        waiting_animation();
        My.set_start_time();
      }
      Red[count] = voltage_level();
      red_sum += Red[count];
      //Serial.print(Red[count]);
      lcd.setCursor(0,0);
      lcd.print("Time: ");
      lcd.setCursor(5,0);
      lcd.print(My.duration());
      count++;
    }
    avg_red = red_sum/count; //IR average value
    red.set_max(avg_red);    // calculating min and max
    if (My.duration() > 4.0)
    {
      red.set_min(avg_red);
    }
    //lcd.setCursor(7,0);
    //lcd.print(avg_red);
    Serial.print(avg_red);
    count = 0;
    avg_red = 0;
    red_sum = 0;
     Serial.print(", ");
    ///////////////////////////////////// IR PART
    My_IR.set_start_time();
    Red_LED_Off();
    IR_LED_On();
    while(My_IR.duration()<=0.02)
    {
      if (voltage_level() < 300.0)
      {
        avg_IR = 0;
        IR_sum = 0;
        waiting_animation();
        My.set_start_time();
      }
      IR[count] = voltage_level();
      IR_sum += IR[count];
      //Serial.print(IR[count]);
      lcd.setCursor(0,0);
      lcd.print("Time: ");
      lcd.setCursor(5,0);
      lcd.print(My.duration());
      count++;
    }
    avg_IR = IR_sum/count;
    ir.set_max(avg_IR);      // calculating min and max
    if (My.duration() > 4.0)
    {
      ir.set_min(avg_IR);
    }
    //lcd.setCursor(7,1);
    //lcd.print(avg_IR);   //IR average value
    Serial.print(avg_IR);
    Serial.println(" ");
    count = 0;
    avg_IR = 0;
    IR_sum = 0;
  }
  ir.maximum;    //resulting variables
  ir.mininmum;
  red.maximum;
  red.mininmum;
  R = ((red.maximum-red.mininmum)/red.mininmum )/((ir.maximum-ir.mininmum)/ir.mininmum);   //R RATIO HERE!!!!
  lcd.setCursor(0,1);
  lcd.print("SpO2%: ");
  SpO2 =-(19.49*R*R)-(10.47*R)+107.9;
  lcd.setCursor(7,1);
  lcd.print(SpO2);
  lcd.print("%");
  calculate_heart_rate();
}


void loop() 
{
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Oximeter code starts here
  ////////////////////////////////////////////////////////////////
  buttonState3 = digitalRead(button3);
  if (buttonState3 != lastButtonState3) { //CLICKING button3
    if (buttonState3 == HIGH) {
      lcd.clear();
      calculate_SpO2();
      //calculate_heart_rate();
      while(digitalRead(button3) == HIGH)
      {
        while(1)
        {
          if(digitalRead(button3) == LOW)
          {
            break;
          }
        }
      }
    }
    delay(50);
  }
  lastButtonState3 = buttonState3;
  //CLICKING button3 ends
  ////////////////////////////////////////////////////////////////
}
