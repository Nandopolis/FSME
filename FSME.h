/*
 * @file FSME.h
 * @author Nandopolis 
 * @date May 12, 2017
 * @brief Libray containing Transition, EvnTransition, TimeTransition and FSME classes
 * 
 * This library tries to implement a FSM kernel for embedded systems, is based
 * on http://www.ace.tuiasi.ro/users/103/Bind4.pdf, which is implemented in c,
 * this library focuses on an object oriented implementation and adds a new
 * type of transition, a timed transition.
 * 
 * For use refer to examples in Examples folder, this examples are implemented
 * on Arduino
 */

#ifndef FSME_H_
#define FSME_H_

#include <stdint.h>

typedef void (*FSME_PF)(void);
typedef uint8_t (*FSME_PF_EV)(void);

/**
 * @brief Generic transition class
 * 
 * This class is used as a base class for EvnTransition and TimeTransition
 * classes. It has common variables as #NextState (which is used in all transition
 * types) and #Active (which is used only on timed transitions), also implements
 * methods to access the NextState variable
 */
class Transition {
  	private:
		uint8_t NextState; /**< The index of the next state in the #States array passed to the fsm */

  	public:
		uint8_t Active; /**< An indicator for the TimeTransition #InitialTime initialization */

		/**
		 * @brief Set the NextState variable
		 * 
		 * @param NextState 
		 */
		void setNextState(uint8_t NextState);

		/**
		 * @brief Virtual function
		 * 
		 * Checks if the transition takes place
		 * Implemented in derived classes
		 * 
		 * @return uint8_t returns 1 if the transition takes place, 0 otherwise
		 */
		virtual uint8_t runEvent(void);

		/**
		 * @brief Get the NextState variable
		 * 
		 * @return uint8_t the value of NextState variable 
		 */
		uint8_t getNextState(void);
};

/**
 * @brief Event Transition class
 * 
 * This class implements the type of transition described on the paper listed
 * on the README.md, it is a transition that takes a pointer to a function that
 * determines if the transition takes place or not
 */
class EvnTransition : public Transition {
	private:
		FSME_PF_EV Event; /**< A pointer to a function that checks if the transition takes place */

	public:
		/**
		 * @brief Construct a new EvnTransition object
		 * 
		 * @param Event a pointer to an event function 
		 * @param NextState the index of the next state
		 */
		EvnTransition(FSME_PF_EV Event, uint8_t NextState);

		/**
		 * @brief Run the event function
		 * 
		 * Runs the event function that is pointed by #Event, that function
		 * determines if the transition takes place or not
		 * 
		 * @return uint8_t returns 1 if the transition takes place, 0 otherwise
		 */
		uint8_t runEvent(void);

		//void serialPrint(void);
};

/**
 * @brief Time Transition class
 * 
 * This class implements a timed transition defined by a time out value
 */
class TimeTransition : public Transition {
	private:
		uint16_t TimeOut; /**< The time out in miliseconds */
		uint32_t InitialTime; /**< timestamp of the first call to the runEvent() function */

	public:
		/**
		 * @brief Construct a new TimeTransition object
		 * 
		 * @param TimeOut time out in miliseconds
		 * @param NextState the index of the next state
		 */
		TimeTransition(uint16_t TimeOut, uint8_t NextState);

		/**
		 * @brief Process the time event
		 * 
		 * Determines if the #TimeOut time has elapsed, if is the first time
		 * also sets the #InitialTime variable
		 * 
		 * @return uint8_t returns 1 if the time out has elapsed, 0 otherwise
		 */
		uint8_t runEvent(void);

		//void serialPrint(void);
};

/**
 * @brief State class
 * 
 * This class implements the data related to a state, e.g. a pointer to the
 * function that should bu runned while the state is active and all the possible
 * trasitions from the state
 */
class State {
	private:
		FSME_PF Action; /**< A pointer to the function runned while the state is active */
		uint8_t TransNO; /**< The number of possible transitions */
		Transition **Trans; /**< An array of pointers to all the possible transitions */

	public:
		/**
		 * @brief Construct a new State object
		 * 
		 * @param Action The function that will be runned while the state is active
		 * @param TransNO The number of possible transitions
		 * @param Trans An array of pointers to the possible transitions
		 */
		State(FSME_PF Action, uint8_t TransNO, Transition **Trans);

		/**
		 * @brief Construct a new empty State object
		 * 
		 */
		State();

		/**
		 * @brief Set the State parameters
		 * 
		 * @param Action The function that will be runned while the state is active
		 * @param Trans An array of pointers to the possible transitions
		 * @param TransNO The number of possible transitions
		 */
		void setState(FSME_PF Action, Transition ** Trans, uint8_t TransNO);

		/**
		 * @brief Set the action function 
		 * 
		 * @param Action THe functionthat will be runned while the state is active
		 */
		void setAction(FSME_PF Action);
		
		/**
		 * @brief Set the possible transitions
		 * 
		 * @param Trans An array of pointers to the possible transitions
		 * @param TransNO The number of possible transitions
		 */
		void setTransitions(Transition ** Trans, uint8_t TransNO);

		/**
		 * @brief Set the Active variable
		 * 
		 * Sets the #Active variable in all the transitions, this function is
		 * called when a state change occurs, used to reset the
		 * #InitialTime timestamp
		 */
		void setActive();

		/**
		 * @brief Run the action function
		 * 
		 * Runs the function pointed by #Action every time the FSME::run()
		 * method is called if this state is the current state
		 */
		void runAction();

		/**
		 * @brief Get Transitions
		 * 
		 * @return Transition** The array of pointers to the transitions
		 */
		Transition **getTransitions(void);

		/**
		 * @brief Get the number of transitions
		 * 
		 * @return uint8_t the value of #TransNO
		 */
		uint8_t getTransitionsNumber(void);

		//void serialPrint(void);
};

/**
 * @brief FSME class
 * 
 * The Finite State Machine class, implements the interaction between states
 * and transitions
 */
class FSME {
	private:
		Transition **_t = 0; /**< The transitions of the current state */
		State *_s = 0; /**< A temporal pointer to the current state */
		uint8_t Enable; /**< Controls if the FSM should run or not */
		uint8_t CurrentState; /**< The index of the current state in the #States array */
		uint8_t StatesNO; /**< The number os=f states */
		uint8_t StateChanged; /**< Indicates if a state has recently changed */
		State *States; /**< An array of states that conform the FSM */
		uint8_t TransNO; /**< The number of transitions of the current state */
		Transition **Trans; /**< An array of pointers to the transitions of the current state */

		/**
		 * @brief Update State
		 * 
		 * Checks if a state change should take place and change it if it does
		 */
		void updateState(void);

		/**
		 * @brief Action
		 * 
		 * Runs the action function of the current state
		 */
		void action(void);

	public:
		/**
		 * @brief Construct a new FSME object
		 * 
		 * @param CurrentState The index of the current state in the @p States array
		 * @param StatesNO The number of states
		 * @param States The array of states
		 * @param TransNO The number of transitions of the current state
		 * @param Trans The array of pointers to the possible transitions of the current state
		 */
		FSME(uint8_t CurrentState, uint8_t StatesNO, State *States, uint8_t TransNO, Transition **Trans);
		
		/**
		 * @brief Construct a new empty FSME object
		 * 
		 */
		FSME();

		/**
		 * @brief Set the initial state
		 * 
		 * Set the #CurretState variable, its transitions and the number of 
		 * transitions
		 * 
		 * @param CurrentState The index of the current state in the #States array
		 */
		void setInitialState(uint8_t CurrentState);

		/**
		 * @brief Set the States
		 * 
		 * Sets the States that conform the FSM
		 * 
		 * @param States An array of State
		 * @param StatesNO the number of states
		 */
		void setStates(State *States, uint8_t StatesNO);

		/**
		 * @brief Run
		 * 
		 * Runs the FSM, this function should be called regularly.
		 * calls FSME::updateState() and FSME::action() methods if the FSM is
		 * enabled
		 */
		void run(void);

		/**
		 * @brief Enable the FSM
		 * 
		 */
		void enable(void);

		/**
		 * @brief Disable the FSM
		 * 
		 */
		void disable(void);

		/**
		 * @brief is state changed?
		 * 
		 * Checks if a state has changed recently, it can be used in an
		 * action function in order to run code only once.
		 * 
		 * @return uint8_t returns 1 if a state has changed, 0 otherwise
		 */
		uint8_t isStateChanged(void);

		/**
		 * @brief CHeck if the FSM is enable
		 * 
		 * @return uint8_t returns 1 if the FSM is enabled, 0 otherwise
		 */
		uint8_t isEnabled(void);

		//void serialPrint(void);
};

#endif /* FSME_H_ */
