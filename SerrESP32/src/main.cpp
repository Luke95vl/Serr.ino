#include <Arduino.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#define SENDIST 5 // distanza sensore cm

const int soglia_critica = 800; // Soglia di umidità terreno alla quale si accenderà la valvola
int soglia_temperatura = 25;

// pin sensori piante
#define Terreno_Pianta_1 A0
#define Terreno_Pianta_2 A1
#define Terreno_Pianta_3 A2
#define Terreno_Pianta_4 A3
#define Terreno_Pianta_5 A4
#define Terreno_Pianta_6 A5

// pin relay elettrovalvole
#define Elettrovalvola1 22
#define Elettrovalvola2 23
#define Elettrovalvola3 24
#define Elettrovalvola4 25
#define Elettrovalvola5 26
#define Elettrovalvola6 27
#define Ventola 39

#define plugPianta1 52
// #define plugPianta2 NAN
// #define plugPianta3 NAN
// #define plugPianta4 NAN
// #define plugPianta5 NAN
// #define plugPianta6 NAN

// pin sensore umidità/temperatura aria
#define SensoreAria 8
#define LEDAcqua LED_BUILTIN
#define DHTTYPE DHT11

DHT dht(SensoreAria, DHT11);

// pin ultrasuoni
#define trigUS 3
#define echoUS 2

// dichiarazione variabili umidità piante
int umiditPianta1;
int umiditPianta2;
int umiditPianta3;
int umiditPianta4;
// int umiditPianta5;
// int umiditPianta6;
bool irrigazioneAttiva; // segnala se è in atto una o più irrigazioni

// const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int tempAria;
int umAria;
int umTerreno;
int lvlAcqua;
int variabileTest;
long timeNow;

// variabili livello acqua
long time = 0;
long oldTimePiante = 0;
long oldTimeTemp = 0;
float dist = 0;
byte height = 20, wHeight = 0, lvlAcquaPerc;

// dichiarazione funzioni
void umiditaTerreno(int SensorePianta, int elettrovalvola, int numeroPianta, int *umiditPianta);
// void LCD();
void livelloAcqua();
void TempHumAria();

void setup()
{
  Serial.begin(115200);
  dht.begin();
  pinMode(Elettrovalvola1, OUTPUT);
  pinMode(Elettrovalvola2, OUTPUT);
  pinMode(Elettrovalvola3, OUTPUT);
  pinMode(Elettrovalvola4, OUTPUT);
  // pinMode(Elettrovalvola5, OUTPUT);
  // pinMode(Elettrovalvola6, OUTPUT);
  pinMode(LEDAcqua, OUTPUT);
  pinMode(Ventola, OUTPUT);
  digitalWrite(Ventola, HIGH);
   digitalWrite(Elettrovalvola1, HIGH);

  // pin verifica plug piante
  pinMode(plugPianta1, INPUT_PULLUP);
  // pinMode(plugPianta2, INPUT_PULLUP);
  // pinMode(plugPianta3, INPUT_PULLUP);
  // pinMode(plugPianta4, INPUT_PULLUP);
  // pinMode(plugPianta5, INPUT_PULLUP);
  // pinMode(plugPianta6, INPUT_PULLUP);

  // lcd.begin(16, 2);
  pinMode(trigUS, OUTPUT);
  pinMode(echoUS, INPUT);
}
void loop()
{
  String printString = "";
  time = millis();
  // if (oldTimeTemp == 0 || (time - oldTimeTemp) > 1800000) // check serbatoio e temperatura ogni 30 minuti
  //  {
  oldTimeTemp = millis();
  livelloAcqua();
  TempHumAria();
  // }
  // if (oldTimePiante == 0 || (time - oldTimePiante) > 3600000 || irrigazioneAttiva) // check ogni 60 minuti o costante se l'irrigazione è attiva
  // {
  oldTimePiante = millis();
  if (!digitalRead(plugPianta1)) // TODO logica pullup pulsante low high da controllare                                                                             // 60 minuti in millisecondi
    umiditaTerreno(Terreno_Pianta_1, Elettrovalvola1, 1, &umiditPianta1);
  else if (digitalRead(plugPianta1) && umiditPianta1 != 0)
    umiditPianta1 == 0;
  // if (!digitalRead(plugPianta2))
  umiditaTerreno(Terreno_Pianta_2, Elettrovalvola2, 2, &umiditPianta2);
  // if (!digitalRead(plugPianta3))
  umiditaTerreno(Terreno_Pianta_3, Elettrovalvola3, 3, &umiditPianta3);
  // if (!digitalRead(plugPianta4))
  umiditaTerreno(Terreno_Pianta_4, Elettrovalvola4, 4, &umiditPianta4);
  // if (!digitalRead(plugPianta5))
  //    umiditaTerreno(Terreno_Pianta_5, Elettrovalvola5, 5, &umiditPianta5);
  //  //if (!digitalRead(plugPianta6))
  //    umiditaTerreno(Terreno_Pianta_6, Elettrovalvola6, 6, &umiditPianta6);
  // }
  // LCD();

  if (lvlAcquaPerc <= 15) // lampeggio serbatoio sotto 15%
    digitalWrite(LEDAcqua, 255);
  else if (lvlAcquaPerc > 15 && lvlAcquaPerc < 30) // lampeggio led se serbatoio tra 15% e 30%
  {
    digitalWrite(LEDAcqua, 255);
    delay(500);
    digitalWrite(LEDAcqua, 0);
    delay(500);
  }
  else if (lvlAcquaPerc >= 30)
    digitalWrite(LEDAcqua, 0);

  // controllo temperatura ventilazione
  if (tempAria >= soglia_temperatura)
    digitalWrite(Ventola, LOW);
  if (tempAria == (soglia_temperatura - 2))
    digitalWrite(Ventola, HIGH);

  // scrittura valori in seriale
  printString = printString + "Sensore pianta 1" + ":Umidità:\t" + String(umiditPianta1) + "\n" +
                "Sensore pianta 2" + ":Umidità:\t" + String(umiditPianta2) + "\n" +
                "Sensore pianta 3" + ":Umidità:\t" + String(umiditPianta3) + "\n" +
                "Sensore pianta 4" + ":Umidità:\t" + String(umiditPianta4) + "\n" +
                "Livello Acqua:\t" + String(lvlAcquaPerc) + "%\n" +
                "Umidità aria: " + String(umAria) + "\tTemp. aria:\t" + String(tempAria) + "° C";
  Serial.println(printString);
  delay(2000);
}

void umiditaTerreno(int SensorePianta, int elettrovalvola, int numeroPianta, int *umiditPianta)
{
  // controllo suolo pianta
  *umiditPianta = analogRead(SensorePianta); // Legge il valore analogico del sensore umidità terreno
  if (*umiditPianta >= soglia_critica && lvlAcquaPerc > 15)
  {
    digitalWrite(elettrovalvola, LOW); // Accendi eletrrovalvola
    // TODO ACCENSIONE POMPA
    irrigazioneAttiva = true; // segnalo che è attiva una irrigazione
  }
  else if (*umiditPianta < 900 || lvlAcquaPerc <= 15)
  {
    // TODO SPEGIMENTO POMPA
    digitalWrite(elettrovalvola, HIGH); // Spegni eletrrovalvola
    if (digitalRead(Elettrovalvola1) == HIGH || digitalRead(Elettrovalvola2) == HIGH || digitalRead(Elettrovalvola3) == HIGH || digitalRead(Elettrovalvola4) == HIGH || digitalRead(Elettrovalvola5) == HIGH || digitalRead(Elettrovalvola6) == HIGH)
      irrigazioneAttiva = false; // se tutte le elettrovalvole sono spente non c'è irrigazione => non serve fare il check costante per spegnere l'irrigazione
  }
}

// void LCD(){
//   variabileTest=44;

//   lcd.setCursor(0, 0);
//   lcd.print("temperatura:");
//   lcd.print(umiditPianta1);
//   lcd.setCursor(0, 1);
//   lcd.print("Um aria:");
//   lcd.print(umAria);
//   delay(2000);
//   lcd.clear();

//   lcd.setCursor(0, 0);
//   lcd.print("Um terra:");
//   lcd.print(variabileTest);
//   lcd.setCursor(0, 1);
//   lcd.print("Lvl acqua:");
//   lcd.print(lvlAcquaPerc);
//   delay(2000);
//   lcd.clear();
// }

void livelloAcqua()
{
  // Mandiamo il segnale
  digitalWrite(trigUS, LOW);
  delayMicroseconds(2);
  digitalWrite(trigUS, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigUS, LOW);

  // Riceviamo il segnale
  time = pulseIn(echoUS, HIGH);

  // Calcoliamo la distanza in cm dal tempo di risposta
  dist = time * 0.017;

  // Togliamo la distanza tra il sensore e il livello massimo dell'acqua
  dist -= SENDIST;

  // Calcoliamo l'altezza dell'acqua
  wHeight = height - dist;

  // Serial.print(wHeight);
  // Serial.print(" cm livello acqua\n");

  // Trasformiamo l'altezza dell'acqua in una percentuale
  lvlAcquaPerc = wHeight * 100 / height;
}

void TempHumAria()
{
  tempAria = dht.readTemperature();
  umAria = dht.readHumidity();
}
