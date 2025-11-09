#include <SPI.h>
#include <Ethernet.h>

//Pin definitions
int Pin_MotorH_Enable = 6; // This enables (starts) the HORIZONTAL motor
int Pin_MotorH_Direction = 7; // This sets the direction of the HORIZONTAL motor
int Pin_Square_Wave_Horizontal = 14;  // A square wave is used to move the stepper motor left and right

int Pin_MotorV_Enable = 3; // This enables the VERTICAL motor
int Pin_MotorV_Direction = 4; // This sets the direction of the VERTICAL motor
int Pin_Square_Wave_Vertical = 13; // A square wave is used to move the stepper motor up and down

// Pin definitions
int LIMIT_SWITCH_LEFT = 16; // A1,When this switch is pressed, the motors stop moving (LS1)
int LIMIT_SWITCH_RIGHT = 18; // A2,When this switch is pressed, the motors stop moving (LS2)
int LIMIT_SWITCH_TOP = 17; // A3, When this switch is pressed, the motors stop moving (LS3)
int LIMIT_SWITCH_BOTTOM = 19; //A4,When this switch is pressed, the motors stop moving (LS4)


//parametres definitions of stepper motor control
int frequency = 0; // For a square wave - maximum is around 250kHz
float duration = 0; // For a square wave
long Pulses_Per_Revolution = 200; // For a square wave, this value does NOT change
int Screw_Pitch = 10; // For every 360 degrees, the motor moves 5mm, this value does NOT change
float motor_speed = 25; // For the motor movement (mm/s) - MAXIMUM SPEED IS 40mm/s and it can not be 0.1mm/s
long distance = 500; // For the motor movement (mm)
int Micro_Step = 32; // For the stepper motor driver setting, this value does NOT change
int reverse_distance = 2; // reverse motion after limit switch was pressed
float reverse_duration = 0; //running time of reverse motion after limit switch was pressed
float moveSpeed;

bool left_Pressed=false;
bool right_Pressed=false;
bool top_Pressed=false;
bool bottom_Pressed=false;
bool stopflag=true;


long debounce_Delay=50;
long stopDelay=50;

int x;
int y;
float dipPositionX;
float dipPositionY1;
float dipPositionY2;
float curingPositionX;
float curingPositionY;
float curingTime;
float dippingTime;
int dipTimes;
float dipSpeed, withdrawSpeed;
float liftHight;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,0,1);

// Initialize the Ethernet server library
EthernetServer server(49152);

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600); // Start serial communication at 9600 baud rate

    // Pin mode setup for limit switches
    pinMode(LIMIT_SWITCH_LEFT, INPUT);
    pinMode(LIMIT_SWITCH_RIGHT, INPUT);
    pinMode(LIMIT_SWITCH_TOP, INPUT);
    pinMode(LIMIT_SWITCH_BOTTOM, INPUT);

    // Pin mode setup for motors
    pinMode(Pin_MotorH_Enable, OUTPUT);
    pinMode(Pin_MotorH_Direction, OUTPUT);
    pinMode(Pin_Square_Wave_Horizontal, OUTPUT);

    pinMode(Pin_MotorV_Enable, OUTPUT);
    pinMode(Pin_MotorV_Direction, OUTPUT);
    pinMode(Pin_Square_Wave_Vertical, OUTPUT);

    //parameter calculation
    reverse_duration = reverse_distance / motor_speed * 1000; // This is calculating the reverse_duration
    frequency = motor_speed * Pulses_Per_Revolution * Micro_Step / Screw_Pitch; // This is calculating the FREQUENCY
    duration = distance / motor_speed * 1000; // This is calculating the DURATION


    Ethernet.init(5); //MKR ETH Shield

    // Open serial communications and wait for port to open:
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Ethernet WebServer Example");

  // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }

    // start the server
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());

  
}



void loop() {

    MessageReceive();

    int left_State=digitalRead(LIMIT_SWITCH_LEFT);
    int right_State=digitalRead(LIMIT_SWITCH_RIGHT);
    int top_State=digitalRead(LIMIT_SWITCH_TOP);
    int bottom_State=digitalRead(LIMIT_SWITCH_BOTTOM);


    //left limit switch state
    if(left_State==LOW&&!left_Pressed){
       
       delay(debounce_Delay);
       left_State=digitalRead(LIMIT_SWITCH_LEFT);
       swichcheck();
       left_Pressed=true;
       Left_back(Pin_MotorH_Enable,Pin_Square_Wave_Horizontal,debounce_Delay,reverse_duration,Pin_MotorH_Direction,frequency,duration);
       
  }
    if(left_State==HIGH){
      left_Pressed=false;
  }

   //right limit switch state
    if(right_State==LOW&&!right_Pressed){
       
       delay(debounce_Delay);
       right_State=digitalRead(LIMIT_SWITCH_RIGHT);
       swichcheck();
       right_Pressed=true;
       Right_back(Pin_MotorH_Enable,Pin_Square_Wave_Horizontal,debounce_Delay,reverse_duration,Pin_MotorH_Direction,frequency,duration);
       
  }
    if(right_State==HIGH){
      right_Pressed=false;
  }

  //top limit switch state
    if(top_State==LOW&&!top_Pressed){
      delay(debounce_Delay);
      top_State=digitalRead(LIMIT_SWITCH_TOP);
      swichcheck();
      top_Pressed=true;
      Top_back(Pin_MotorV_Enable,Pin_Square_Wave_Vertical,debounce_Delay,reverse_duration,Pin_MotorV_Direction,frequency,duration);
  }
    if(top_State==HIGH){
      top_Pressed=false;

  }

  //bottom limit switch state
    if(bottom_State==LOW&&!bottom_Pressed){
      delay(debounce_Delay);
      bottom_State=digitalRead(LIMIT_SWITCH_BOTTOM);
      swichcheck();
      bottom_Pressed=true;
      Bottom_back(Pin_MotorV_Enable,Pin_Square_Wave_Vertical,debounce_Delay,reverse_duration,Pin_MotorV_Direction,frequency,duration);
  }
    if(bottom_State==HIGH){
      bottom_Pressed=false;
  }

}

void moveRight() {
    digitalWrite(Pin_MotorH_Enable, LOW); // Enable the horizontal motor
    digitalWrite(Pin_MotorH_Direction, LOW); // Set direction 
    tone(Pin_Square_Wave_Horizontal, frequency, duration); // Move the motor
}

void moveLeft() {
    digitalWrite(Pin_MotorH_Enable, LOW); // Enable the vertical motor
    digitalWrite(Pin_MotorH_Direction, HIGH); // Set direction
    tone(Pin_Square_Wave_Horizontal, frequency, duration); // Move the motor
}

void moveTop() {
    digitalWrite(Pin_MotorV_Enable, LOW); // Enable the horizontal motor
    digitalWrite(Pin_MotorV_Direction, HIGH); // Set direction
    tone(Pin_Square_Wave_Vertical, frequency, duration); // Move the motor
}

void moveBottom() {
    digitalWrite(Pin_MotorV_Enable, LOW); // Enable the vertical motor
    digitalWrite(Pin_MotorV_Direction, LOW); // Set direction 
    tone(Pin_Square_Wave_Vertical, frequency, duration); // Move the motor
}


void Stop() {
    stopflag=false;
    digitalWrite(Pin_MotorH_Enable, HIGH); // Enable the horizontal motor
    digitalWrite(Pin_MotorV_Enable, HIGH); // Enable the vertical motor
    delay(stopDelay); // Wait for the movement to complete
    noTone(Pin_Square_Wave_Vertical); // Stop the tone
    noTone(Pin_Square_Wave_Horizontal); // Stop the tone

}
    


void swichcheck( ){
  if(digitalRead(LIMIT_SWITCH_LEFT) == LOW) {
    Serial.println("LEFT LIMIT PRESSED");//output "LEFT LIMIT" when left limit switch is pressed
  }
  if(digitalRead(LIMIT_SWITCH_RIGHT) == LOW) {
    Serial.println("RIGHT LIMIT PRESSED");//output "RIGHT LIMIT" when right limit switch is pressed
  }
  if(digitalRead(LIMIT_SWITCH_TOP) == LOW) {
    Serial.println("TOP LIMIT PRESSED");//output "TOP LIMIT" when top limit switch is pressed
  }
  if(digitalRead(LIMIT_SWITCH_BOTTOM) == LOW) {
    Serial.println("BOTTOM LIMIT PRESSED");//output "BOTTOM LIMIT" when bottom limit switch is pressed
  }

}

void Enable_motor(int Motor_ID, int Status){

  digitalWrite(Motor_ID, Status); // ENABLE is LOW active
  
} 


//motor direction set function   
void Set_Motor_Direction(int Motor_ID, int Direction){

  digitalWrite(Motor_ID, Direction);
  
}

void Left_back(int Pin_MotorH_Enable, int Pin_Square_Wave_Horizontal, int DebounceDelay, int reverse_duration, int Pin_MotorH_Direction, int frequency,  int duration)
{
    Enable_motor(Pin_MotorH_Enable, 1);
    noTone(Pin_Square_Wave_Horizontal);
    delay(DebounceDelay); // This delays the number of times the button debounces after it has been pressed so that we get only one value when displayed on the serial monitor
    //back
    Enable_motor(Pin_MotorH_Enable, 0);
    Set_Motor_Direction(Pin_MotorH_Direction, 0);
    tone(Pin_Square_Wave_Horizontal, frequency, reverse_duration);
}

void Right_back(int Pin_MotorH_Enable, int Pin_Square_Wave_Horizontal, int DebounceDelay, int reverse_duration, int Pin_MotorH_Direction, int frequency, int duration)
{
    Enable_motor(Pin_MotorH_Enable, 1);
    noTone(Pin_Square_Wave_Horizontal);
    delay(DebounceDelay); // This delays the number of times the button debounces after it has been pressed so that we get only one value when displayed on the serial monitor
    //back
    Enable_motor(Pin_MotorH_Enable, 0);
    Set_Motor_Direction(Pin_MotorH_Direction, 1);
    tone(Pin_Square_Wave_Horizontal, frequency, reverse_duration);
}

void Top_back(int Pin_MotorV_Enable, int Pin_Square_Wave_Vertical, int DebounceDelay, int reverse_duration, int Pin_MotorV_Direction, int frequency,  int duration)
{
    Enable_motor(Pin_MotorV_Enable, 1);
    noTone(Pin_Square_Wave_Vertical);
    delay(DebounceDelay); // This delays the number of times the button debounces after it has been pressed so that we get only one value when displayed on the serial monitor
    //back
    Enable_motor(Pin_MotorV_Enable, 0);      
    Set_Motor_Direction(Pin_MotorV_Direction, 0);
    tone(Pin_Square_Wave_Vertical, frequency, reverse_duration);
}


void Bottom_back(int Pin_MotorV_Enable, int Pin_Square_Wave_Vertical, int DebounceDelay, int reverse_duration, int Pin_MotorV_Direction, int frequency,  int duration)
{
    Enable_motor(Pin_MotorV_Enable, 1);
    noTone(Pin_Square_Wave_Vertical);
    delay(DebounceDelay); // This delays the number of times the button debounces after it has been pressed so that we get only one value when displayed on the serial monitor
    //back
    Enable_motor(Pin_MotorV_Enable, 0);      
    Set_Motor_Direction(Pin_MotorV_Direction, 1);
    tone(Pin_Square_Wave_Vertical, frequency, reverse_duration);

}

void printPatameters( float distance, float speed, float moveTime){
     Serial.println("distance(mm):");
     Serial.println(distance);
     Serial.println("speed(mm/s):");
     Serial.println(speed);
     Serial.println("moveTime(ms):");
     Serial.println(moveTime);
}

void HomeY() {
    moveTop();
}

void HomeX() {
    moveRight();
}

void Dip(){
  Serial.println("dippingTime:");
  Serial.println(dippingTime);
  float h=abs(x-dipPositionX);
  float v=abs(y-dipPositionY1);
  float dx=curingPositionX-dipPositionX;
  float moveFrequency=moveSpeed * Pulses_Per_Revolution * Micro_Step / Screw_Pitch;
  float moveDurationH= h*1000/moveSpeed;
  float moveDurationV= v*1000/moveSpeed;
  float dipDuration=(dipPositionY2-dipPositionY1)*1000/dipSpeed;
  float dipFrequency=dipSpeed * Pulses_Per_Revolution * Micro_Step / Screw_Pitch;
  float withdrawDuration=(dipPositionY2-dipPositionY1)*1000/withdrawSpeed;
  float withdrawFrequency=withdrawSpeed * Pulses_Per_Revolution * Micro_Step / Screw_Pitch;
  float goCuringDuration=dx*1000/moveSpeed;
  float todip=dipPositionY1-liftHight;
  float tooven=curingPositionY-liftHight;
  float todipDuration=todip*1000/moveSpeed;
  float toovenDuration=tooven*1000/moveSpeed;
  float backYduration=dipPositionY1*1000/moveSpeed;
  float backXduration=dipPositionX*1000/moveSpeed;
  bool stopflag=true;
  
  if (x>dipPositionX){
     digitalWrite(Pin_MotorH_Enable, LOW); 
     digitalWrite(Pin_MotorH_Direction, LOW); 
     tone(Pin_Square_Wave_Horizontal, moveFrequency, moveDurationH); // Move the motor
     x=dipPositionX;
  }else{
    digitalWrite(Pin_MotorH_Enable, LOW); 
    digitalWrite(Pin_MotorH_Direction, HIGH); 
    tone(Pin_Square_Wave_Horizontal, moveFrequency, moveDurationH); // Move the motor
    x=dipPositionX;
  }
  delay(moveDurationH);
 if (y>dipPositionY1){
     digitalWrite(Pin_MotorV_Enable, LOW); // Enable the vertical motor
     digitalWrite(Pin_MotorV_Direction, HIGH); 
     tone(Pin_Square_Wave_Vertical, moveFrequency, moveDurationV); // Move the motor
     delay(moveDurationV);
     y=dipPositionY1;
  }else{
    digitalWrite(Pin_MotorV_Enable, LOW); // Enable the vertical motor
    digitalWrite(Pin_MotorV_Direction,LOW); 
    tone(Pin_Square_Wave_Vertical, moveFrequency, moveDurationV); // Move the motor
    delay(moveDurationV);
    y=dipPositionY1;
  }

  int i=1;
    for(i=1;i<=dipTimes&&stopflag;i++){
      digitalWrite(Pin_MotorV_Enable, LOW); 
      digitalWrite(Pin_MotorV_Direction, LOW); 
      tone(Pin_Square_Wave_Vertical, dipFrequency, dipDuration);//go to dip start position
      delay(dipDuration);
      noTone(Pin_Square_Wave_Vertical);
      delay(dippingTime);
      digitalWrite(Pin_MotorV_Enable, LOW); 
      digitalWrite(Pin_MotorV_Direction, HIGH); 
      tone(Pin_Square_Wave_Vertical, withdrawFrequency, withdrawDuration);//dip process
      delay(withdrawDuration);
      noTone(Pin_Square_Wave_Vertical);
      digitalWrite(Pin_MotorV_Enable, LOW); 
      digitalWrite(Pin_MotorV_Direction, HIGH); 
      tone(Pin_Square_Wave_Vertical, moveFrequency, todipDuration);//withdraw process
      delay(todipDuration);
      noTone(Pin_Square_Wave_Vertical);
      digitalWrite(Pin_MotorH_Enable, LOW); 
      digitalWrite(Pin_MotorH_Direction, HIGH); 
      tone(Pin_Square_Wave_Horizontal, moveFrequency, goCuringDuration);//go to a user-dicided height and go curing
      delay(goCuringDuration);
      noTone(Pin_Square_Wave_Horizontal);
      digitalWrite(Pin_MotorV_Enable, LOW); 
      digitalWrite(Pin_MotorV_Direction, LOW); 
      tone(Pin_Square_Wave_Vertical, moveFrequency, toovenDuration);
      delay(toovenDuration);
      noTone(Pin_Square_Wave_Vertical);
      delay(curingTime);
      digitalWrite(Pin_MotorV_Enable, LOW); 
      digitalWrite(Pin_MotorV_Direction, HIGH); 
      tone(Pin_Square_Wave_Vertical, moveFrequency, toovenDuration);//curing process
      delay(toovenDuration);
      noTone(Pin_Square_Wave_Vertical);
      digitalWrite(Pin_MotorH_Enable, LOW); 
      digitalWrite(Pin_MotorH_Direction, LOW); 
      tone(Pin_Square_Wave_Horizontal, moveFrequency, goCuringDuration); 
      delay(goCuringDuration);
      noTone(Pin_Square_Wave_Horizontal);//go back to dip start position
      digitalWrite(Pin_MotorV_Enable, LOW); 
      digitalWrite(Pin_MotorV_Direction, LOW); 
      tone(Pin_Square_Wave_Vertical, moveFrequency, todipDuration);
      delay(todipDuration);
      noTone(Pin_Square_Wave_Vertical);
    }

    delay(1000);
    //Go back to TOP
    digitalWrite(Pin_MotorV_Enable, LOW); 
    digitalWrite(Pin_MotorV_Direction, HIGH); 
    tone(Pin_Square_Wave_Vertical, moveFrequency, backYduration);
    delay(backYduration);
    noTone(Pin_Square_Wave_Vertical);
    //Go back to RIGHT
    digitalWrite(Pin_MotorH_Enable, LOW); 
    digitalWrite(Pin_MotorH_Direction, LOW); 
    tone(Pin_Square_Wave_Horizontal, moveFrequency, backXduration); 
    delay(backXduration);
    noTone(Pin_Square_Wave_Horizontal);
    x=0;
    y=0;
    stopflag=true;
}

void MessageReceive() {
  EthernetClient client = server.available();
  String receivedMessage = "";  // Initialize an empty String to store the received message

  if (client) {
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
              char message = client.read();
              Serial.write(message);  
              receivedMessage += message;  // Append the read character to the String
            }
          }
        }
        
        int firstCommaIndex= receivedMessage.indexOf(',');
        int secondCommaIndex= receivedMessage.indexOf(',', firstCommaIndex+1);
        int thirdCommaIndex= receivedMessage.indexOf(',', secondCommaIndex+1);
        int fourthCommaIndex = receivedMessage.indexOf(',', thirdCommaIndex + 1);
        int fifthCommaIndex = receivedMessage.indexOf(',', fourthCommaIndex + 1);
        int sixthCommaIndex = receivedMessage.indexOf(',', fifthCommaIndex + 1);
        int seventhCommaIndex = receivedMessage.indexOf(',', sixthCommaIndex + 1);
        int eighthCommaIndex = receivedMessage.indexOf(',', seventhCommaIndex + 1);
        int ninthCommaIndex = receivedMessage.indexOf(',', eighthCommaIndex + 1);
        int tenthCommaIndex = receivedMessage.indexOf(',', ninthCommaIndex + 1);
        int eleventhCommaIndex = receivedMessage.indexOf(',', tenthCommaIndex + 1);
        int twelfthCommaIndex = receivedMessage.indexOf(',', eleventhCommaIndex + 1);
        
        float direction,distance,speed;
        String cmd= receivedMessage.substring(0,firstCommaIndex);
        String directionStr=receivedMessage.substring(firstCommaIndex+1,secondCommaIndex);
        direction= directionStr.toInt();
        String distanceStr=receivedMessage.substring(secondCommaIndex+1,thirdCommaIndex);
        distance= distanceStr.toFloat();
        String speedStr=receivedMessage.substring(thirdCommaIndex + 1, fourthCommaIndex);
        speed= speedStr.toFloat();  
        
        float moveTime=distance*1000.0/speed;
        float newFrequency = speed * Pulses_Per_Revolution * Micro_Step / Screw_Pitch;
        
        if(cmd=="HomeY"){
          HomeY();
          Serial.print("CMD: ");
          Serial.println(cmd);
          Serial.println("'HomeY' Excuted ");
          y=0;
          Serial.println("Vertical Position:");
          Serial.println(y);
        }
        else if(cmd=="HomeX"){
          HomeX();
          Serial.print("CMD: ");
          Serial.println(cmd);
          Serial.println("'HomeX' Excuted ");
          x=0;
          Serial.print("Horizental Position:");
          Serial.println(x);
        }
        
        else if(cmd=="Move"){
                if(direction==0){          
                  if(distance+x>263){
                  Serial.println("Over Limitaion, Move failed");
                }else{
                  digitalWrite(Pin_MotorH_Enable, LOW); // Enable the vertical motor
                  digitalWrite(Pin_MotorH_Direction, HIGH); // Set direction (adjust as needed)
                  tone(Pin_Square_Wave_Horizontal, newFrequency, moveTime); // Move the motor
                  x+=distance;
                  Serial.println("'Move Left' Excuted ");
                  printPatameters(distance, speed, moveTime);
                  client.write("Horizental Position:");
                  Serial.print("Horizental Position:");
                  Serial.println(x);
                }
          }
          else if(direction==1){
               if(distance>y){
                  Serial.println("Over Limitaion, Move failed");
                }else{
                  digitalWrite(Pin_MotorV_Enable, LOW); 
                  digitalWrite(Pin_MotorV_Direction, HIGH);
                  tone(Pin_Square_Wave_Vertical, newFrequency, moveTime); 
                  y-=distance;
                  Serial.println("'Move Top' Excuted ");
                  printPatameters(distance, speed, moveTime);
                  Serial.print("Vertical Position:");
                  Serial.println(y);
                }
            }
            else if(direction==2){
                  if(distance>x){
                  Serial.println("Over Limitaion, Move failed");
                  }else{
                  digitalWrite(Pin_MotorH_Enable, LOW); // Enable the vertical motor
                  digitalWrite(Pin_MotorH_Direction, LOW); // Set direction (adjust as needed)
                  tone(Pin_Square_Wave_Horizontal, newFrequency, moveTime); // Move the motor
                  x-=distance;
                  Serial.println("'Move Right' Excuted ");
                  printPatameters(distance, speed, moveTime);
                  Serial.print("Horizental Position:");
                  Serial.println(x);
                }
              }
              else if(direction==3){
                    if(distance+y>362){
                    Serial.println("Over Limitaion, Move failed");
                  }else{
                  digitalWrite(Pin_MotorV_Enable, LOW); // Enable the vertical motor
                  digitalWrite(Pin_MotorV_Direction, LOW); // Set direction (adjust as needed)
                  tone(Pin_Square_Wave_Vertical, newFrequency, moveTime); // Move the motor
                  y+=distance;
                  Serial.println("'Move Bottom' Excuted ");
                  printPatameters(distance, speed, moveTime);
                  Serial.print("Vertical Position:");
                  Serial.println(y);
              }
          }
          
        }
        else if(cmd=="Stop"){
          Stop();
        }else if(cmd=="Dip"){
          Dip();
        }else if(cmd=="Submit"){
        String moveSpeedStr=receivedMessage.substring(firstCommaIndex+1,secondCommaIndex);
        moveSpeed= moveSpeedStr.toFloat();
        String dipSpeedStr=receivedMessage.substring(secondCommaIndex+1,thirdCommaIndex);
        dipSpeed= dipSpeedStr.toFloat();
        String withdrawSpeedStr=receivedMessage.substring(thirdCommaIndex + 1, fourthCommaIndex);
        withdrawSpeed= withdrawSpeedStr.toFloat();  
        String DipXStr = receivedMessage.substring(fourthCommaIndex + 1, fifthCommaIndex);
        dipPositionX=DipXStr.toFloat();
        String DipY1Str = receivedMessage.substring(fifthCommaIndex + 1, sixthCommaIndex);
        dipPositionY1=DipY1Str.toFloat();
        String DipY2Str = receivedMessage.substring(sixthCommaIndex + 1, seventhCommaIndex);
        dipPositionY2=DipY2Str.toFloat();
        String CuringXStr = receivedMessage.substring(seventhCommaIndex + 1, eighthCommaIndex);
        curingPositionX=CuringXStr.toFloat();
        String CuringYStr = receivedMessage.substring(eighthCommaIndex+1,ninthCommaIndex);
        curingPositionY=CuringYStr.toFloat();
        String dipTimesStr = receivedMessage.substring(ninthCommaIndex+1,tenthCommaIndex);
        dipTimes=dipTimesStr.toInt();
        String curingTimeStr = receivedMessage.substring(tenthCommaIndex+1,eleventhCommaIndex);
        curingTime=curingTimeStr.toFloat();
        String dippingTimeStr = receivedMessage.substring(eleventhCommaIndex+1,twelfthCommaIndex);
        dippingTime=dippingTimeStr.toFloat();
        String liftHightStr = receivedMessage.substring(twelfthCommaIndex+1);
        liftHight=liftHightStr.toFloat();


        Serial.println("dipSpeed:");
        Serial.println(dipSpeed);
        Serial.println("withdrawSpeed:");
        Serial.println(withdrawSpeed);
        Serial.println("dipPositionX:");
        Serial.println(dipPositionX);
        Serial.println("dipPositionY1:");
        Serial.println(dipPositionY1);
        Serial.println("dipPositionY2:");
        Serial.println(dipPositionY2);
        Serial.println("curingPositionX:");
        Serial.println(curingPositionX);
        Serial.println("curingPositionY:");
        Serial.println(curingPositionY);
        Serial.println("dipTimes :");
        Serial.println(dipTimes);
        Serial.println("'Dip position parameters' collected ");
        
        }
}


