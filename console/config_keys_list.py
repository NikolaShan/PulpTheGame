#!/usr/bin/python

import os
import re
import json


FIRST_CAP_RE = re.compile('(.)\s*([A-Z][a-z]+)')
ALL_CAP_RE = re.compile('([a-z0-9])([A-Z])')
def convertUppercase(name):
    sub = FIRST_CAP_RE.sub(r'\1_\2', name)
    return ALL_CAP_RE.sub(r'\1_\2', sub).upper()

SPLIT_RE = re.compile("[^A-Za-z]+")
def convertCamelcase(chars):
  words = SPLIT_RE.split(chars)
  return "".join( w.title() for i, w in enumerate(words))


class CPPWriter(object):

    def __init__(self):
        self.headerResult = ""
        self.sourceResult = ""
        self.keyPath = []

    def write_cpp(self, value):
        self.headerResult = "#pragma once\n#include <string>\n#include <vector>\n\nnamespace R {\nnamespace Config {\n"
        self.sourceResult = "#include \"ConfigKeys.h\"\nnamespace R {\nnamespace Config {\n"
        self.write_dict(value)
        self.sourceResult += "\n} //R\n}//Config"
        self.headerResult += "\n} //R\n}//Config"

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
        camelCaseName = convertCamelcase(name)
        self.headerResult += "namespace " + camelCaseName + " {\n"
        self.sourceResult += "namespace " + camelCaseName + " {\n"
        self.headerResult += "extern const std::string PATH;\n"
        self.sourceResult += "const std::string PATH = \"" + "_".join(self.keyPath) + "\";\n"

    def end_namespace(self, name):
        del self.keyPath[-1]
        self.headerResult += "} //" + name + "\n"
        self.sourceResult += "} //" + name + "\n"

    def write_key(self, name):
        key = convertUppercase(name)
        if len(self.keyPath) > 0:
            name = "_".join(self.keyPath) + "_" + name
        self.headerResult += "extern const std::string " + key + ";\n"
        self.sourceResult += "const std::string " + key + " = \"" + name + "\";\n"

def write_result():
    currentPath = os.path.dirname(os.path.realpath(__file__))
    configFilePath = os.path.join(currentPath, "..", "Resources", "AppSettings_ios.json")

    with open(configFilePath) as data_file:
        data = json.load(data_file)

        writer = CPPWriter()
        writer.write_cpp(data)

        headerFile = open(os.path.join(currentPath, "..", "Classes", "Resources", "ConfigKeys.h"), 'w+')
        oldHeaderdata = headerFile.read()
        if oldHeaderdata != writer.headerResult:
            headerFile.write(writer.headerResult)
        headerFile.close()

        sourceFile = open(os.path.join(currentPath, "..", "Classes", "Resources", "ConfigKeys.cpp"), 'w+')
        oldSourceData = sourceFile.read()
        if oldSourceData != writer.sourceResult:
            sourceFile.write(writer.sourceResult)
        sourceFile.close()

