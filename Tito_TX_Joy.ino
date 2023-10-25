//transmisor de joystick para tito

#define JOYX  A0
#define JOYY  A1
#define POTE  A2

#define CENTRO_X  410
#define CENTRO_Y  450
#define CENTRO_POTE  370

#define ALFA  0.9

void setup() 
{
Serial.begin(57600);  // put your setup code here, to run once:

}

void loop() 
{
int16_t canal1;
int16_t canal2;
int16_t canal3;


while(1)
  {
  canal1=-(((int16_t)((float)canal1*(1-ALFA) + (float)analogRead(JOYX)*(ALFA))) - CENTRO_X);
  canal2=((int16_t)((float)canal2*(1-ALFA) + (float)analogRead(JOYY)*(ALFA))) - CENTRO_Y;
  canal3=((int16_t)((float)canal3*(1-ALFA) + (float)analogRead(POTE)*(ALFA))) - CENTRO_POTE;
  Serial.print(canal1,4);
  Serial.print(',');
  Serial.print(canal2, 4);
  Serial.print(',');
  Serial.println(canal3, 4);
  delay(100);
  canal3++;
  }

}
