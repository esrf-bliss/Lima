#!/usr/bin/python

import os
from distutils.sysconfig import get_python_lib

python_path = get_python_lib()

f = open("python_path.tmp", "w")
f.write(str(python_path))
f.close()
