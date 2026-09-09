// Acesta este codul pentru placuta ARDUINO UNO
// citeste LDR (fotorezistor) si DS18B20 (temp) in 4 unghiuri setate {15, 35, 75, 110};
// alege unghiul cu cea mai mare expunere la lumina si ramane acolo 10 secunde pana cand cauta iar
// butonul pe D2 porneste/opreste sistemul, datele se trimit serial spre ESP
// smoothmove misca servo ul incet ca sa nu sara brusc
// delayCuButon inlocuieste delay urile ca sa nu ratam apasarea butonului

#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

Servo servo;
OneWire ow(4);
DallasTemperature ds(&ow);

// unghiurile de scanat
int unghiuri[] = {15, 35, 75, 110};
int pozitie = 70; // unde e servo ul initial

bool pornit = false; // incepe oprit 
bool lastBtn = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  servo.attach(9);
  servo.write(pozitie);
  ds.begin();
  delayCuButon(500);
}

// muta servo incet, un grad pe rand; ca sa nu fie brusc si sa fie cat mai smooth
void smoothmove(int tinta) {
  int pas;
  if (tinta > pozitie) // fata /spate
    pas = 1;
  else pas = -1;

  while (pozitie != tinta) {
    pozitie += pas;
    servo.write(pozitie);
    delayCuButon(10);
  }
}

// citeste LDR de 16 ori si face media ca sa rreturneze rezultate mai stabile
int citesteLDR() {
  int suma = 0;
  for (int i = 0; i < 16; i++) {
    suma += analogRead(A0);
    delayCuButon(5);
  }
  return suma / 16;
}

void delayCuButon(int ms) {
  for (int i = 0; i < ms; i += 10) {
    bool b = digitalRead(2);
    if (lastBtn == HIGH && b == LOW) {
      delay(50);
      b = digitalRead(2);
      if (b == LOW) {
          pornit = !pornit;
          Serial.println(pornit ? "STATUS=ON" : "STATUS=OFF"); // trimit spre esp
      }
    }
    lastBtn = b;
    delay(10);
  }
}

void loop() {
  if (!pornit) {
    delayCuButon(50);
    return;
  }

  // citeste temperatura o data pe ciclu
  ds.requestTemperatures();
  float temp = ds.getTempCByIndex(0);

  int bestLumina = -1;
  int bestUnghi = unghiuri[0];

  // scanam fiecare unghi
  for (int i = 0; i < 4; i++) {
    smoothmove(unghiuri[i]);
    delayCuButon(200); // asteptam sa se stabilizeze

    int lumina = citesteLDR();

    // trimitem masuratorile catre ESP
    Serial.print("U=");
    Serial.print(unghiuri[i]);
    Serial.print(" lumina=");
    Serial.print(lumina);
    Serial.print(" temp=");
    Serial.println(temp, 2);

    // retinem cel mai bun unghi
    if (lumina > bestLumina) {
      bestLumina = lumina;
      bestUnghi = unghiuri[i];
    }
  }

  // mutam la unghiul cu cea mai multa lumina
  smoothmove(bestUnghi);

  Serial.print("BESTU=");
  Serial.print(bestUnghi);
  Serial.print(" lumina=");
  Serial.print(bestLumina);
  Serial.print(" temp=");
  Serial.println(temp, 2);

  delayCuButon(10000);
}
