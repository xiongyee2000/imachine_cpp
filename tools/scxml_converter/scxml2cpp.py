#!/usr/bin/python
#coding=utf-8

import getopt
import sys
import os

from xml.dom.minidom import parse
import xml.dom.minidom

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

class State:
    def __init__(self, id, name, isInit = False, subState = None):
        self.id = id
        self.name = name
        self.isInit = isInit
        self.subState = subState
        
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
    def __init__(self):
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
        
        stateElementList = None
        firstElement = None
        foundInitState = False
        
        initStateName = element.getAttribute('initial') #try to find initial as attribute
        if initStateName == '': #try to find initial as child element
            tmpList = getFirstLevelElementsByTagName(element, 'initial')
            if len(tmpList) > 0:
                initStateName = tmpList[0].firstChild.data
                foundInitState = True
            else: #Then the initial shall be the first one
                tmpNode = element.firstChild
                while tmpNode != None:
                    if tmpNode.nodeType == tmpNode.ELEMENT_NODE and (tmpNode.tagName == 'state' or tmpNode.tagName == 'parallel'):
                        initStateName = tmpNode.getAttribute('id')
                        if initStateName == '': #id might not be present according the scxml specification
                            initStateName = '_@_'
                            tmpNode.setAttribute('id', '_@_')
                        foundInitState = True
                        break 
                    tmpNode = tmpNode.nextSibling         
        else:
            foundInitState = True 

        if not foundInitState:
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
            if isSubMachine(stateElement):
                subMachine = StateMachine()
                subMachine.build(stateElement)
                state = State(stateCount, stateName, subMachine)
            else:
                state = State(stateCount, stateName)
            self.stateList.append(state)
            if initStateName == stateName:
                self.initState = state
    
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
                                
pass

def buildSubState(stateElement, stateObject):
    return
pass

def main(argv):    

    header_file_begin = '''
#ifndef $HEADER_MACRO
#define $HEADER_MACRO

#include <string>
#include <list>
#include "org/jinsha/imachine/StateMachine.h"

using namespace org::jinsha::imachine;

class $className : public StateMachine {

public:
'''

    header_file_end = '''
    $className(int id);
    virtual ~$className();

private:
    $className() = delete;
    $className(const ATMStateMachine& instance) = delete;
    $className& operator = (const ATMStateMachine& instance) = delete;

    std::list<StateMachine*> subMachines;
};

#endif
'''    

    cpp_file_begin = '''
#include "$className.h"

$className::$className(int id) :
    StateMachine(id, "$className")
{
'''

    cpp_file_end = '''
};
    
$className::~$className()
{
    for (auto machine : subMachines) {
        delete machine;
    }
};

'''
    
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
    
    rootStateMachine = StateMachine()
    rootStateMachine.build(rootElement, True)
            
    outputHeaderFileName = outputDir + '/' + className + '.h';
    outputCppFileName = outputDir + '/' + className + '.cpp';
    outputHeaderFile = open(outputHeaderFileName, 'w')
    outputCppFile = open(outputCppFileName, 'w')
    headerMacro = className.upper() + '_H_'
    header_file_begin = header_file_begin.replace('$HEADER_MACRO', headerMacro)
    header_file_begin = header_file_begin.replace('$className', className)
    outputHeaderFile.write(header_file_begin)
    
    for state in rootStateMachine.stateList:
        tmpStr  = '    /**\n'
        tmpStr += '     * state %s\n' % (state.name)
        tmpStr += '     */\n'
        tmpStr += '    static const int STATE_%d = %d;\n\n' % (state.id, state.id)
        outputHeaderFile.write(tmpStr)
    outputHeaderFile.write('\n')
    for transition in rootStateMachine.transitionList:
        tmpStr  = '    /**\n'
        tmpStr += '     * transition %s\n' % (transition.name)
        tmpStr += '     */\n'
        tmpStr += '    static const int TRAN_%d = %d;\n\n' % (transition.id, transition.id)
        outputHeaderFile.write(tmpStr)
    outputHeaderFile.write('\n')
    for event in rootStateMachine.eventList:
        tmpStr  = '    /**\n'
        tmpStr += '     * event %s\n' % (event.name)
        tmpStr += '     */\n'
        tmpStr += '    static const int EVENT_%d = %d;\n\n' % (event.id, event.id)
        outputHeaderFile.write(tmpStr)
    outputHeaderFile.write('\n')                 
    
    header_file_end = header_file_end.replace('$className', className)
    outputHeaderFile.write(header_file_end)
    
    cpp_file_begin = cpp_file_begin.replace('$className', className)
    outputCppFile.write(cpp_file_begin)
    
    for state in rootStateMachine.stateList:
        tmpStr = '    addState(STATE_%d, "%s"); \n' % (state.id, state.name)
        outputCppFile.write(tmpStr)
    outputCppFile.write('\n')
    
    tmpStr = '    setInitState(STATE_%d); \n' % (rootStateMachine.initState.id)
    outputCppFile.write(tmpStr)
    outputCppFile.write('\n')
    
    for state in rootStateMachine.stateList:
        for eventMap in state.eventMapList:
            event = eventMap['event']
            transition = eventMap['transition']
        tmpStr = '    addTransition(TRAN_%d, STATE_%d, EVENT_%d, STATE_%d, "%s"); \n' % (transition.id, transition.fromState.id, transition.toState.id, event.id, state.name)
        outputCppFile.write(tmpStr)
    outputCppFile.write('\n')           
    
    cpp_file_end = cpp_file_end.replace('$className', className)
    outputCppFile.write(cpp_file_end)

pass

if __name__ == "__main__":
    main(sys.argv)