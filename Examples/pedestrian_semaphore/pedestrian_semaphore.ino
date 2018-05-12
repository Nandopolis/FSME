#include "Arduino.h"
#include <FSME.h>

enum {
  CAR_GREEN = 2,
  CAR_YELLOW,
  CAR_RED,
  PED_GREEN,
  PED_RED,
  PED_WAITING,
};

enum {
  CARS_GREEN_NO_PED,
  CARS_GREEN_INT,
  CARS_GREEN_PED_WAIT,
  CARS_YELLOW,
  CARS_RED_PED_WALK,
  CARS_RED_PED_FLASH,
};
Transition *cgnp_trans[2];
Transition *cgi_trans[1];
Transition *cgpw_trans[1];
Transition *cy_trans[1];
Transition *crpw_trans[1];
Transition *crpf_trans[1];
State sem_states[6];
FSME sem;

uint8_t flashes;

uint8_t isPedWaiting(void);
uint8_t enoughFlashes(void);

void carGreenLoop(void);
void pedWaitingLoop(void);
void carYellowLoop(void);
void pedWalkLoop(void);
void pedFlashLoop(void);

void setup() {
  // put your setup code here, to run once:
  pinMode(CAR_GREEN, OUTPUT);
  pinMode(CAR_YELLOW, OUTPUT);
  pinMode(CAR_RED, OUTPUT);
  pinMode(PED_GREEN, OUTPUT);
  pinMode(PED_RED, OUTPUT);
  pinMode(PED_WAITING, INPUT_PULLUP);

  digitalWrite(CAR_GREEN, 0);
  digitalWrite(CAR_YELLOW, 0);
  digitalWrite(CAR_RED, 0);
  digitalWrite(PED_GREEN, 0);
  digitalWrite(PED_RED, 0);

  cgnp_trans[0] = new EvnTransition(isPedWaiting, CARS_GREEN_PED_WAIT);
  cgnp_trans[1] = new TimeTransition(10000, CARS_GREEN_INT);
  cgi_trans[0] = new EvnTransition(isPedWaiting, CARS_YELLOW);
  cgpw_trans[0] = new TimeTransition(3000, CARS_YELLOW);
  cy_trans[0] = new TimeTransition(3000, CARS_RED_PED_WALK);
  crpw_trans[0] = new TimeTransition(10000, CARS_RED_PED_FLASH);
  crpf_trans[0] = new EvnTransition(enoughFlashes, CARS_GREEN_NO_PED);

  sem_states[CARS_GREEN_NO_PED].setState(carGreenLoop, cgnp_trans, 2);
  sem_states[CARS_GREEN_INT].setState(carGreenLoop, cgi_trans, 1);
  sem_states[CARS_GREEN_PED_WAIT].setState(pedWaitingLoop, cgpw_trans, 1);
  sem_states[CARS_YELLOW].setState(carYellowLoop, cy_trans, 1);
  sem_states[CARS_RED_PED_WALK].setState(pedWalkLoop, crpw_trans, 1);
  sem_states[CARS_RED_PED_FLASH].setState(pedFlashLoop, crpf_trans, 1);

  sem.setStates(sem_states, 6);
  sem.setInitialState(CARS_GREEN_NO_PED);

  flashes = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  sem.run();
}

uint8_t isPedWaiting(void) {
  return (digitalRead(PED_WAITING) == LOW);
}

uint8_t enoughFlashes(void) {
  return (flashes > 3);
}

void carGreenLoop(void) {
  if (sem.isStateChanged()) {
    digitalWrite(PED_RED, 1);
    digitalWrite(CAR_RED, 0);
    digitalWrite(CAR_GREEN, 1);
  }
}

void pedWaitingLoop(void) {
  static uint32_t temp = millis();
  
  if (millis() - temp > 500) {
    if (digitalRead(CAR_GREEN)) {
      digitalWrite(CAR_GREEN, 0);
    }
    else {
      digitalWrite(CAR_GREEN, 1);
    }
    temp = millis();
  }
}

void carYellowLoop(void) {
  if (sem.isStateChanged()) {
    digitalWrite(CAR_GREEN, 0);
    digitalWrite(CAR_YELLOW, 1);
  }
}

void pedWalkLoop(void) {
  if (sem.isStateChanged()) {
    digitalWrite(CAR_YELLOW, 0);
    digitalWrite(CAR_RED, 1);
    digitalWrite(PED_RED, 0);
    digitalWrite(PED_GREEN, 1);
  }
}

void pedFlashLoop(void) {
  static uint32_t temp = millis();
  
  if (sem.isStateChanged()) {
    digitalWrite(PED_GREEN, 0);
    digitalWrite(PED_RED, 1);
    flashes = 0;
  }
  
  if (millis() - temp > 500) {
    if (digitalRead(PED_RED)) {
      digitalWrite(PED_RED, 0);
    }
    else {
      digitalWrite(PED_RED, 1);
      flashes++;
    }
    temp = millis();
  }
}