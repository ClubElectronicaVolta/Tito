// Programa para manejar a Tito, un movil con dos motores (traccion diferencial motor izquierdo y motor derecho)
// V 0.1
// Se maneja con un control remoto encontrado en el tiradero de cosas electronicas. Usa el protocolo NEC
// ocupa el PIN 2 para recibir.

//comandos de las teclas a utilizar: arriba, abajo, der, izq, etc 
#define T_ARRIBA    0x06  //avanza
#define T_ABAJO     0x44  //retrocede
#define T_DER       0x40  //gira derecha
#define T_IZQ       0x47  //gira izq
#define T_ENTER     0x07  // aun no tiene funcion. Automatico-manual...?
#define T_VOLUP     0x02  //aumenta velocidad
#define T_VOLDN     0x51  //disminuye velocidad
#define T_POWER     0x14  //aun no tiene funcion.

#define PWM_DER 5   //los pines 5 y 6 en el nano tienen mas frecuencia que los otros
#define PWM_IZQ 6
#define DIR_DER 3   //pines que determinan la direccion del motor
#define DIR_IZQ 4
#define AVANZA    0   // 0 va para adelante, 1 va para atras...por definicion...puede ser alreves
#define RETROCEDE 1
#define LED_ON    0   //que tengo que poner para que el led se prenda
#define LED_OFF   1   //que tengo que poner para que el led se apague
#define LED_R     7   // pines donde esta conectado el led rgb
#define LED_G     8
#define LED_B     9

#define TIMEOUT 1000  //1 segundo=1000 milisegundos

#define DECODE_NEC    // este es el protocolo que usa el control remoto de la basura


#include "PinDefinitionsAndMore.h"
#include "IRremote.hpp" // include the library


void setup() 
{

IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); //<- inicializo receptor, entrada en el pin 2, y destella en LED en pin 13
pinMode(LED_R, OUTPUT);     //inicializo todo como salida
pinMode(LED_G, OUTPUT);
pinMode(LED_B, OUTPUT);
pinMode(PWM_DER, OUTPUT);
pinMode(PWM_IZQ, OUTPUT);
pinMode(DIR_DER, OUTPUT);
pinMode(DIR_IZQ, OUTPUT);
}

void loop() 
{
uint16_t comando=0;   //guardo el comando
uint8_t  velocidad=0, indicador_velocidad;  //aca voy a tener las velocidades
uint8_t velocidades[]={20, 50, 100, 120, 150, 200, 220, 255}; //velocidades que voy a tener


long tiempo_espera;   //aca voy a medir el tiempo que pasa hasta que recibo otro comando

while(1)
  {
  if (IrReceiver.decode())  //si hay un dato disponible, entro
    {
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) //si error o desconocido, entro
      {  
      }
    else comando=IrReceiver.decodedIRData.command;  //si no, me quedo con el comando
  
    IrReceiver.resume();  //habilito la recepcion de mas datos.
    }

  if (comando !=0) // si comando distinto de cero significa que he recibido comando nuevo
    {
    switch (comando)
      {
      case T_ARRIBA:
        {
        analogWrite(PWM_DER, velocidad);
        analogWrite(PWM_IZQ, velocidad);
        digitalWrite(DIR_DER,AVANZA);
        digitalWrite(DIR_IZQ,AVANZA);
        break;    
        }
      case T_ABAJO:
        {
        analogWrite(PWM_DER, (255-velocidad));
        analogWrite(PWM_IZQ, (255-velocidad));
        digitalWrite(DIR_DER,RETROCEDE);
        digitalWrite(DIR_IZQ,RETROCEDE);
        break;    
        }
      case T_IZQ:
        {
        if(digitalRead(DIR_IZQ)==AVANZA)  //si estoy avanzado, girar izquierda es apagar motor izquierdo
          {
          analogWrite(PWM_DER, velocidad);
          analogWrite(PWM_IZQ, 0);
          digitalWrite(DIR_DER,AVANZA);
          digitalWrite(DIR_IZQ,AVANZA);
          }
        else 
          {
          analogWrite(PWM_DER, 255);
          analogWrite(PWM_IZQ, (255-velocidad));
          digitalWrite(DIR_DER,RETROCEDE);
          digitalWrite(DIR_IZQ,RETROCEDE);  
          }
        break;    
        }
      case T_DER:
        {
        if(digitalRead(DIR_DER)==AVANZA)
          {
          analogWrite(PWM_DER, 0);
          analogWrite(PWM_IZQ, velocidad);
          digitalWrite(DIR_DER,AVANZA);
          digitalWrite(DIR_IZQ,AVANZA);
          }
        else
          {
          analogWrite(PWM_DER, (255-velocidad));
          analogWrite(PWM_IZQ, 255);
          digitalWrite(DIR_DER,RETROCEDE);
          digitalWrite(DIR_IZQ,RETROCEDE);            
          }
        break;    
        }
      case T_VOLUP:
      case T_VOLDN:
        {
        if(comando==T_VOLUP) {if(++indicador_velocidad > 7) indicador_velocidad=7;}
        if(comando==T_VOLDN) {if(indicador_velocidad !=0 ) --indicador_velocidad;}       
        velocidad=velocidades[indicador_velocidad];
        if((indicador_velocidad & 0x01) !=0) digitalWrite(LED_R, LED_ON);
        else digitalWrite(LED_R, LED_OFF);
        if((indicador_velocidad & 0x02) !=0) digitalWrite(LED_G, LED_ON);
        else digitalWrite(LED_G, LED_OFF);
        if((indicador_velocidad & 0x04) !=0) digitalWrite(LED_B, LED_ON);
        else digitalWrite(LED_B, LED_OFF);
        break;    
        }
      default:      //si es cualquier otra tecla apago todo
        {
        analogWrite(PWM_DER, 0);
        analogWrite(PWM_IZQ, 0);  
        digitalWrite(DIR_DER,AVANZA);
        digitalWrite(DIR_IZQ,AVANZA);
        break;
        }
      }
    tiempo_espera=millis();
    comando=0;
    }
  else
    {
    if(millis() - tiempo_espera > TIMEOUT)  //si paso mucho tiempo sin recibir comando apago todo
      {
      analogWrite(PWM_DER, 0);
      analogWrite(PWM_IZQ, 0);  
      digitalWrite(DIR_DER,AVANZA);
      digitalWrite(DIR_IZQ,AVANZA);  
      }
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
