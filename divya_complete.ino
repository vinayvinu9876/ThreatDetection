//#include<SoftwareSerial.h>

int FiresensorPin = A0; // select the input pin for the LDR  Fire sensor
int smokeA0 = A5;

int danger_led = 9; // Output pin for Danger LED
int okay_led = 4; // Output pin for OkayLED
int buzzer = 12; // Output pin for Buzzer

int tempPin = 1;

int dancing_lights[4] = {3,2,11,10};
int lights_length = 4;

bool motor_on = false;

int no = 0;
/*motor setup pins 5,6,7*/
#define enA 5
#define in1 6
#define in2 7

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(danger_led, OUTPUT);    //Initialize LED
  pinMode(okay_led ,OUTPUT);
  pinMode(buzzer,OUTPUT);  //Initialize Buzzer
  pinMode(smokeA0, INPUT);

  digitalWrite(okay_led , HIGH);

  for(int i=0;i<lights_length;i++){
    pinMode(dancing_lights[i],OUTPUT);
  }
  Serial.println("---------------------------------------------------------");
  Serial.println("\t WELCOME");
  Serial.println("---------------------------------------------------------");
  Serial.println();
  Serial.println("\t THREAT DETECTION AND MANAGMENT SYSTEM");

}

void loop() {
  // put your main code here, to run repeatedly:
  
  no = no+1;
  //Print Detials every ten seconds
  if(no%10==0 || no==1){
    write_report();
    
  }
 
  read_fire_sensor();
  read_temperature();
  check_gas_sensor();
  delay(1000);

}

void emergencyLightsOn(){
  
  digitalWrite(danger_led,HIGH);
  digitalWrite(okay_led,LOW);
  Serial.println("Emergency Lights On");
 
}

void emergencyLightsOff(){
 
  digitalWrite(danger_led,LOW);
  digitalWrite(okay_led,HIGH);
  Serial.println("Emergency Lights Off");
 
}


void read_fire_sensor(){
    
    bool triggered = false;
    while (analogRead(FiresensorPin) < 100)  //Read sensor value(int) and compare with threshold value 100
    {

      Serial.println("Fire Detected");
      Serial.println(analogRead(FiresensorPin));      
        if(!triggered){
          emergencyLightsOn();
          if(!motor_on){
              start_motor(); 
          }                 
          digitalWrite(buzzer,HIGH);
          triggered = true;          
       }
        let_lights_dance();
        
        delay(100);
    }
    if(triggered){
      
       digitalWrite(buzzer,LOW);
       stop_motor();
       motor_on = false;
       emergencyLightsOff();
        delay(1000);
    }

   
    
  
   
}
  
void start_motor(){
     int pwmOutput = map(100, 0, 1023, 0 , 100); // Map the potentiometer value from 0 to 255
     analogWrite(enA, pwmOutput); // Send PWM signal to L298N Enable pin
      digitalWrite(in1, HIGH);
     digitalWrite(in2, LOW);     
     motor_on = true;
     Serial.println("Switched On Motor"); 
}

void stop_motor(){
   digitalWrite(in1,LOW);
   digitalWrite(in2,LOW);
   //Serial.println("Motor off");
   motor_on = false;
   Serial.println("Switched Off Motor");
}

float get_temperature(){
        int val = analogRead(tempPin);
        float mv = ( val/1024.0)*5000; 
        float cel = mv/10;
        //float farh = (cel*9)/5 + 32;
        return cel;
}

void read_temperature(){
       
        float cel = get_temperature();

      
        if(cel>40 && !motor_on){
            
              start_motor();
            
        }

        if(cel<40 && motor_on){
           stop_motor();
        }
}


void check_gas_sensor(){
  int triggered = false;
  // Checks if it has reached the threshold value
  while(analogRead(smokeA0) > 400)
  { 
    if(!triggered){            
      tone(buzzer, 1000, 200);
      start_motor();
      emergencyLightsOn();
      triggered = true;
      Serial.println("Gas Detected");
    }
    let_lights_dance();
    
  }

  if(triggered)
  {    
    stop_motor();
    emergencyLightsOff();
    noTone(buzzer);
  }
 
  
}

void write_report(){
     Serial.println();
     Serial.println("ROUND ");
     Serial.print(no/10);
     Serial.println();

     Serial.println("TEMPRATURE = ");
     Serial.print(get_temperature());
     Serial.print("*C");
     Serial.println();

     if(motor_on)
        Serial.println("FAN ON");
     else
        Serial.println("FAN OFF");

     Serial.println("Fire Sensor Value");
     Serial.print(analogRead(FiresensorPin));
     Serial.println();

     Serial.println("Gas Sensor Value");
     Serial.print(analogRead(smokeA0));
     Serial.println();

     
}

void let_lights_dance(){
  for(int i=0;i<lights_length;i++){
    digitalWrite(dancing_lights[i],HIGH);
    delay(100);
    digitalWrite(dancing_lights[i],LOW);
  }
}
