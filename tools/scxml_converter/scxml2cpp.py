#!/usr/bin/python
# -*- coding: UTF-8 -*-

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
    ATMStateMachine(int id);
    virtual ~ATMStateMachine();

private:
    ATMStateMachine();
    ATMStateMachine(const ATMStateMachine& instance);
    ATMStateMachine& operator = (const ATMStateMachine& instance);

    std::list<StateMachine*> subMachines;
};

#endif
'''

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

    domTree = xml.dom.minidom.parse(scxmlFilePath)
    rootElement = domTree.documentElement
    initStateId = None
    
    if rootElement.nodeType == rootElement.ELEMENT_NODE and rootElement.tagName == 'scxml':
        initStateId = rootElement.getAttribute('initial')
    else:
        print 'This is not a scxml file.'
        return 2
    
    if (not os.path.exists(outputDir)):
        os.mkdir(outputDir, 0755)
        if (not os.path.exists(outputDir)):
            print "Failed to create outputDir: " + outputDir
            return 3
    
    outputHeaderFileName = outputDir + '/' + className + '.h';
    outputCppFileName = outputDir + '/' + className + '.cpp';
    outputHeaderFile = open(outputHeaderFileName, 'w')
    outputCppFile = open(outputCppFileName, 'w')
    headerMacro = className.upper() + '_H_'
    global header_file_begin
    header_file_begin = header_file_begin.replace('$HEADER_MACRO', headerMacro)
    header_file_begin = header_file_begin.replace('$className', className)
    
    outputHeaderFile.write(header_file_begin)
    
    stateElementList = getFirstLevelElementsByTagName(rootElement, 'state')
    index = 0
    for stateElement in stateElementList:
        index += 1
        id = stateElement.getAttribute('id')
        if id == None:
            id = str(index)
        #transitionElementList = getTransitionElements(stateElement)
        if isSubMachine(stateElement):
            a = 1
            #createSubMachine(stateElement);
        outputHeaderFile.write('    static const int STATE_%s = %d;\n' % (id, index))
            
    outputHeaderFile.write(header_file_end)

pass

if __name__ == "__main__":
    main(sys.argv)