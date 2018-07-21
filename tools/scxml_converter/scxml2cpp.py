#!/usr/bin/python
#coding=utf-8

import getopt
import sys
import os

from xml.dom.minidom import parse
import xml.dom.minidom
from inspect import isroutine


headerFileBegin = '''
#ifndef $HEADER_MACRO
#define $HEADER_MACRO

#include <string>
#include <list>
#include "org/jinsha/imachine/StateMachine.h"

using namespace org::jinsha::imachine;
'''
    
classDefBegin = '''

class $className : public StateMachine {

public:
'''

classDefEnd = '''
    $className(int id);
    virtual ~$className();

private:
    $className() = delete;
    $className(const ATMStateMachine& instance) = delete;
    $className& operator = (const ATMStateMachine& instance) = delete;

    std::list<StateMachine*> subMachines;
};'''

headerFileEnd = '''

#endif
'''    

cppFileBegin = '''
#include "$className.h"
'''
classImplBegin = '''

$className::$className(int id) :
    StateMachine(id, "$className")
{
'''

classImplEnd = '''
};
    
$className::~$className()
{
    for (auto machine : subMachines) {
        delete machine;
    }
};
'''

cppFileEnd = '''

'''    

def getFirstLevelElementsByTagName(node, tagName):
    elementList = []
    for childNode in node.childNodes:
        if childNode.nodeType == childNode.ELEMENT_NODE and childNode.tagName == tagName:
            elementList.append(childNode)
    return elementList
pass

def isSubMachine(stateElement):
    return False
pass

def getTransitionElements(stateElement):
    return []
pass

#If an element has sub state sub element
def hasSubState(element):
    tmpList = getFirstLevelElementsByTagName(element, 'state')
    if len(tmpList) > 0:
        return True
    else:
        tmpList = getFirstLevelElementsByTagName(element, 'parallel')
        if len(tmpList) > 0:
            return True
        else:
            tmpList = getFirstLevelElementsByTagName(element, 'final')
            if len(tmpList) > 0:
                return True            
    
    return False   
pass

#Get initial state element of a (scxml/state/parallel) element
def getInitialStateElement(element):
    if not hasSubState(element):
        return None
    initialStateElement = None
    initStateId = element.getAttribute('initial') #try to find initial as attribute
    if initStateId != '':
        foundInitState = True 
    else: #try to find initial as child element
        tmpList = getFirstLevelElementsByTagName(element, 'initial')
        if len(tmpList) > 0:
            initStateId = tmpList[0].firstChild.data
            foundInitState = True
        else: #Then the initial shall be the first one
            tmpNode = element.firstChild
            while tmpNode != None:
                if tmpNode.nodeType == tmpNode.ELEMENT_NODE and (tmpNode.tagName == 'state' or tmpNode.tagName == 'parallel'):
#                     initStateId = tmpNode.getAttribute('id')
#                     if initStateId == '': #id might not be present according the scxml specification
#                         initStateId = '_@_'
#                         tmpNode.setAttribute('id', '_@_')
                    initialStateElement = tmpNode;
                    foundInitState = True
                    return  initialStateElement
                tmpNode = tmpNode.nextSibling

    if not foundInitState:
        #print '[ERROR] No initial state is found.'
        return None
    
    tmpList =  getFirstLevelElementsByTagName(element, "state")
    if len(tmpList) == 0:
        tmpList =  getFirstLevelElementsByTagName(element, "parallel")
    for item in tmpList:
        if initStateId == item.getAttribute('id'):
            initialStateElement = item 
            break 
    return initialStateElement
pass

class State:
    def __init__(self, id, name, parent, subMachine = None):
        self.id = id
        self.name = name
        self.parent = parent
        self.isInit = False
        self.subMachine = subMachine
        if subMachine != None:
            subMachine.parent = self
        
        self.eventMapList = []
pass

class Transition:
    def __init__(self, id, name, fromState, toState):
        self.id = id
        self.name = name
        self.fromState = fromState
        self.toState = toState
pass

class Event:
    def __init__(self, id, name):
        self.id = id
        self.name = name
pass

class StateMachine:
    def __init__(self, className):
        self.parent = None
        self.className = className
        self.stateList = []
        self.transitionList = []
        self.eventList = []
        self.initState = None
        
    def getStateByName(self, name):
        for item in self.stateList:
            if item.name == name:
                return item
        return None
        
    def getEventByName(self, name):
        for item in self.eventList:
            if item.name == name:
                return item
        return None
        
    def build(self, element, isRoot = False):
        self.stateList = []
        self.transitionList = []
        self.eventList = []
        self.subMachineList = []
        
        stateElementList = None
        firstElement = None
        foundInitState = False
        
        initStateElement = getInitialStateElement(element)
        if initStateElement == None:
            print '[ERROR] No initial state is found.'
            return 4
        
        stateElementList = getFirstLevelElementsByTagName(element, 'state')
#         if isRoot:
#             stateElementList = []
#             stateElementList.append(element)
#         else:
#             stateElementList = getFirstLevelElementsByTagName(element, 'state')
        
        ################################################################
        #Generate states
        ################################################################               
        stateCount = 0
        for stateElement in stateElementList:
            stateCount += 1
            state = None
            stateName = stateElement.getAttribute('id')
            if stateName == '':
                stateName = str(stateCount)
            if hasSubState(stateElement):
                subMachine = StateMachine(self.className + "_" + stateName)
                subMachine.build(stateElement)
                state = State(stateCount, stateName, self, subMachine)
                self.subMachineList.append(subMachine)
            else:
                state = State(stateCount, stateName, self)
            self.stateList.append(state)
            if stateElement == initStateElement:
                self.initState = state
                state.isInit = True
    
        ################################################################
        #Generate transitions and event maps
        ################################################################               
        transitionCount = 0
        eventCount = 0
        stateIndex = -1
        for stateElement in stateElementList:
            stateIndex += 1
            transitionElementList = getFirstLevelElementsByTagName(stateElement, 'transition')
            for transitionElement in transitionElementList:
                transitionCount += 1
                targetStateName = transitionElement.getAttribute('target')
                if targetStateName == '':
                    print 'transition without a target not support.'
                    exit -1
                sourceState = self.getStateByName(self.stateList[stateIndex].name)
                targetState = self.getStateByName(targetStateName)
                transition = Transition(transitionCount, targetStateName, sourceState, targetState)
                self.transitionList.append(transition)
                eventName = transitionElement.getAttribute('event')
                if eventName != '':
                    event = self.getEventByName(eventName)
                    if event == None:
                        eventCount += 1
                        event = Event(eventCount, eventName)
                        self.eventList.append(event)
                    sourceState.eventMapList.append({'event':event, 'transition': transition})
                else: #eventless transition
                    event = Event(-1, eventName)
                    sourceState.eventMapList.append({'event':event, 'transition': transition})
                    
    def outputClassDef(self, outputHeaderFile):
        global classDefBegin
        myClassDefBegin = classDefBegin.replace('$className', self.className)
        outputHeaderFile.write(myClassDefBegin)
        for state in self.stateList:
            tmpStr  = '    /**\n'
            tmpStr += '     * state %s\n' % (state.name)
            tmpStr += '     */\n'
            tmpStr += '    static const int STATE_%d = %d;\n\n' % (state.id, state.id)
            outputHeaderFile.write(tmpStr)
        outputHeaderFile.write('\n')
        for transition in self.transitionList:
            tmpStr  = '    /**\n'
            tmpStr += '     * transition %s\n' % (transition.name)
            tmpStr += '     */\n'
            tmpStr += '    static const int TRAN_%d = %d;\n\n' % (transition.id, transition.id)
            outputHeaderFile.write(tmpStr)
        outputHeaderFile.write('\n')
        for event in self.eventList:
            tmpStr  = '    /**\n'
            tmpStr += '     * event %s\n' % (event.name)
            tmpStr += '     */\n'
            tmpStr += '    static const int EVENT_%d = %d;\n\n' % (event.id, event.id)
            outputHeaderFile.write(tmpStr)
        outputHeaderFile.write('\n')       
        
        global classDefEnd
        myClassDefEnd = classDefEnd.replace('$className', self.className)
        outputHeaderFile.write(myClassDefEnd)
        outputHeaderFile.write('\n')       

        for subMachine in self.subMachineList:
            subMachine.outputClassDef(outputHeaderFile)
        outputHeaderFile.write('\n')       
        
    pass
        
    def outputClassImpl(self, outputCppFile):
        global classImplBegin 
        myClassImplBegin = classImplBegin.replace('$className', self.className)
        outputCppFile.write(myClassImplBegin)
        
        for state in self.stateList:
            tmpStr = '    addState(STATE_%d, "%s"); \n' % (state.id, state.name)
            outputCppFile.write(tmpStr)
        outputCppFile.write('\n')
        
        tmpStr = '    setInitState(STATE_%d); \n' % (self.initState.id)
        outputCppFile.write(tmpStr)
        outputCppFile.write('\n')
        
        for state in self.stateList:
            for eventMap in state.eventMapList:
                event = eventMap['event']
                transition = eventMap['transition']
                tmpStr = '    addTransition(TRAN_%d, STATE_%d, STATE_%d, EVENT_%d, "%s"); \n' % (transition.id, transition.fromState.id, transition.toState.id, event.id, transition.fromState.name + "->" + transition.toState.name)
                outputCppFile.write(tmpStr)
        outputCppFile.write('\n')           
        
        for subMachine in self.subMachineList:
            tmpStr = '    {\n'
            tmpStr += '        StateMachine* subMachine = new %s(0, %s);\n' % (subMachine.className, subMachine.className)
            tmpStr += '        setSubMachine(STATE_%d, subMachine, true); \n' % (subMachine.parent.id)
            tmpStr += '        subMachines.push_back(subMachine); \n'
            tmpStr += '    };\n'
            outputCppFile.write(tmpStr)
            
        global classImplEnd
        myClassImplEnd  = classImplEnd.replace('$className', self.className)
        outputCppFile.write(myClassImplEnd)
        outputCppFile.write('\n')           
         
        for subMachine in self.subMachineList:
            subMachine.outputClassImpl(outputCppFile)
        
    pass    
                                
pass



def main(argv):    

    
    scxmlFilePath = None
    outputDir = None
    namespace = None
    className = 'NoNameStateMachine'
    
    usage = '''
Usage:
<command> <scxmlFile> <outputDir> [options]

scxmlFile: The scxml file path
outputDir: The output directory path after conversion

options: 
-n,    --ns: The base namespace of the classes converted to.
-c,    --className: The state machine class name after conversion.
-h,    --help: Usage
'''    

    ################################################################
    #Process command line arguments
    ################################################################   
    if len(sys.argv) < 3:
        print usage
        return -1
    scxmlFilePath = sys.argv[1]
    outputDir = sys.argv[2]
    if outputDir.endswith('/') or outputDir.endswith('\\'):
        outputDir = outputDir[:-1]

    try:
        opts, args = getopt.getopt(sys.argv[3:], "n:c:h", ["ns=", "className=", "help"]) 
        for a,o in opts: 
            if a in ('-n', '--ns'):
                namespace = str(o)
            elif a in ('-h', '--help'): 
                print usage
                return 0
            elif a in ('-c', '--className'):
                className = str(o)
                className = className.replace(' ', '')
    except:
        print "Invalid argument(s)."
        print usage
        return -2
    
    if (not os.path.exists(scxmlFilePath)):
        print "scxmlFile does not exist." 
        return 1

    ################################################################
    #Parse input scxml file
    ################################################################   
    domTree = xml.dom.minidom.parse(scxmlFilePath)
    rootElement = domTree.documentElement
    initStateName = None
    
    #if rootElement.nodeType == rootElement.ELEMENT_NODE and rootElement.tagName == 'scxml':
    if rootElement.tagName != 'scxml':
#         initStateName = rootElement.getAttribute('initial')
#         if initStateName == '':
#             tmpNode = rootElement.firstChild
#             while tmpNode != None:
#                 if tmpNode.nodeType == tmpNode.ELEMENT_NODE and (tmpNode.tagName == 'state' or tmpNode.tagName == 'parallel'):
#                     initStateName = tmpNode.getAttribute('id')
#                     break
#                 else:
#                    tmpNode = tmpNode.nextSibling 
#             if initStateName == '':
#                 print '[ERROR] No initial state is found.'
#                 return 4
#    else:
        print 'This is not a scxml file.'
        return 2
    
    if (not os.path.exists(outputDir)):
        os.mkdir(outputDir, 0755)
        if (not os.path.exists(outputDir)):
            print "Failed to create outputDir: " + outputDir
            return 3
    
    rootStateMachine = StateMachine(className)
    rootStateMachine.build(rootElement, True)
            
    outputHeaderFileName = outputDir + '/' + className + '.h';
    outputCppFileName = outputDir + '/' + className + '.cpp';
    outputHeaderFile = open(outputHeaderFileName, 'w')
    outputCppFile = open(outputCppFileName, 'w')
    headerMacro = className.upper() + '_H_'
    global headerFileBegin
    headerFileBegin = headerFileBegin.replace('$HEADER_MACRO', headerMacro)
    headerFileBegin = headerFileBegin.replace('$className', className)
    outputHeaderFile.write(headerFileBegin)
    rootStateMachine.outputClassDef(outputHeaderFile)
    
    global cppFileBegin
    cppFileBegin = cppFileBegin.replace('$className', className)
    outputCppFile.write(cppFileBegin)
    rootStateMachine.outputClassImpl(outputCppFile)
    global headerFileEnd
    outputHeaderFile.write(headerFileEnd)
    global cppFileEnd
    outputHeaderFile.write(cppFileEnd)
    


pass

if __name__ == "__main__":
    main(sys.argv)