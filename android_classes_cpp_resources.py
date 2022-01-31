#!/usr/bin/python

import os, sys

def find_files(directory):
    for root, dirs, files in os.walk(directory):
        for basename in files:
            if basename.endswith(('.c', '.cc', '.cpp')):
                filename = os.path.join(root, basename)
                yield filename

prefix = "../../"
for filename in find_files('Classes'):
    print os.path.join(prefix, filename) + " \\"
