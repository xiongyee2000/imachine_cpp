#include <iostream>
#include "ATMStateMachine.h"
#include "org/jinsha/imachine/StateMachineEngine.h"

using namespace std;

int main(int argc, char** argv)
{
	int input = 0;
	bool exitFlag = false;
	int checkValue = -1;
	int transitionId = -1;

	ATMStateMachine atm(1001);

	StateMachineEngine engine(1);
	engine.loadStateMachine(atm);

	StateMachineEngine::StateAction stateEntryAction = [&](const StateMachineEngine* engine, const State* state) mutable -> void {
		checkValue = state->getId();
		printf("[stateEntryAction] State %d entered. \n", checkValue);
	};
	StateMachineEngine::StateAction stateExitAction = [&](const StateMachineEngine* engine, const State* state) mutable -> void {
		printf("[stateExitAction] State %d exited. \n", state->getId());
	};

	StateMachineEngine::TransitionAction transitionAction = [&](const StateMachineEngine* engine, const Transition* transition) mutable -> void {
		transitionId = transition->getId();
		printf("[transitionAction] Transition %d triggered. \n", transitionId);
	};

	auto servingStateMachine = engine.getSubEngine(ATMStateMachine::STATE_SERVING);

	engine.setStateEntryAction(ATMStateMachine::STATE_OFF, &stateEntryAction);
	engine.setStateExitAction(ATMStateMachine::STATE_OFF, &stateExitAction);
	engine.setStateEntryAction(ATMStateMachine::STATE_SELFTEST, &stateEntryAction);
	engine.setStateExitAction(ATMStateMachine::STATE_SELFTEST, &stateExitAction);
	engine.setStateEntryAction(ATMStateMachine::STATE_IDLE, &stateEntryAction);
	engine.setStateExitAction(ATMStateMachine::STATE_IDLE, &stateExitAction);
	engine.setStateEntryAction(ATMStateMachine::STATE_MAINTENANCE, &stateEntryAction);
	engine.setStateExitAction(ATMStateMachine::STATE_MAINTENANCE, &stateExitAction);
	engine.setStateEntryAction(ATMStateMachine::STATE_OUT_OF_SERVICE, &stateEntryAction);
	engine.setStateExitAction(ATMStateMachine::STATE_OUT_OF_SERVICE, &stateExitAction);
	engine.setStateEntryAction(ATMStateMachine::STATE_SERVING, &stateEntryAction);
	engine.setStateExitAction(ATMStateMachine::STATE_SERVING, &stateExitAction);
	servingStateMachine->setStateEntryAction(ATMStateMachine::SUBSTATE_AUTH, &stateEntryAction);
	servingStateMachine->setStateExitAction(ATMStateMachine::SUBSTATE_AUTH, &stateExitAction);
	servingStateMachine->setStateEntryAction(ATMStateMachine::SUBSTATE_SELECT_TRANSACTION, &stateEntryAction);
	servingStateMachine->setStateExitAction(ATMStateMachine::SUBSTATE_SELECT_TRANSACTION, &stateExitAction);
	servingStateMachine->setStateEntryAction(ATMStateMachine::SUBSTATE_TRANSACTION, &stateEntryAction);
	servingStateMachine->setStateExitAction(ATMStateMachine::SUBSTATE_TRANSACTION, &stateExitAction);

	engine.setTransitionAction(ATMStateMachine::TRAN_Off_to_SelfTest, &transitionAction);
	engine.setTransitionAction(ATMStateMachine::TRAN_SelfTest_to_Idle, &transitionAction);
	engine.setTransitionAction(ATMStateMachine::TRAN_SelfTest_to_OutOfService, &transitionAction);
	engine.setTransitionAction(ATMStateMachine::TRAN_Idle_to_Maintenance, &transitionAction);
	engine.setTransitionAction(ATMStateMachine::TRAN_Idle_to_Servicing, &transitionAction);
	engine.setTransitionAction(ATMStateMachine::TRAN_Maintenance_to_SelfTest, &transitionAction);
	engine.setTransitionAction(ATMStateMachine::TRAN_Maintenance_to_OutOfService, &transitionAction);
	engine.setTransitionAction(ATMStateMachine::TRAN_OutOfService_to_Maintenance, &transitionAction);
	engine.setTransitionAction(ATMStateMachine::TRAN_OutOfService_to_Off, &transitionAction);
	engine.setTransitionAction(ATMStateMachine::TRAN_Serving_to_Idle_Cancel, &transitionAction);
	engine.setTransitionAction(ATMStateMachine::TRAN_Serving_to_Idle_Done, &transitionAction);
	engine.setTransitionAction(ATMStateMachine::TRAN_Serving_to_OutOfService, &transitionAction);
	servingStateMachine->setTransitionAction(ATMStateMachine::TRAN_AUTH_TO_SELECT_TRANSACTION, &transitionAction);
	servingStateMachine->setTransitionAction(ATMStateMachine::TRAN_SELECT_TRANSACTION_TO_TRANSACTION, &transitionAction);
	servingStateMachine->setTransitionAction(ATMStateMachine::TRAN_TRANSACTION_TO_FINAL, &transitionAction);

	auto self_test = [&]() mutable -> void {
		printf("[test_1]:\n");
		if (checkValue != ATMStateMachine::STATE_OFF) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_STARTUP);
		if (checkValue != ATMStateMachine::STATE_SELFTEST) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_FAILURE);
		if (checkValue != ATMStateMachine::STATE_OUT_OF_SERVICE) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_THUTDOWN);
		if (checkValue != ATMStateMachine::STATE_OFF) {
			printf("Failed.\n");
			return;
		}
		printf("Passed.\n");

		printf("[test_2]:\n");
		engine.postEvent(ATMStateMachine::EVENT_STARTUP);
		if (checkValue != ATMStateMachine::STATE_SELFTEST) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_SUCCESS);
		if (checkValue != ATMStateMachine::STATE_IDLE) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_THUTDOWN);
		if (checkValue != ATMStateMachine::STATE_OFF) {
			printf("Failed.\n");
			return;
		}
		printf("Passed.\n");

		printf("[test_3]:\n");
		engine.postEvent(ATMStateMachine::EVENT_STARTUP);
		if (checkValue != ATMStateMachine::STATE_SELFTEST) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_SUCCESS);
		if (checkValue != ATMStateMachine::STATE_IDLE) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_SERVICE);
		if (checkValue != ATMStateMachine::STATE_MAINTENANCE) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_FAILURE);
		if (checkValue != ATMStateMachine::STATE_OUT_OF_SERVICE) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_SERVICE);
		if (checkValue != ATMStateMachine::STATE_MAINTENANCE) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_SUCCESS);
		if (checkValue != ATMStateMachine::STATE_SELFTEST) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_SUCCESS);
		if (checkValue != ATMStateMachine::STATE_IDLE) {
			printf("Failed.\n");
			return;
		}
		printf("Passed.\n");

		printf("[test_4]:\n");
		engine.postEvent(ATMStateMachine::EVENT_CARD_INSERTED);
		if (checkValue != ATMStateMachine::SUBSTATE_AUTH) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_CANCEL);
		if (checkValue != ATMStateMachine::STATE_IDLE) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_CARD_INSERTED);
		if (checkValue != ATMStateMachine::SUBSTATE_AUTH) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_FAILURE);
		if (checkValue != ATMStateMachine::STATE_OUT_OF_SERVICE) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_SERVICE);
		if (checkValue != ATMStateMachine::STATE_MAINTENANCE) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_SUCCESS);
		if (checkValue != ATMStateMachine::STATE_SELFTEST) {
			printf("Failed.\n");
			return;
		}
		engine.postEvent(ATMStateMachine::EVENT_SUCCESS);
		if (checkValue != ATMStateMachine::STATE_IDLE) {
			printf("Failed.\n");
			return;
		}
		printf("Passed.\n");

		printf("[test_5]:\n");
		engine.postEvent(ATMStateMachine::EVENT_CARD_INSERTED);
		if (checkValue != ATMStateMachine::SUBSTATE_AUTH) {
			printf("Failed.\n");
			return;
		}
		servingStateMachine->postEvent(ATMStateMachine::EVENT_AUTH_PASSED);
		if (checkValue != ATMStateMachine::SUBSTATE_SELECT_TRANSACTION) {
			printf("Failed.\n");
			return;
		}
		servingStateMachine->postEvent(ATMStateMachine::EVENT_TRANSACTION_SELECTED);
		if (checkValue != ATMStateMachine::STATE_IDLE) {
			printf("Failed.\n");
			return;
		}
		printf("Passed.\n");
	};

	engine.start();

	for (;!exitFlag;)
	{
		printf("1-EVENT_STARTUP\n");
		printf("2-EVENT_THUTDOWN\n");
		printf("3-EVENT_SUCCESS\n");
		printf("4-EVENT_FAILURE\n");
		printf("5-EVENT_CANCEL\n");
		printf("6-EVENT_SERVICE\n");
		printf("7-EVENT_CARD_INSERTED\n");
		printf("8-EVENT_AUTH_PASSED\n");
		printf("9-EVENT_TRANSACTION_SELECT\n");
		printf("100-Exit\n");
		printf("101-Self_Test\n");
		printf("\n[ATM State Machine] Sent Event:\n");

		int dummy = scanf("%d", &input);

		switch (input) {
			case 1:
				engine.postEvent(ATMStateMachine::EVENT_STARTUP);
				break;
			case 2:
				engine.postEvent(ATMStateMachine::EVENT_THUTDOWN);
				break;
			case 3:
				engine.postEvent(ATMStateMachine::EVENT_SUCCESS);
				break;
			case 4:
				engine.postEvent(ATMStateMachine::EVENT_FAILURE);
				break;
			case 5:
				engine.postEvent(ATMStateMachine::EVENT_CANCEL);
				break;
			case 6:
				engine.postEvent(ATMStateMachine::EVENT_SERVICE);
				break;
			case 7:
				engine.postEvent(ATMStateMachine::EVENT_CARD_INSERTED);
				break;
			case 8:
				servingStateMachine->postEvent(ATMStateMachine::EVENT_AUTH_PASSED);
				break;
			case 9:
				servingStateMachine->postEvent(ATMStateMachine::EVENT_TRANSACTION_SELECTED);
				break;
			case 100:
				exitFlag = true;
				break;
			case 101:
				self_test();
				exitFlag = true;
				break;
			default :
				break;
		}
	}

	engine.shutdown();

	return 0;
}
