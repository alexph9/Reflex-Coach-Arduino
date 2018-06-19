/*TFG ALEJANDRO PUEBLA HOLGUÍN 2018
** => REFLEX COACH: Prototipo para entrenamiento, medición y evolución
**               de respuestas ante estímulos.
*/

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Time.h>
#include <TimeLib.h>

//Settings de Firebase y del WIFI
#define FIREBASE_HOST "reflex-coach.firebaseio.com"
#define FIREBASE_AUTH "FAliaQNPvUoTk6woFTmEJzBPpfyp8DP1ggAannJr"
#define WIFI_SSID "ALEXPH"
#define WIFI_PASSWORD "REFLEXCOACH"

#define NUM_ELEMENTOS 4
#define NUM_INTENTOS 50
#define TIEMPO_MAX 1000
/************************TIME STAMP*************************************/

#include <NTPClient.h>
#include <WiFiUdp.h>
#define NTP_OFFSET   60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "europe.pool.ntp.org"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

/***********************************************************************/

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
float tiempoInicial, tiempoFinal;

//Guardar el tiempo tardado de cada intento.
float tiempoTardado[NUM_INTENTOS];
//Guardar el Pulsador seleccionado (buzz)
short int buzz[NUM_INTENTOS];
//Guardar si ha fallado=0 /acertado =1
bool isSuccess[NUM_INTENTOS];

//Declaracion variable partida inicializada
bool partidaInicializada;
bool usuarioRegistrado;
short int contIntentos;
int pinesLedNodeMCU[NUM_ELEMENTOS] = {16, 4, 2, 12};
int pinesPulsadorNodeMCU[NUM_ELEMENTOS] = {5, 0, 14, 13};
int ledPartida = 15;
int estadoLedPartida;
//SuccessPercent = NUM_INTENTOS/numAciertos
int numAciertos;
//Bool para saber si el jugador a pulsado el botón
bool isBotonPulsado;
//Clave del Usuario
String userId = "bP4WJbewTNOY1QjbJQ9eHOjjX462";
float successPercent;
String timeStamp;


String principalPath;
//Indices
int i;


void setup() {

  timeClient.begin();
  /********************CONEXIÓN WIFI***************************/
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  /***********************************************************/
  
  /*
     (Led0, Pulsador0) = Pines(2,3) = Cable Azul
     (Led1, Pulsador1) = Pines(4,5) = Cable Naranja
     (Led2, Pulsador2) = Pines(6,7) = Cable Blanco
     (Led3, Pulsador3) = Pines(8,9) = Cable Verde
     (Led4, Pulsador4) = Pines(10,11) = Cable Amarillo
  */
  for (i = 0; i < NUM_ELEMENTOS; i++) {
    led[i] = pinesLedNodeMCU[i];
    pulsador[i] = pinesPulsadorNodeMCU[i] ;
    estadoLed[i] = LOW;
    estadoPulsador[i] = LOW;
  }
  
  estadoLedPartida = LOW;
  
  for (i = 0; i < NUM_ELEMENTOS; i++) {
    pinMode(pulsador[i], INPUT); //Declaracion de los pulsadores como entradas
    pinMode(led[i], OUTPUT); //Declaracion de los led como salida
  }
  pinMode(ledPartida, OUTPUT); //Declaracion de los led como salida

  for (i = 0; i < NUM_INTENTOS; i++) {
    tiempoTardado[i] = 0.0;
    buzz[i] = 0;
    isSuccess[i] = false;
  }

  partidaInicializada = false;
  tiempoInicial = 0.0;
  tiempoFinal = 0.0;
  usuarioRegistrado = false;
  numAciertos = 0;
  successPercent = -1;
}

void loop() {
  timeClient.update();
  randomSeed(millis()); //Semilla

  /******Esperando al inicio de Partida********************/
  if (!partidaInicializada) {
    digitalWrite(ledPartida, HIGH);
    estadoLedPartida = HIGH;
    while (estadoLedPartida) {
      delay(100);
      digitalWrite(ledPartida, LOW);
      delay(100);
      digitalWrite(ledPartida, HIGH);
      estadoPulsador[0] = digitalRead(pulsador[0]);
      if (estadoPulsador[0]) {
        digitalWrite(ledPartida, LOW);
        estadoPulsador[0] = LOW;
        estadoLedPartida = LOW;
        partidaInicializada = true;
      }
    }
  }
  /**********Comienza la partida************************/
  for (contIntentos = 0; contIntentos < NUM_INTENTOS; contIntentos++) {
    numAleatorioLed = random(NUM_ELEMENTOS); //Random entre 0 a NUM_ELEMENTOS-1 para elegir el Led a encender
    numAleatorioTiempo = (random(NUM_ELEMENTOS) + 1) * 100;
    delay(numAleatorioTiempo);
    digitalWrite(led[numAleatorioLed], HIGH);
    estadoLed[numAleatorioLed] = HIGH;
    tiempoInicial = millis();
    //Mientras el pulsador no se presione (estadoLed[numAleatorio] = HIGH)
    while (estadoLed[numAleatorioLed]) {
      estadoPulsador[numAleatorioLed] = digitalRead(pulsador[numAleatorioLed]);
      //El usuario NO presiona el boton *FALLO*
      if (millis() - tiempoInicial >= TIEMPO_MAX) {
        digitalWrite(led[numAleatorioLed], LOW);
        estadoPulsador[numAleatorioLed] = LOW;
        estadoLed[numAleatorioLed] = LOW;
        //Guardamos el tiempo tardado (el tiempoTotal en este caso)
        tiempoTardado[contIntentos] = TIEMPO_MAX;
        //Guardamos el pulsador seleccionado;
        buzz[contIntentos] = numAleatorioLed;
        //Guardamos un fallo;
        isSuccess[contIntentos] = false;
      } //El usuario presiona el boton *ACIERTO*
      else if (estadoPulsador[numAleatorioLed]) {
        tiempoFinal = millis();
        digitalWrite(led[numAleatorioLed], LOW);
        estadoPulsador[numAleatorioLed] = LOW;
        estadoLed[numAleatorioLed] = LOW;
        numAciertos++;
        //Guardamos el tiempo tardado
        tiempoTardado[contIntentos] = tiempoFinal - tiempoInicial;
        //Guardamos el pulsador seleccionado;
        buzz[contIntentos] = numAleatorioLed;
        //Guardamos un acierto;
        isSuccess[contIntentos] = true;
      }
    }
  }

  successPercent = (numAciertos / NUM_INTENTOS) * 100;
  timeStamp = timeClient.getEpochTime();
  principalPath = String("users/" + userId + "/games/" + timeStamp + "/");

  //TODO Envio oportuno de datos a Firebase
  /*****GAME**********/
  Firebase.setString(principalPath + "timeStamp" , String(timeStamp));
  Firebase.setFloat(principalPath + "maxTime" , TIEMPO_MAX);
  Firebase.setInt(principalPath + "numTries" , NUM_INTENTOS);
  Firebase.setFloat(principalPath + "successPercent" , successPercent);

  /******TRIES**********/
  for (i = 0; i < NUM_INTENTOS; i++) {
    Firebase.setInt(String(principalPath + "tries/" + i + "/buzz"), buzz[i]);
    Firebase.setFloat(String(principalPath + "tries/" + i + "/delay"), tiempoTardado[i]);
    Firebase.setBool(String(principalPath + "tries/" + i + "/isSuccess"), isSuccess[i]);
  }
  partidaInicializada = false;
  usuarioRegistrado = false;
}
