/*
 * ATM.cpp
 *
 *  Created on: Dec 10, 2017
 *      Author: bear
 */

#include <ATMStateMachine.h>

//const std::string ATM::STR_EVENT_STARTUP = "StartUp";
//const std::string ATM::STR_EVENT_THUTDOWN = "ShutDown";
//const std::string ATM::STR_EVENT_SUCCESS = "Success";
//const std::string ATM::STR_EVENT_FAILURE = "Failure";
//const std::string ATM::STR_EVENT_CANCEL = "Cancel";
//const std::string ATM::STR_EVENT_SERVICE = "Service";
//const std::string ATM::STR_EVENT_CARD_INSERTED = "CardInserted";

ATMStateMachine::ATMStateMachine(int id) :
	StateMachine(id, "ATM")
{
	addState(STATE_OFF, "Off");
	addState(STATE_SELFTEST, "SelfTest");
	addState(STATE_IDLE, "Idle");
	addState(STATE_MAINTENANCE, "Maintenance");
	addState(STATE_OUT_OF_SERVICE, "OutOfService");
	addState(STATE_SERVING, "Serving");

	setInitState(STATE_OFF);

	addTransition(TRAN_Off_to_SelfTest,
			STATE_OFF,
			STATE_SELFTEST,
			EVENT_STARTUP,
			"Off_to_SelfTest");
	addTransition(TRAN_SelfTest_to_Idle,
			STATE_SELFTEST,
			STATE_IDLE,
			EVENT_SUCCESS,
			"SelfTest_to_Idle");
	addTransition(TRAN_SelfTest_to_OutOfService,
			STATE_SELFTEST,
			STATE_OUT_OF_SERVICE,
			EVENT_FAILURE,
			"SelfTest_to_Maintenance");
	addTransition(TRAN_Idle_to_Maintenance,
			STATE_IDLE,
			STATE_MAINTENANCE,
			EVENT_SERVICE,
			"Idle_to_Maintenance");
	addTransition(TRAN_Idle_to_Servicing,
			STATE_IDLE,
			STATE_SERVING,
			EVENT_CARD_INSERTED,
			"Idle_to_Servicing");
	addTransition(TRAN_Idle_to_Off,
			STATE_IDLE,
			STATE_OFF,
			EVENT_THUTDOWN,
			"Idle_to_Off");
	addTransition(TRAN_Maintenance_to_SelfTest,
			STATE_MAINTENANCE,
			STATE_SELFTEST,
			EVENT_SUCCESS,
			"Maintenance_to_SelfTest");
	addTransition(TRAN_Maintenance_to_OutOfService,
			STATE_MAINTENANCE,
			STATE_OUT_OF_SERVICE,
			EVENT_FAILURE,
			"Maintenance_to_OutOfService");
	addTransition(TRAN_OutOfService_to_Maintenance,
			STATE_OUT_OF_SERVICE,
			STATE_MAINTENANCE,
			EVENT_SERVICE,
			"OutOfService_to_Maintenance");
	addTransition(TRAN_OutOfService_to_Off,
			STATE_OUT_OF_SERVICE,
			STATE_OFF,
			EVENT_THUTDOWN,
			"OutOfService_to_Off");
	addTransition(TRAN_Serving_to_Idle_Cancel,
			STATE_SERVING,
			STATE_IDLE,
			EVENT_CANCEL,
			"Servicing_to_Idle_Cancel");
	addTransition(TRAN_Serving_to_Idle_Done,
			STATE_SERVING,
			STATE_IDLE,
			EVENT_SUCCESS,
			"Servicing_to_Idle_Done");
	addTransition(TRAN_Serving_to_OutOfService,
			STATE_SERVING,
			STATE_OUT_OF_SERVICE,
			EVENT_FAILURE,
			"Servicing_to_OutOfService");

	//Sub-machine
	StateMachine* servingStateMachine = new StateMachine(10011);
	subMachines.push_back(servingStateMachine);
	setSubMachine(STATE_SERVING, servingStateMachine, TRAN_Serving_to_Idle_Done);

	servingStateMachine->addState(SUBSTATE_AUTH);
	servingStateMachine->addState(SUBSTATE_SELECT_TRANSACTION);
	servingStateMachine->addState(SUBSTATE_TRANSACTION);
	servingStateMachine->addState(State::FINAL_STATE_ID);

	servingStateMachine->setInitState(SUBSTATE_AUTH);

	servingStateMachine->addTransition(TRAN_AUTH_TO_SELECT_TRANSACTION,
			SUBSTATE_AUTH,
			SUBSTATE_SELECT_TRANSACTION,
			EVENT_AUTH_PASSED,
			"TRAN_AUTH_TO_SELECT_TRANSACTION");
	servingStateMachine->addTransition(TRAN_SELECT_TRANSACTION_TO_TRANSACTION,
			SUBSTATE_SELECT_TRANSACTION,
			SUBSTATE_TRANSACTION,
			EVENT_TRANSACTION_SELECTED,
			"TRAN_SELECT_TRANSACTION_TO_TRANSACTION");
	servingStateMachine->addTransition(TRAN_TRANSACTION_TO_FINAL,
			SUBSTATE_TRANSACTION,
			State::FINAL_STATE_ID,
			"TRAN_TRANSACTION_TO_FINAL");
}

ATMStateMachine::~ATMStateMachine()
{
	for (auto machine : subMachines) {
		delete machine;
	}
}

