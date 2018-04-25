/*
 * FSME.cpp
 *
 *  Created on: May 12, 2017
 *      Author: Nandopolis
 */

//#include "Arduino.h"
#include "FSME.h"

Transition::Transition(FSME_PF_EV Event, uint8_t NextState) {
	this->Event = Event;
	this->NextState = NextState;
}

Transition::Transition() {
	this->Event = 0;
	this->NextState = 0;
}

void Transition::setEvent(FSME_PF_EV Event) {
	this->Event = Event;
}

void Transition::setNextState(uint8_t NextState) {
	this->NextState = NextState;
}

uint8_t Transition::runEvent(void) {
	return this->Event();
}

uint8_t Transition::getNextState(void) {
	return this->NextState;
}
/*
void Transition::serialPrint(void) {
	Serial.print("event pointer: ");
	Serial.println((uint16_t)this->Event);
	Serial.print("next state: ");
	Serial.println(this->NextState);
}
*/
State::State(FSME_PF Action, uint8_t TransNO, Transition *Trans) {
	this->Action = Action;
	this->TransNO = TransNO;
	this->Trans = Trans;
}

State::State() {
	this->Action = 0;
	this->TransNO = 0;
	this->Trans = 0;
}

void State::setAction(FSME_PF Action) {
	this->Action = Action;
}

void State::setTransitions(Transition *Trans, uint8_t TransNO) {
	this->Trans = Trans;
	this->TransNO = TransNO;
}

void State::runAction(void) {
	this->Action();
}

Transition * State::getTransitions(void) {
	return (this->Trans);
}

uint8_t State::getTransitionsNumber(void) {
	return this->TransNO;
}
/*
void State::serialPrint(void) {
	uint8_t i = 0;
	Serial.print("action pointer: ");
	Serial.println((uint16_t)this->Action);
	Serial.print("# of transitions: ");
	Serial.println(this->TransNO);
	do {
		Serial.print("transition # ");
		Serial.print(i);
		Serial.println(" :");
		this->Trans[i].serialPrint();
	} while (++i < this->TransNO);
}
*/
FSME::FSME(uint8_t Enable, uint8_t CurrentState, uint8_t StatesNO, State *States, uint8_t TransNO, Transition *Trans) {
	this->Enable = Enable;
	this->CurrentState = CurrentState;
	this->StatesNO = StatesNO;
	this->StateChanged = 1;
	this->States = States;
	this->TransNO = TransNO;
	this->Trans = Trans;
}

FSME::FSME() {
	this->Enable = 1;
	this->CurrentState = 0;
	this->StatesNO = 0;
	this->StateChanged = 1;
	this->States = 0;
	this->TransNO = 0;
	this->Trans = 0;
}

void FSME::setStates(State *States, uint8_t StatesNO) {
	this->States = States;
	this->StatesNO = StatesNO;
}

void FSME::setInitialState(uint8_t CurrentState) {
	this->CurrentState = CurrentState;
	this->Trans = this->States[CurrentState].getTransitions();
	this->TransNO = this->States[CurrentState].getTransitionsNumber();
}

void FSME::updateState(void) {
	uint8_t _i = 0;
	uint8_t _n;

	_t = this->Trans;
	_n = this->TransNO;

	for (_i = 0; _i < _n; _i++) {
		if (_t[_i].runEvent() == 1) {
			this->CurrentState = _t[_i].getNextState();
			_s = &(this->States[this->CurrentState]);
			this->Trans = _s->getTransitions();
			this->TransNO = _s->getTransitionsNumber();
			this->StateChanged = 1;
			break;
		}
	}
}

void FSME::action(void) {
	this->States[this->CurrentState].runAction();
}

void FSME::run(void) {
	if (this->Enable == 0) {
		return;
	}
	this->updateState();
	this->action();
	this->StateChanged = 0;
}

void FSME::enable(void) {
	this->Enable = 1;
}

void FSME::disable(void) {
	this->Enable = 0;
}

uint8_t FSME::isStateChanged(void) {
	return this->StateChanged;
}

uint8_t FSME::isEnabled(void) {
	return this->Enable;
}
/*
void FSME::serialPrint(void) {
	uint8_t i = 0;
	Serial.print("enabled: ");
	Serial.println(this->Enable);
	Serial.print("current state: ");
	Serial.println(this->CurrentState);
	Serial.print("state changed: ");
	Serial.println(this->StateChanged);
	Serial.print("# of states: ");
	Serial.println(this->StatesNO);
	do {
		Serial.print("state # ");
		Serial.print(i);
		Serial.println(" :");
		this->States[i].serialPrint();
	} while (++i < this->StatesNO);
}*/
