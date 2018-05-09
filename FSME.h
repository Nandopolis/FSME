/*
 * FSME.h
 *
 *  Created on: May 12, 2017
 *      Author: Nandopolis
 */

#ifndef FSME_H_
#define FSME_H_

#include <stdint.h>

extern uint32_t actualTime(void);

typedef void(*FSME_PF) (void);
typedef uint8_t(*FSME_PF_EV) (void);

class Transition {
private:
	uint8_t NextState;

public:
	uint8_t Active;
	void setNextState(uint8_t);
	virtual uint8_t runEvent(void);
	uint8_t getNextState(void);
};

class EvnTransition: public Transition {
private:
	FSME_PF_EV Event;

public:
	EvnTransition(FSME_PF_EV, uint8_t);
	EvnTransition();

	void setEvnTransition(FSME_PF_EV, uint8_t);
	void setEvent(FSME_PF_EV);
	uint8_t runEvent(void);

	//void serialPrint(void);
};

class TimeTransition: public Transition {
private:
	uint16_t TimeOut;
	uint32_t InitialTime;

public:
	TimeTransition(uint16_t, uint8_t);
	TimeTransition();

	void setTimeTransition(uint16_t, uint8_t);
	uint8_t runEvent(void);

	//void serialPrint(void);
};

class State {
private:
	FSME_PF Action;
	uint8_t TransNO;
	Transition **Trans;

public:
	State(FSME_PF, uint8_t, Transition **);
	State();

	void setState(FSME_PF, Transition **, uint8_t);
	void setAction(FSME_PF Action);
	void setTransitions(Transition **, uint8_t);
	void setActive();
	void runAction();
	Transition ** getTransitions(void);
	uint8_t getTransitionsNumber(void);

	//void serialPrint(void);
};

class FSME {
private:
	Transition **_t = 0;
	State *_s = 0;
	uint8_t Enable;
	uint8_t CurrentState;
	uint8_t StatesNO;
	uint8_t StateChanged;
	State *States;
	uint8_t TransNO;
	Transition **Trans;

	void updateState(void);
	void action(void);

public:
	FSME(uint8_t, uint8_t, uint8_t, State *, uint8_t, Transition **);
	FSME();

	void setInitialState(uint8_t CurState);
	void setStates(State *, uint8_t);

	void run(void);
	void enable(void);
	void disable(void);
	uint8_t isStateChanged(void);
	uint8_t isEnabled(void);

	//void serialPrint(void);
};

#endif /* FSME_H_ */
