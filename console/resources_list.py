#!/usr/bin/python

import re, os, sys
from stat import *

def removeSpaces(name):
	return name.replace(" ", "")

first_cap_re = re.compile('(.)\s*([A-Z][a-z]+)')
all_cap_re = re.compile('([a-z0-9])([A-Z])')
double_under_re = re.compile('[_-]+')
linux_slashes_re = re.compile('\\\\')
def convert(name):
    s1 = first_cap_re.sub(r'\1_\2', name)
    s2 = all_cap_re.sub(r'\1_\2', s1).upper()
    return double_under_re.sub(r'_',s2)

def convert_linux_path(path):
	return linux_slashes_re.sub(r'/',path)

def get_directory_structure(rootdir):
    dir = {}
    rootdir = rootdir.rstrip(os.sep)
    start = rootdir.rfind(os.sep) + 1
    for path, dirs, files in os.walk(rootdir):
    	files = [f for f in files if not f[0] == '.']
    	dirs[:] = [d for d in dirs if not d[0] == '.']

        folders = path[start:].split(os.sep)
    	parentFolders = folders[:-1]
    	currentFolderName = folders[-1]
    	if (currentFolderName.startswith("resources-")):
    		currentFolderName = folders[-2]
    		parentFolders = folders[:-2]
        filesInFolder = dict.fromkeys(files)
        parent = reduce(dict.get, parentFolders, dir)
        if currentFolderName in parent:
        	parent[currentFolderName].update(filesInFolder)
        else:
        	parent[currentFolderName] = filesInFolder
    return dir

image_extensions = ['.png', '.jpg', '.jpeg', 'pvr', 'pvr.ccz']
def is_image_extension(ext):
	return ext.lower() in image_extensions

def get_file_suffix_for_extension(ext):
	if is_image_extension(ext):
		return "_IMG"
	elif ext.lower() == ".vert":
		return "_VERT"
	elif ext.lower() == ".frag":
		return "_FRAG"
	elif ext.lower() == ".plist":
		return "_PLIST"
	else:
		return ""

class CppWriter:
	def __init__(self):
		self.headerResult = "";
		self.sourceResult = "";	

	def write(self, src):
		self.headerResult = "//autogenerated file, don't try to change something manually"
		self.headerResult += "\n#pragma once"
		self.headerResult += "\n#include <string>"
		self.headerResult += "\n#include \"Strings.h\"\n"
		self.sourceResult = "#include \"Resources.h\""
		self.write_resources_list(src, "")

	def write_value(self, key, value, path):
		if isinstance(value, dict):
			path = '' if (key == 'R') else os.path.join(path, key)
			self.start_namespace(key)
			self.write_folder_name(key)			
			self.write_folder_path(path)
			self.write_resources_list(value, path)
			self.end_namespace(key)
		else:
			self.write_resource(key, path)

	def write_resources_list(self, dictionary, path):
		for key in dictionary.keys():
			self.write_value(key, dictionary[key], path)

	def start_namespace(self, name):
		name = removeSpaces(name)
		self.headerResult += "\nnamespace " + name + " {"
		self.sourceResult += "\nnamespace " + name + " {"

	def end_namespace(self, name):
		name = removeSpaces(name)
		self.headerResult += "\n} //" + name
		self.sourceResult += "\n} //" + name

	def write_resource(self, name, folder):
		if (name.startswith(".")):
			return
		fileName, ext = os.path.splitext(name)
		varName = convert(fileName) + get_file_suffix_for_extension(ext)
		fullPath = os.path.join(folder, name)
		self.headerResult += "\nextern const std::string " + varName + ";"
		self.sourceResult += "\nconst std::string " + varName + " = \"" + convert_linux_path(fullPath) + "\";"

	def write_folder_path(self, folder):
		folderName = os.path.basename(os.path.normpath(folder))
		if (folderName != "."):
			varName = "DIR_PATH";
			self.headerResult += "\nextern const std::string " + varName + ";"
			self.sourceResult += "\nconst std::string " + varName + " = \"" + convert_linux_path(folder) + "\";"		

	def write_folder_name(self, folder):
		folderName = os.path.basename(os.path.normpath(folder))
		if (folderName != "."):
			varName = "DIR_NAME";
			self.headerResult += "\nextern const std::string " + varName + ";"
			self.sourceResult += "\nconst std::string " + varName + " = \"" + convert_linux_path(folder) + "\";"			

	def write_levels_count(self, count):
		self.headerResult += "\nnamespace R {\n\textern const int LEVELS_COUNT;\n}"
		self.sourceResult += "\nnamespace R {\n\tconst int LEVELS_COUNT = " + str(count) + ";\n}"


def write_result():
	currentPath = os.path.dirname(os.path.realpath(__file__))
	files = get_directory_structure(os.path.join(currentPath, "..", "Resources"))
	levelsCount = len(files['Resources']['Levels'])
	files['Resources'].pop('Levels')
	files['R'] = files.pop('Resources')

	rootFiles = files['R']
	if 'CCB' in rootFiles:
		for key, value in rootFiles['CCB'].iteritems():
			rootFiles[key] = value
		rootFiles.pop('CCB')
	
#	del files['R']['Content']

	writer = CppWriter()
	writer.write(files)
	writer.write_levels_count(levelsCount)

	resourcesDir = os.path.join(currentPath, "..", "Classes", "Resources")
	if not os.path.exists(resourcesDir):
		os.makedirs(resourcesDir)

	headerFile = open(os.path.join(resourcesDir, "Resources.h"), 'w+')

	oldHeaderdata = headerFile.read()
	if (oldHeaderdata != writer.headerResult ):
		headerFile.write(writer.headerResult)
	headerFile.close()	

	sourceFile = open(os.path.join(currentPath, "..", "Classes", "Resources", "Resources.cpp"), 'w+')
	oldSourceData = sourceFile.read()
	if (oldSourceData != writer.sourceResult):
		sourceFile.write(writer.sourceResult)
	sourceFile.close()
