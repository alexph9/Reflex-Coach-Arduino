/*Proyecto TFG Alejandro Puebla Holguin
 * 
 * -> Prueba de concepto 1:
 *      Se enciende un led aleatorio, 
 *      debemos accionar el pulsador para apagarlo. 
 */
#include <SoftwareSerial.h>
#define NUM_ELEMENTOS 5
#define NUM_INTENTOS 50

 //Declaracion pulsadores
short int pulsador[NUM_ELEMENTOS];

 //Declaracion led
short int led[NUM_ELEMENTOS];

//Declaracion de un numero aleatorio para eleccion de led a encender
short int numAleatorioLed;
//Declaracion numero aleatorio para elegir el tiempo de espera entre un led y el siguiente
short int numAleatorioTiempo;

//Declaracion estados Led y estados pulsador
short int estadoLed[NUM_ELEMENTOS];
short int estadoPulsador[NUM_ELEMENTOS];

//Declaracion variable para controlar el tiempo de actuacion
unsigned long tiempoInicial, tiempoFinal;

//Declaracion matriz para guardar datos de la partida(tiempoTotal, Pulsador seleccionado)
unsigned short partida [2] [NUM_INTENTOS];

//Declaracion variable partida inicializada
    //partidaInicializada = 0 False;
    //partidaInicializada = 1 True;
char partidaInicializada;
short int contIntentos;

void setup() {
  /*
   * (Led0, Pulsador0) = Pines(2,3) = Cable Azul
   * (Led1, Pulsador1) = Pines(4,5) = Cable Naranja
   * (Led2, Pulsador2) = Pines(6,7) = Cable Blanco
   * (Led3, Pulsador3) = Pines(8,9) = Cable Verde
   * (Led4, Pulsador4) = Pines(10,11) = Cable Amarillo
  */

  int i, j;
  for (i = 0; i < NUM_ELEMENTOS; i++){
    led[i] = (i + 1) * 2 ;
    pulsador[i] = led[i] + 1 ;
    estadoLed[i] = LOW;
    estadoPulsador [i] = LOW;
  }

  for (i = 0; i < NUM_ELEMENTOS; i++){
    pinMode(pulsador[i], INPUT); //Declaracion de los pulsadores como entradas
    pinMode(led[i], OUTPUT); //Declaracion de los led como salida
  }

  for (i = 0; i < 2; i++){
    for (j = 0; j < NUM_INTENTOS; j++){
      partida[i][j] = 0;
    }
  }

  partidaInicializada = 0;
  tiempoInicial = 0;
  tiempoFinal = 0; 
}

void loop() {

  randomSeed(millis()); //Semilla

  //Bucle inicio de partida
  if(!partidaInicializada) {
    digitalWrite(led[0], HIGH);
    estadoLed[0] = HIGH;
    while(estadoLed[0]){
      delay(100);
      digitalWrite(led[0], LOW);
      delay(100);
      digitalWrite(led[0], HIGH);
      estadoPulsador[0] = digitalRead(pulsador[0]);
      if(estadoPulsador[0]){
        digitalWrite(led[0], LOW);
        estadoPulsador[0] = LOW;
        estadoLed[0] = LOW;
        partidaInicializada = 1;
      }
    } 
  }
  
  for(contIntentos = 0; contIntentos < NUM_INTENTOS; contIntentos++){
    numAleatorioLed = random(NUM_ELEMENTOS); //Random entre 0 a NUM_ELEMENTOS-1 para elegir el Led a encender
    numAleatorioTiempo = (random(5) + 1) * 100;
    delay(numAleatorioTiempo);
    digitalWrite(led[numAleatorioLed], HIGH);
    estadoLed[numAleatorioLed] = HIGH;
    tiempoInicial = millis();
    //Mientras el pulsador no se presione (estadoLed[numAleatorio] = HIGH)
    while(estadoLed[numAleatorioLed]){
      estadoPulsador[numAleatorioLed] = digitalRead(pulsador[numAleatorioLed]);
      if(millis() - tiempoInicial >= 1000){
        tiempoFinal = 500;
        digitalWrite(led[numAleatorioLed], LOW);
        estadoPulsador[numAleatorioLed] = LOW;
        estadoLed[numAleatorioLed] = LOW;
        //Guardamos el tiempo total
        partida[0][contIntentos] = tiempoFinal - tiempoInicial; 
        //Guardamos el pulsador seleccionado;
        partida[0][contIntentos] = numAleatorioLed;
      }
      else if(estadoPulsador[numAleatorioLed]){
        tiempoFinal = millis();
        digitalWrite(led[numAleatorioLed], LOW);
        estadoPulsador[numAleatorioLed] = LOW;
        estadoLed[numAleatorioLed] = LOW;
        //Guardamos el tiempo total
        partida[0][contIntentos] = tiempoFinal - tiempoInicial; 
        //Guardamos el pulsador seleccionado;
        partida[0][contIntentos] = numAleatorioLed;
      }
    }
  }
  //TODO Envio oportuno de datos a Firebase
  partidaInicializada = 0;   
} 
 

