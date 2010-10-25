import os

root_name = __path__[0]

csadmin_dirs = ['/csadmin/local', '/csadmin/commmon']
script_get_os = 'scripts/get_compat_os.share'
get_os = None
for d in csadmin_dirs:
	aux_get_os = os.path.join(d, script_get_os)
	if os.path.exists(aux_get_os):
		get_os = aux_get_os
		break
if get_os is not None:
        compat_plat = os.popen(get_os).readline().strip()
        for aux_plat in compat_plat.split():
        	if aux_plat.strip() in os.listdir(root_name):
        		plat = aux_plat
        		break

        lima_plat = os.path.join(root_name, plat)
        __path__.insert(0, lima_plat)

import Core

if get_os is not None:
        all_dirs = os.listdir(lima_plat)
        all_dirs.remove('Lib')

        __all__ = all_dirs
        del plat, compat_plat, aux_plat, lima_plat, all_dirs

del root_name, csadmin_dirs, get_os, script_get_os, d, aux_get_os
del os
