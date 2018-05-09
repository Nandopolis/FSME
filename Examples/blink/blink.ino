// A blink example using the Finite State Machine for Embedded systems (FSME) library

#include "Arduino.h"
#include "FSME.h"

// enum the states, just for human readable states
enum states {
	s0,
	s1
};

// declaration of the FSM, states and transitions
FSME fsm;
State states[2]; // you need to declare an array of states
Transition *s0_trans[1]; // normally you declare an array of pointers to transitions,
Transition *s1_trans[1]; // even if there is only 1 posible transition

// a variable used for timing
uint32_t temp;

// function prototype used as a time out event
uint8_t timeOut(void);
// function prototypes used as state's actions
void turnOn(void);
void turnOff(void);

//The setup function is called once at startup of the sketch
void setup()
{
// Add your initialization code here
  Serial.begin(115200);
  while(!Serial); // wait for serial port to connect. Needed for native USB.
	pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  
  // setting up the FSM
  Serial.println("setting up the fsm...");

  // declare an event transition (EvnTransition)
  // the first argument is a pointer to an event function that defines when the transition should happen
  // the second argument is the next state that should be set if the transition is triggered
  // *** note that its a timed transition, you can declare a time transition instead:
  // *** s0_trans[0] = new TimeTransition(time_out_in_miliseconds, s1);
  s0_trans[0] = new EvnTransition(timeOut, s1);
  s1_trans[0] = new EvnTransition(timeOut, s0);  // the same for all transitions
  
  states[s0].setAction(turnOn); // pass a pointer to an action function that will be running while the state is active
  states[s0].setTransitions(s0_trans, 1); // pass an array of posible transitions from that state, and the number of transitions
  states[s1].setAction(turnOff); // the same for all states
  states[s1].setTransitions(s1_trans, 1); // you can assign both action and transitions with: 
                                          // states[s1].setState(turnOff, s1_trans, 1);
  
  fsm.setStates(states, 2); // pass an array of states that conform the FSM, and the number of states
  fsm.setInitialState(s0); // set the initial state
/*
  Serial.print("timeOut pointer: ");
  Serial.println((uint16_t)timeOut);
  Serial.print("turnOn pointer: ");
  Serial.println((uint16_t)turnOn);
  Serial.print("turnOff pointer: ");
  Serial.println((uint16_t)turnOff);
  fsm.serialPrint();
*/
  Serial.println("done!");
  temp = millis(); // start watching the time from this point
}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
	fsm.run(); // run the FSM
}

// An Event Function
// this will be called every time the transition associated with this event is evaluated
// this function doesn't take arguments and it must return an uint8_t
// if the function returns 1, the transition takes place
// if the function returns 0, ther is no transition
uint8_t timeOut(void){
	if (millis() - temp > 500) {
		temp = millis();
    Serial.println("time out!");
		return 1;
	}
	return 0;
}

// An Action Function
// this will be called while the state associated remains active
// this function doesn't take arguments, and it must return a void
void turnOn(void){
  if (fsm.isStateChanged()) { // you can check if it's the first time the function is called,
                              // i.e. the state was recently changed, to execute one time instructions
    Serial.println("led ON");
    digitalWrite(13, 1);
  }
}

void turnOff(void){
  if (fsm.isStateChanged()) {
    Serial.println("led OFF");
    digitalWrite(13, 0);
  }
}