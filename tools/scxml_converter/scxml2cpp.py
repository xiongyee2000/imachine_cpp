#!/usr/bin/python
# -*- coding: UTF-8 -*-

import getopt
import sys
import os

from xml.dom.minidom import parse
import xml.dom.minidom

def main(argv):    
    scxmlFilePath = None
    outputDir = None
    namespace = None
    
    usage = '''
Usage:
<command> <scxmlFile> <outputDir> [options]

scxmlFile: The scxml file path
outputDir: The output directory path after conversion

options: 
-n,          --ns: The base namespace of the classes converted to.
-h,         --help: Usage
'''    
    
    if len(sys.argv) < 3:
        print usage
        return -1
    scxmlFilePath = sys.argv[1]
    outputDir = sys.argv[2]

    try:
        opts, args = getopt.getopt(sys.argv[3:], "n:h", ["ns=", "help"]) 
        for a,o in opts: 
            if a in ('-n', '--ns'):
                namespace = str(o)
            elif a in ('-h', '--help'): 
                print usage
                return 0
    except:
        print "Invalid argument(s)."
        print usage
        return -2
    
    if (not os.path.exists(scxmlFilePath)):
        print "scxmlFile does not exist." 
        return 1
    
    domTree = xml.dom.minidom.parse(scxmlFilePath)
    rootElement = domTree.documentElement
    
    for node in rootElement.childNodes:
        if node.nodeType == node.ELEMENT_NODE and node.tagName == 'state':
            print node.getAttribute('id')

pass

if __name__ == "__main__":
    main(sys.argv)