import os
import platform

system = platform.system()
if system == 'Linux':
    env = Environment(CCFLAGS = '-fPIC')
    f = file('/proc/cpuinfo')
    b = f.read()
    l =[int(x.split(':')[-1]) for x in b.split('\n') if x.find('processor') > -1]
    l.sort(reverse=True)
    env.SetOption('num_jobs',l[0] + 2)
else:
    env = Environment()
Export('env')

env.Decider('MD5-timestamp')

SConscript([os.path.join('third-party','SConstruct'),
            os.path.join('build','SConstruct')])
