#include <LiquidCrystal.h>
#include <DHT.h>
#include "DHT.h"
#define SENDIST 5 //ultrasuoni


const int soglia_critica = 500; //Soglia di umidità terreno alla quale si accenderà la valvola
//pin sensori piante

#define Terreno_Pianta_1 A0
#define Terreno_Pianta_2 A1
#define Terreno_Pianta_3 A2
#define Terreno_Pianta_4 A3

//pin relay elettrovalvole
#define Elettrovalvola1 2
#define Elettrovalvola2 3
#define Elettrovalvola3 4 
#define Elettrovalvola4 5 

//pin sensore umidità/temperatura aria
#define SensoreAria 8
#define DHTTYPE DHT11
DHT dht(SensoreAria, DHTTYPE);



//pin ultrasuoni
#define trigUS 10
#define echoUS 11

//dichiarazione variabili umidità piante
int umiditPianta1;
int umiditPianta2;
int umiditPianta3;
int umiditPianta4;


LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

int tempAria;
int umAria;
int umTerreno;
int lvlAcqua;
int variabileTest;

//variabili livello acqua
long time = 0, wHeight = 0, wPer;
double dist = 0;
long height = 100;



void setup()
{
  Serial.begin(9600);
  pinMode(Elettrovalvola1, OUTPUT);
  pinMode(Elettrovalvola2, OUTPUT);
  pinMode(Elettrovalvola3, OUTPUT);
  pinMode(Elettrovalvola4, OUTPUT);
  lcd.begin(16, 2);
  pinMode(trigUS, OUTPUT);
  pinMode(echoUS, INPUT);

}
void loop()
{
  umiditaTerreno();
  LCD();
  livelloAcqua();
}

void umiditaTerreno(){
  //controllo suolo pianta 
  umiditPianta1 = analogRead(Terreno_Pianta_1); //Legge il valore analogico del sensore umidità terreno
  Serial.print("Sensore pianta 1 ");
  Serial.println(umiditPianta1); //Stampa a schermo il valore
  if (umiditPianta1 <= soglia_critica)
  digitalWrite(Elettrovalvola1,HIGH); //Accendi led
  else
  digitalWrite(Elettrovalvola1,LOW); //Spegni led
  
  //controllo suolo pianta 2
  
  umiditPianta2 = analogRead(Terreno_Pianta_2); //Legge il valore analogico del sensore umidità terreno
  Serial.print("Sensore pianta 2 ");
  Serial.println(umiditPianta2); //Stampa a schermo il valore
  if (umiditPianta2 <= soglia_critica)
  digitalWrite(Elettrovalvola2,HIGH); //Accendi led
  else
  digitalWrite(Elettrovalvola2,LOW); //Spegni led
  
  //controllo suolo pianta 3
  
  umiditPianta3 = analogRead(Terreno_Pianta_3); //Legge il valore analogico del sensore umidità terreno
  Serial.print("Sensore pianta 3 ");
  Serial.println(umiditPianta3); //Stampa a schermo il valore
  if (umiditPianta3 <= soglia_critica)
  digitalWrite(Elettrovalvola3,HIGH); //Accendi led
  else
  digitalWrite(Elettrovalvola3,LOW); //Spegni led
  
  //controllo suolo pianta 4
  
  umiditPianta4 = analogRead(Terreno_Pianta_4); //Legge il valore analogico del sensore umidità terreno
  Serial.print("Sensore pianta 4 ");
  Serial.println(umiditPianta4); //Stampa a schermo il valore
  if (umiditPianta4 <= soglia_critica)
  digitalWrite(Elettrovalvola4,HIGH); //Accendi led
  else
  digitalWrite(Elettrovalvola4,LOW); //Spegni led
  delay(2000); //Attende due secondi
}

void LCD(){
  variabileTest=44;
  
  lcd.setCursor(0, 0);
  lcd.print("temperatura:");
  lcd.print(variabileTest);
  lcd.setCursor(0, 1);
  lcd.print("Um aria:");
  lcd.print(variabileTest);
  delay(2000);
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Um terra:");
  lcd.print(variabileTest);
  lcd.setCursor(0, 1);
  lcd.print("Lvl acqua:");
  lcd.print(wPer);
  delay(2000);
  lcd.clear();
}

void livelloAcqua(){
  // Mandiamo il segnale
  digitalWrite(trigUS, LOW);
  delayMicroseconds(2);
  digitalWrite(trigUS, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigUS, LOW);
  
  //Riceviamo il segnale
  time = pulseIn(echoUS, HIGH);
  
  // Calcoliamo la distanza in cm dal tempo di risposta
  dist = time / 29.0 / 2.0;
  
  Serial.print(dist);
  Serial.print(" cm letto dal sensore\n");
  
  
  // Togliamo la distanza tra il sensore e il livello massimo dell'acqua
  height = height - SENDIST;
  dist = dist - SENDIST;
  
  
  //Calcoliamo l'altezza dell'acqua
  wHeight = height - dist;
  
  Serial.print(wHeight);
  Serial.print(" cm livello acqua\n");
  
  //Trasformiamo l'altezza dell'acqua in una percentuale
  wPer = wHeight * 100 / height;
  
    
  Serial.print(wPer);
  Serial.print("\%\n");
}

void TempHumAria(){
  int umAria = dht.readTemperature();
  int tempAria = dht.readHumidity();
}
