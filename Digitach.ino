 /*
 *  TACÓMETRO / MEDIDOR RPM
 *  Programado por Pablo Luaces Presas
 *  Este sketch usa interrupciones, no compatible con todas las placas arduino
 *  www.dezatec.es
 *  
 *  Connect a wire to car bobine +12v with a zenner protection
 */

#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
#include <SevenSegmentFun.h>

/*
 * CONFIG VARS
 */

const unsigned int DISPLAY_CLK = 4; // Display CLK wire pin
const unsigned int DISPLAY_DIO = 5; // Display DIO wire pin 
const unsigned int RPM_SIGNAL_PIN = 2; // Car RPM signal wire pin (INTERRUPT PIN) (with 5v Zener and 100K resistor to 12V bobine signal)
const unsigned int RPM_PULSES_NUMBER = 4; // Pulses in one revolution
const unsigned int RPM_MEASSURE_INTERVAL = 200; // Display refresh interval
const int MAX_RPM = 12000; // For progress bar display

/*
 * PROGRAM VARS & LIBRARIES
 */

long microTimer; // Timer para obtener los microsegundos exactos de la medición
volatile unsigned long PULSES; // Contador incremental de interrupciones
SevenSegmentFun display(DISPLAY_CLK, DISPLAY_DIO); // Iniciamos librería del display

/*
 * PULSE COUNTER RESTART
 */

void newCycle(){
  microTimer = micros(); // Timer reset
  PULSES = 0; // Counter reset
}

/*
 * INTERRUPT CALLBACK [RPM_SIGNAL_PIN]
 */

void pulseCounter(){
  PULSES++; // Increment pulses
}

/*
 * PROGRAM INICIALIZATION
 */

void setup() {
  // Abrimos puerto serial
  //Serial.begin(9600);

  // Configuramos los pines
  pinMode(RPM_SIGNAL_PIN, INPUT);

  // Disparador de contador RPM
  attachInterrupt(digitalPinToInterrupt(RPM_SIGNAL_PIN), pulseCounter, RISING); // Ejecutar pulseCounter cada vez que se detecta una subida de señal
  
  // Configuramos el display
  display.init();
  display.setBacklight(50);
  display.print("SUZUKI");
  delay(300);
  display.snake(1);
  display.clear();

  // Iniciamos el timer
  microTimer = micros();
}

/*
 * ARDUINO INFINITE LOOP
 */

void loop() {
  // Actualizar información de RPM cada X tiempo

  char rpmCharArray[4]; // Create empty char array to store formatted RPM
  long milisElapsed = (micros() - microTimer) / 1000; // Get time elapsed for more precision 
  int actualRpm = ((PULSES/RPM_PULSES_NUMBER) * (60000/milisElapsed) / 2); // Calculate RPM [(REVS * (MILISECONDS_IN_MINUTE/RPM_MEASSURE_INTERVAL) / 2TURNS)]

  //Serial.println("ELAPSED: " + String(milisElapsed));
  //Serial.println("PULSES: " + String(PULSES));
  //Serial.println("RPM: " + String(actualRpm));
  //Serial.println("--------------------");

  newCycle(); // Reset pulses after read it

  if (actualRpm > 0){
    // Uncomment this to show RPM Numbers
    //sprintf(rpmCharArray, "%04d", actualRpm); // Formatt RPM
    //display.print(rpmCharArray); // Show RPM number in display
    //delay(100);
    
    int rpmPorcent = round((100 * actualRpm) / MAX_RPM);
    display.print9LevelVertical(rpmPorcent); // Show progress bar in display
    if (rpmPorcent >= 90) display.blink(); // Blink on hight level

    //Serial.println(actualRpm);
    //Serial.println(rpmPorcent);
    //Serial.println("--------------------");
    
  }else display.clear();

  delay(RPM_MEASSURE_INTERVAL);
}

