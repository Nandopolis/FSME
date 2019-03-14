// A pedestrian semaphore example using the Finite State Machine for Embedded systems (FSME) library

#include "Arduino.h"
#include <FSME.h>

// pin numbers definition
enum {
  CAR_GREEN = 2,
  CAR_YELLOW,
  CAR_RED,
  PED_GREEN,
  PED_RED,
  PED_WAITING,
};

// declaration of the FSM, states and transitions
// enum the states, just for human readable states
enum {
  CARS_GREEN_NO_PED,
  CARS_GREEN_INT,
  CARS_GREEN_PED_WAIT,
  CARS_YELLOW,
  CARS_RED_PED_WALK,
  CARS_RED_PED_FLASH,

  MAX_STATES  // this is the number of states
};
// declaration of the transitions
Transition *cgnp_trans[2];  // normally you declare an array of pointers to transitions,
Transition *cgi_trans[1];   // even if there is only 1 posible transition.
Transition *cgpw_trans[1];  // It needs to be an array of pointers to transitions in order
Transition *cy_trans[1];    // to be able to have different types of transitions in the
Transition *crpw_trans[1];  // same array
Transition *crpf_trans[1];
// declaration of the states
State sem_states[MAX_STATES];
// declaration of the FSM
FSME sem;

uint8_t flashes; // a blink counter

// functions prototypes used as transition events
uint8_t isPedWaiting(void);
uint8_t enoughFlashes(void);

// function prototypes used as state's actions
void carGreenLoop(void);
void pedWaitingLoop(void);
void carYellowLoop(void);
void pedWalkLoop(void);
void pedFlashLoop(void);

void setup() {
  // put your setup code here, to run once:
  // pin read/write settings
  pinMode(CAR_GREEN, OUTPUT);
  pinMode(CAR_YELLOW, OUTPUT);
  pinMode(CAR_RED, OUTPUT);
  pinMode(PED_GREEN, OUTPUT);
  pinMode(PED_RED, OUTPUT);
  pinMode(PED_WAITING, INPUT_PULLUP);

  // pin state initialitation
  digitalWrite(CAR_GREEN, 0);
  digitalWrite(CAR_YELLOW, 0);
  digitalWrite(CAR_RED, 0);
  digitalWrite(PED_GREEN, 0);
  digitalWrite(PED_RED, 0);

  // transitions for CARS_GREEN_NO_PED state
  // init an event transition (EvnTransition)
  // the first argument is a pointer to an event function that defines when the transition should happen
  // the second argument is the next state that should be set if the transition is triggered
  cgnp_trans[0] = new EvnTransition(isPedWaiting, CARS_GREEN_PED_WAIT);
  // init a time transition (TimeTransition)
  // the first argument is a timeout value in milliseconds that defines when the transition should happen
  // the second argument is the next state that should be set if the transition is triggered
  cgnp_trans[1] = new TimeTransition(10000, CARS_GREEN_INT);
  
  // transitions for CARS_GREEN_INT state
  cgi_trans[0] = new EvnTransition(isPedWaiting, CARS_YELLOW);
  
  // transitions for CARS_GREEN_PED_WAIT state
  cgpw_trans[0] = new TimeTransition(3000, CARS_YELLOW);
  
  // transitions for CARS_YELLOW state
  cy_trans[0] = new TimeTransition(3000, CARS_RED_PED_WALK);
  
  // transitions for CARS_RED_PED_WALK state
  crpw_trans[0] = new TimeTransition(10000, CARS_RED_PED_FLASH);
  
  // transitions for CARS_RED_PED_FLASH state
  crpf_trans[0] = new EvnTransition(enoughFlashes, CARS_GREEN_NO_PED);

  // states setting
  // use State::setState method as a shorthand for State::setAction and State::setTransitions methods
  // the first argument is a pointer to an action function that will be running while the state is active
  // the second argument is an array of posible transitions from that state
  // the last argument is  the number of transitions
  sem_states[CARS_GREEN_NO_PED].setState(carGreenLoop, cgnp_trans, 2);
  sem_states[CARS_GREEN_INT].setState(carGreenLoop, cgi_trans, 1);
  sem_states[CARS_GREEN_PED_WAIT].setState(pedWaitingLoop, cgpw_trans, 1);
  sem_states[CARS_YELLOW].setState(carYellowLoop, cy_trans, 1);
  sem_states[CARS_RED_PED_WALK].setState(pedWalkLoop, crpw_trans, 1);
  sem_states[CARS_RED_PED_FLASH].setState(pedFlashLoop, crpf_trans, 1);

  // FSM setting
  sem.setStates(sem_states, MAX_STATES);  // pass an array of states that conform the FSM, and the number of states
  sem.setInitialState(CARS_GREEN_NO_PED); // set the initial state

  flashes = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  sem.run();  // run the FSM
}

// An Event Function
// this will be called every time the transition associated with this event is evaluated
// this function doesn't take arguments and it must return an uint8_t
// if the function returns 1, the transition takes place
// if the function returns 0, there is no transition
uint8_t isPedWaiting(void) {
  return (digitalRead(PED_WAITING) == LOW);
}
// Another Event Function
uint8_t enoughFlashes(void) {
  return (flashes > 3);
}

// An Action Function
// this will be called while the state associated remains active
// this function doesn't take arguments, and it must return a void
void carGreenLoop(void) {
  // you can check if it's the first time the function is called,
  // i.e. the state was recently changed, to execute one time instructions
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
  // this will run once
  if (sem.isStateChanged()) {
    digitalWrite(PED_GREEN, 0);
    digitalWrite(PED_RED, 1);
    flashes = 0;
  }
  // this will run while the FSM is in the associated state
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