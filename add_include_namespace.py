import re
import os
import glob
import sys
import platform
import imp

lima_includes = set()
lima_dirs = ['common',
             'control',
             'control/software_operation',
             'hardware',
             ]

base_script_path,_ = os.path.split(os.path.realpath(__file__))

#load processlib namespace
processlib_rel_path = 'third-party/Processlib/add_include_namespace.py'
processlib = imp.load_source('processlib',
                             os.path.join(base_script_path,processlib_rel_path))
processlib.init()

for lima_include_path in lima_dirs:
    includes = glob.glob(os.path.join(base_script_path,lima_include_path,
                                      'include','lima','*.h'))
    lima_includes.update([os.path.split(x)[1] for x in includes])


if platform.system() == 'Windows':
    file_list = []
    for arg in sys.argv[1:]:
        file_list.extend(glob.glob(arg))
else:
    file_list = sys.argv[1:]

processlib.sed(file_list)

match_include = re.compile('^\s*#include\s+["<](.+?)[">]\s*$')
for file in file_list:
    with open(file,'r+') as f:
        w_buffer = ''
        modify = False
        for line in f:
            match_group = match_include.match(line)
            if match_group and match_group.group(1) in lima_includes:
                w_buffer += '#include "lima/%s"\n' % match_group.group(1)
                modify = True
            else:
                w_buffer += line
        if modify:
            f.seek(0,0)
            f.write(w_buffer)
