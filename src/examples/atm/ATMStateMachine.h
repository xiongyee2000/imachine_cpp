/*
 * ATM.h
 *
 *  Created on: Dec 10, 2017
 *      Author: bear
 */
#ifndef EXAMPLES_ATM_ATMSTATEMACHINE_H_
#define EXAMPLES_ATM_ATMSTATEMACHINE_H_

#include <string>
#include <list>
#include "org/jinsha/imachine/StateMachine.h"

using namespace org::jinsha::imachine;

class ATMStateMachine : public StateMachine {
public:
	static const int STATE_OFF = 1;
	static const int STATE_SELFTEST = 2;
	static const int STATE_IDLE = 3;
	static const int STATE_MAINTENANCE = 4;
	static const int STATE_OUT_OF_SERVICE = 5;
	static const int STATE_SERVING = 6;

	static const int TRAN_Off_to_SelfTest = 1;
	static const int TRAN_SelfTest_to_Idle = 2;
	static const int TRAN_SelfTest_to_OutOfService = 3;
	static const int TRAN_Idle_to_Maintenance = 4;
	static const int TRAN_Idle_to_Servicing = 5;
	static const int TRAN_Idle_to_Off = 6;
	static const int TRAN_Maintenance_to_SelfTest = 7;
	static const int TRAN_Maintenance_to_OutOfService = 8;
	static const int TRAN_OutOfService_to_Maintenance = 9;
	static const int TRAN_OutOfService_to_Off = 10;
	static const int TRAN_Serving_to_Idle_Cancel = 11;
	static const int TRAN_Serving_to_Idle_Done = 12;
	static const int TRAN_Serving_to_OutOfService = 13;

	static const int EVENT_STARTUP = 1;
	static const int EVENT_THUTDOWN = 2;
	static const int EVENT_SUCCESS = 3;
	static const int EVENT_FAILURE = 4;
	static const int EVENT_CANCEL = 5;
	static const int EVENT_SERVICE = 6;
	static const int EVENT_CARD_INSERTED = 7;
	static const int EVENT_AUTH_PASSED = 8;
	static const int EVENT_TRANSACTION_SELECTED = 9;

//	static const std::string STR_EVENT_STARTUP;
//	static const std::string STR_EVENT_THUTDOWN;
//	static const std::string STR_EVENT_SUCCESS;
//	static const std::string STR_EVENT_FAILURE;
//	static const std::string STR_EVENT_CANCEL;
//	static const std::string STR_EVENT_SERVICE;
//	static const std::string STR_EVENT_CARD_INSERTED;

	static const int SUBSTATE_AUTH = 101;
	static const int SUBSTATE_SELECT_TRANSACTION = 102;
	static const int SUBSTATE_TRANSACTION = 103;

	static const int TRAN_AUTH_TO_SELECT_TRANSACTION = 101;
	static const int TRAN_SELECT_TRANSACTION_TO_TRANSACTION = 102;
	static const int TRAN_TRANSACTION_TO_FINAL = 103;

	ATMStateMachine(int id);
	virtual ~ATMStateMachine();

private:
	ATMStateMachine();
	ATMStateMachine(const ATMStateMachine& instance);
	ATMStateMachine& operator = (const ATMStateMachine& instance);

	std::list<StateMachine*> subMachines;
};

#endif /* EXAMPLES_ATM_ATMSTATEMACHINE_H_ */
