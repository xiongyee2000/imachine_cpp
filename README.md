# I-MACHINE
A State Machine Framework

# Overall

I-Machine is aimed to provide a state machine framework that enables machine self-learning and machine intelligence.

# Introduction

## State Machine

A state machine can be viewed as:

A set of states,
A set of transitions,
A set of input events,
A set of actions,
where states and transitions represent the inherent “model of change” of an object, and actions usually represent user desired behaviors upon the change of the object.

## State Machine Engine

In a certain state, a state machine accepts specific input events and triggers corresponding transitions respectively; and when a transition is triggered by one of these input events, corresponding actions are performed respectively. The State Machine Engine (SME) is designed in this framework to implement this mechanism in a unified way.

## NSM & DAR

Different from many other state machine frameworks, e.g. the one in RoseRT, the SME in this framework uses the NSM and the DAR to drive a state machine. NSM is the acronym for Naked State Machine, which is a state machine that has no action. DAR is the acronym for Dynamic Action Registration, which is a mechanism that allows a user hook desired actions to the NSM.

## State Machine Runtime Dynamic

Normally a state machine is predefined by users. But advanced machine intelligence requires a state machine be able to "evolve" itself when the environment changes. The I-Machine framework is aimed to provide the capability for state machine to change its states/transitions/event-handling etc. at runtime.

# Values

* By using NSM and DAR the transition logic of a state machine is totally decoupled from user action logic of an application; and by decoupling the transition logic from the user action logic, the framework improves the flexibility and reusability of a state machine.
* With the State Machine Runtime Dynamic feature, the framework constructs a fundamental basis for the advanced machine learning and machine intelligence.

# Build

* Go to project root directory
* Run ./build.sh
* After build is successfully done:
  * the header files will be generated at ./dist/
  * the library will be generated at ./dist/lib/
  * the demo application will be generated at ./dist/bin/atm

# Build Eclipse Project

  * Go to project root directory
  * Run ./build-eclipse.sh
  * After build is successfully done, the eclipse project will be generated at ./eclipse_project, and you can import it in your Eclipse.

# Build UT (based on googletest)

* Go to project root directory
* Run ./build-ut.sh
* After build is successfully done, the ut applicatioin will be generated at ./ut_build/bin/imachine_ut, and can run it to check the UT result.



