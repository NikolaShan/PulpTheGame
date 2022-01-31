#!/usr/bin/python

import os
import re
import plistlib


FIRST_CAP_RE = re.compile('(.)\s*([A-Z][a-z]+)')
ALL_CAP_RE = re.compile('([a-z0-9])([A-Z])')
def convert(name):
    sub = FIRST_CAP_RE.sub(r'\1_\2', name)
    return ALL_CAP_RE.sub(r'\1_\2', sub).upper()

class CPPWriter(object):
    
    def __init__(self):
        self.headerResult = ""
        self.sourceResult = ""
        self.keyPath = []

    def write_cpp(self, value):
        self.headerResult = "#pragma once\n#include <string>\n#include <vector>\n\nnamespace R {\nnamespace Strings {\n"
        self.sourceResult = "#include \"Strings.h\"\nnamespace R {\nnamespace Strings {\n"
        self.write_dict(value)
        self.sourceResult += "\n} //R\n}//Strings"
        self.headerResult += "\n} //R\n}//Strings"

    def write_value(self, key, value):
        if isinstance(value, dict):
            self.start_namespace(key)
            self.write_dict(value)
            self.end_namespace(key)
        else:
            self.write_key(key)

    def write_dict(self, dic):
        for key in dic.keys():
            value = dic[key]
            self.write_value(key, value)
            
    def start_namespace(self, name):
        self.keyPath.append(name)
        self.headerResult += "namespace " + name + " {\n"
        self.sourceResult += "namespace " + name + " {\n"
        self.write_key("VAL")

    def end_namespace(self, name):
        del self.keyPath[-1]
        self.headerResult += "} //" + name + "\n"
        self.sourceResult += "} //" + name + "\n"

    def write_key(self, name):
        key = convert(name)
        if len(self.keyPath) > 0:
            name = "/".join(self.keyPath) + "/" + name
        self.headerResult += "extern const std::string " + key + ";\n"
        self.sourceResult += "const std::string " + key + " = \"" + name + "\";\n"

def write_result():
    currentPath = os.path.dirname(os.path.realpath(__file__))
    localizationFilePath = os.path.join(currentPath, "..", "Resources", "Localization", "en", "strings.plist")
    if os.path.exists(localizationFilePath):
        plist = plistlib.readPlist(localizationFilePath)
    
        writer = CPPWriter()
        writer.write_cpp(plist)

        headerFile = open(os.path.join(currentPath, "..", "Classes", "Resources", "Strings.h"), 'w+')
        oldHeaderdata = headerFile.read()
        if oldHeaderdata != writer.headerResult:
            headerFile.write(writer.headerResult)
        headerFile.close()

        sourceFile = open(os.path.join(currentPath, "..", "Classes", "Resources", "Strings.cpp"), 'w+')
        oldSourceData = sourceFile.read()
        if oldSourceData != writer.sourceResult:
            sourceFile.write(writer.sourceResult)
        sourceFile.close()

