#include <FSME.h>

enum{
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
Transition cgnp_trans[2];
Transition cgi_trans[1];
Transition cgpw_trans[1];
Transition cy_trans[1];
Transition crpw_trans[1];
Transition crpf_trans[1];
State sem_states[6];
FSME sem;

uint32_t time_out;
uint8_t flashes;
uint32_t temp;

uint8_t timeOut(void);
uint8_t isPedWaiting(void);
uint8_t enoughFlashes(void);

void carGreen(void);
void pedWaiting(void);
void carYellow(void);
void pedWalk(void);
void pedFlash(void);

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

  cgnp_trans[0].setTransition(isPedWaiting, CARS_GREEN_PED_WAIT);
  cgnp_trans[1].setTransition(timeOut, CARS_GREEN_INT);
  cgi_trans[0].setTransition(isPedWaiting, CARS_YELLOW);
  cgpw_trans[0].setTransition(timeOut, CARS_YELLOW);
  cy_trans[0].setTransition(timeOut, CARS_RED_PED_WALK);
  crpw_trans[0].setTransition(timeOut, CARS_RED_PED_FLASH);
  crpf_trans[0].setTransition(enoughFlashes, CARS_GREEN_NO_PED);

  sem_states[CARS_GREEN_NO_PED].setState(carGreen, cgnp_trans, 2);
  sem_states[CARS_GREEN_INT].setState(carGreen, cgi_trans, 1);
  sem_states[CARS_GREEN_PED_WAIT].setState(pedWaiting, cgpw_trans, 1);
  sem_states[CARS_YELLOW].setState(carYellow, cy_trans, 1);
  sem_states[CARS_RED_PED_WALK].setState(pedWalk, crpw_trans, 1);
  sem_states[CARS_RED_PED_FLASH].setState(pedFlash, crpf_trans, 1);

  sem.setStates(sem_states, 6);
  sem.setInitialState(CARS_GREEN_NO_PED);

  time_out = 0xFFFFFFFF;
  flashes = 0;
  temp = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  sem.run();
}

uint8_t timeOut(void) {
  return (millis() - temp > time_out);
}

uint8_t isPedWaiting(void) {
  return (digitalRead(PED_WAITING) == LOW);
}

uint8_t enoughFlashes(void) {
  return (flashes > 3);
}

void carGreen(void) {
  if (sem.isStateChanged()) {
    digitalWrite(PED_RED, 1);
    digitalWrite(CAR_RED, 0);
    digitalWrite(CAR_GREEN, 1);
    time_out = 10000;
    temp = millis();
  }
}

void pedWaiting(void) {
  if (sem.isStateChanged()) {
    time_out = 2000;
    temp = millis();
  }
}

void carYellow(void) {
  if (sem.isStateChanged()) {
    digitalWrite(CAR_GREEN, 0);
    digitalWrite(CAR_YELLOW, 1);
    time_out = 3000;
    temp = millis();
  }
}

void pedWalk(void) {
  if (sem.isStateChanged()) {
    digitalWrite(CAR_YELLOW, 0);
    digitalWrite(CAR_RED, 1);
    digitalWrite(PED_RED, 0);
    digitalWrite(PED_GREEN, 1);
    time_out = 10000;
    temp = millis();
  }
}

void pedFlash(void) {
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
