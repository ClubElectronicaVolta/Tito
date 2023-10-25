// Programa para manejar a Tito, un movil con dos motores (traccion diferencial motor izquierdo y motor derecho)
// V 0.1
// Se maneja con un joystick de pc conectado a un arduino y transmitiendo codigos via un NRF24L01

/*
1-3 x+ 13,5k x- 95k x centro 56,7k
1-6 y- 10,2k y+ 86k y centro 46,5k
1-13 thro 10k hacia delante 95k hacia atras
2-4 main trigguer
*/

# include <Servo.h>

#define PWM_DER 5   //los pines 5 y 6 en el nano tienen mas frecuencia que los otros
#define PWM_IZQ 6
#define DIR_DER 3   //pines que determinan la direccion del motor
#define DIR_IZQ 4
#define AVANZA    0   // 0 va para adelante, 1 va para atras...por definicion...puede ser alreves
#define RETROCEDE 1
#define LED_ON    0   //que tengo que poner para que el led se prenda
#define LED_OFF   1   //que tengo que poner para que el led se apague
#define LED_R     10   // pines donde esta conectado el led rgb
#define LED_G     8
#define LED_B     9
#define SERVO     7

#define TIMEOUT_UART  5
#define TIMEOUT 1000  //1 segundo=1000 milisegundos

#define DECODE_NEC    // este es el protocolo que usa el control remoto de la basura

#define BUFFER_UART 10
char recibidos[BUFFER_UART]; //buffer de caracteres recibidos
char caracter;      //caracter recibido
char indice=0;      //indice de caracteres recibidos

Servo servocamara;  //creo objeto servo

void setup() 
{
Serial.begin(57600);      //abro el puerto serial

pinMode(LED_R, OUTPUT);     //inicializo todo como salida
pinMode(LED_G, OUTPUT);
pinMode(LED_B, OUTPUT);
pinMode(PWM_DER, OUTPUT);
pinMode(PWM_IZQ, OUTPUT);
pinMode(DIR_DER, OUTPUT);
pinMode(DIR_IZQ, OUTPUT);
servocamara.attach(SERVO);
servocamara.write(90);

}

//-------- Seteo la velocidad y direccion de los motores -----------
void setea_velocidad(int16_t vel_der,int16_t vel_izq)
{
if(vel_der>=0) digitalWrite(DIR_DER,AVANZA);
else {vel_der= 255+vel_der; digitalWrite(DIR_DER,RETROCEDE);}
if(vel_izq>=0) digitalWrite(DIR_IZQ,AVANZA);
else {vel_izq= 255+vel_izq; digitalWrite(DIR_IZQ,RETROCEDE);}
if(vel_der>255) vel_der=255;
if(vel_izq>255) vel_izq=255;
analogWrite(PWM_DER, vel_der);
analogWrite(PWM_IZQ, vel_izq);

}

// -------------------------- Lazo principal ----------------------------------
void loop() 
{
uint16_t comando=0;   //guardo el comando
uint8_t  velocidad=0, indicador_velocidad;  //aca voy a tener las velocidades
uint8_t velocidades[]={20, 50, 100, 120, 150, 200, 220, 255}; //velocidades que voy a tener
long timeout_caracteres=0;
long tiempo_espera, timeout_motores;   //aca voy a medir el tiempo que pasa hasta que recibo otro comando
char caracter, *token, *copia;
int16_t canalx, canaly, canalz;
int16_t angulo_servo=90;

while(1)
  {
  if ((Serial.available()>0)) //si hay algun acaracter en el puerto serial...lo descargo
    {
    timeout_caracteres=millis();  //actualizo el tiempo en el que recibi el ultimo caracter
    caracter=Serial.read();  //leo el caracter recibido
    if(caracter=='\n' || caracter=='\r')
      {
      recibidos[indice]='\0';
      copia=recibidos;
      token=strtok(copia, ",");
      if(token!=NULL) canalx=atoi(token);
      token=strtok(NULL, ",");
      if(token!=NULL) canaly=atoi(token);
      token=strtok(NULL, ",");
      if(token!=NULL) canalz=atoi(token);
      indice=0;  
      setea_velocidad((canaly-canalx), (canaly+canalx));   
      timeout_motores=millis();
      
      if (canalz/2>90) canalz=180;
      else if (canalz/2 < -90) canalz=-180;
      servocamara.write(angulo_servo+canalz/2);
      /*
      Serial.print(canalx);
      Serial.print(',');
      Serial.print(canaly);
      Serial.print(',');
      Serial.println(canalz);
      */
      }
    else
      {
      recibidos[indice]=caracter;
      if((++indice)>=BUFFER_UART-1) indice=BUFFER_UART-1;
      }
    }
  else if(((millis()-timeout_caracteres)>TIMEOUT_UART) && (indice!=0))  //llego un caracter y paso un buen rato, tengo que hacer el parseo
    {
    indice=0;
    
    }

    if(millis()-timeout_motores >500)
      {
      setea_velocidad(0,0);  
      }
  }//cierre while infinito
}//cierre main



/*
#define T_UP    0xF906FF00  //avanza
#define T_DN    0xBB44FF00  //retrocede
#define T_DER   0xBF40FF00  //gira derecha
#define T_IZQ   0xB847FF00  //gira izq
#define T_ENTER 0xF807FF00  // nose?
#define T_VOLUP 0xFD02FF00  //aumenta velocidad
#define T_VOLDN 0xAE51FF00  //disminuye velocidad
#define T_POWER 0xEB14FF00  //tampoco se?
 */
