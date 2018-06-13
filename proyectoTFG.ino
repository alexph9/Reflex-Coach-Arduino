#include <Time.h>
#include <TimeLib.h>

/*Proyecto TFG Alejandro Puebla Holguin

   -> Prueba de concepto 1:
        Se enciende un led aleatorio,
        debemos accionar el pulsador para apagarlo.
*/
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

//Settings de Firebase y del WIFI
#define FIREBASE_HOST "reflex-coach.firebaseio.com"
#define FIREBASE_AUTH "FAliaQNPvUoTk6woFTmEJzBPpfyp8DP1ggAannJr"
#define WIFI_SSID "ALEXPH"
#define WIFI_PASSWORD "REFLEXCOACH"

#define NUM_ELEMENTOS 5
#define NUM_INTENTOS 50
#define TIEMPO_MAX 1000
/*********TIME STAMP********/
#include <NTPClient.h>
#include <WiFiUdp.h>
#define NTP_OFFSET   1*60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "europe.pool.ntp.org"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
/***************************/

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
int pinesLedNodeMCU[NUM_ELEMENTOS] = {16, 4, 2, 12, 15};
int pinesPulsadorNodeMCU[NUM_ELEMENTOS] = {5, 0, 14, 13, 9};
//SuccessPercent = NUM_INTENTOS/numAciertos
int numAciertos;
//Bool para saber si el jugador a pulsado el botón
bool isBotonPulsado;
//Clave del Usuario
String userId = "00000";
float successPercent;
String timeStamp;


String principalPath;
//Indices
int i, j;


void setup() {
  /*
     (Led0, Pulsador0) = Pines(2,3) = Cable Azul
     (Led1, Pulsador1) = Pines(4,5) = Cable Naranja
     (Led2, Pulsador2) = Pines(6,7) = Cable Blanco
     (Led3, Pulsador3) = Pines(8,9) = Cable Verde
     (Led4, Pulsador4) = Pines(10,11) = Cable Amarillo
  */

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
  for (i = 0; i < NUM_ELEMENTOS; i++) {
    led[i] = pinesLedNodeMCU[i];
    pulsador[i] = pinesPulsadorNodeMCU[i] ;
    estadoLed[i] = LOW;
    estadoPulsador[i] = LOW;
  }



  for (i = 0; i < NUM_ELEMENTOS; i++) {
    pinMode(pulsador[i], INPUT); //Declaracion de los pulsadores como entradas
    pinMode(led[i], OUTPUT); //Declaracion de los led como salida
  }

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

  /******Login usuario********/
  i = 0; //Contador de botones pulsados (El usuario debe pulsar 5 botones para registrarse)
  /*
  while (!usuarioRegistrado) {
    for (j = 0; j < NUM_ELEMENTOS; j++) {
      digitalWrite(led[j], HIGH);
      estadoLed[j] = HIGH;
    }
    isBotonPulsado = false;
    while (!isBotonPulsado) {
      delay(200);
      for (j = 0; j < NUM_ELEMENTOS; j++) {
        digitalWrite(led[j], LOW);
        estadoLed[j] = LOW;
      }
      delay(200);
      for (j = 0; j < NUM_ELEMENTOS; j++) {
        digitalWrite(led[j], HIGH);
        estadoLed[j] = HIGH;
      }
      estadoPulsador[0] = digitalRead(pulsador[0]);
      estadoPulsador[1] = digitalRead(pulsador[1]);
      estadoPulsador[2] = digitalRead(pulsador[2]);
      estadoPulsador[3] = digitalRead(pulsador[3]);
      estadoPulsador[4] = digitalRead(pulsador[4]);
      //Cuando se pulsa el PULSADOR 0
      if (estadoPulsador[0]) {
        digitalWrite(led[0], LOW);
        estadoPulsador[0] = LOW;
        estadoLed[0] = LOW;
        userId[i] = '0';
        i++;
        isBotonPulsado = true;
      }

      //Cuando se pulsa el PULSADOR 1
      else if (estadoPulsador[1]) {
        digitalWrite(led[1], LOW);
        estadoPulsador[1] = LOW;
        estadoLed[1] = LOW;
        userId[i] = '1';
        i++;
        isBotonPulsado = true;
      }

      //Cuando se pulsa el PULSADOR 2
      else if (estadoPulsador[2]) {
        digitalWrite(led[2], LOW);
        estadoPulsador[2] = LOW;
        estadoLed[2] = LOW;
        userId[i] = '2';
        i++;
        isBotonPulsado = true;
      }

      //Cuando se pulsa el PULSADOR 3
      else if (estadoPulsador[3]) {
        digitalWrite(led[3], LOW);
        estadoPulsador[3] = LOW;
        estadoLed[3] = LOW;
        userId[i] = '3';
        i++;
        isBotonPulsado = true;
      }

      //Cuando se pulsa el PULSADOR 4
      else if (estadoPulsador[4]) {
        digitalWrite(led[4], LOW);
        estadoPulsador[4] = LOW;
        estadoLed[4] = LOW;
        userId[i] = '4';
        i++;
        isBotonPulsado = true;
      }
      if (i == 5) {
        usuarioRegistrado = true;
      }
    }
  }
*/
  /******Esperando al inicio de Partida********************/
  if (!partidaInicializada) {
    digitalWrite(led[0], HIGH);
    estadoLed[0] = HIGH;
    while (estadoLed[0]) {
      delay(100);
      digitalWrite(led[0], LOW);
      delay(100);
      digitalWrite(led[0], HIGH);
      estadoPulsador[0] = digitalRead(pulsador[0]);
      if (estadoPulsador[0]) {
        digitalWrite(led[0], LOW);
        estadoPulsador[0] = LOW;
        estadoLed[0] = LOW;
        partidaInicializada = true;
      }
    }
  }
  /**********Comienza la partida************************/
  for (contIntentos = 0; contIntentos < NUM_INTENTOS; contIntentos++) {
    numAleatorioLed = random(NUM_ELEMENTOS); //Random entre 0 a NUM_ELEMENTOS-1 para elegir el Led a encender
    numAleatorioTiempo = (random(5) + 1) * 100;
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
  delay(1000);
  partidaInicializada = false;
  usuarioRegistrado = false;

}


