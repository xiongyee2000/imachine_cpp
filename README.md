# I-MACHINE
A State Machine Framework

# Overall

I-Machine is aimed to provide a state machine framework that makes the state machine more independent and reusable, and makes the state machine based application more powerful and flexible.

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

# Value

By using NSM and DAR the transition logic of a state machine is totally decoupled from user action logic of an application; and by decoupling the transition logic from the user action logic, the framework improves the flexibility and reusability of a state machine.
