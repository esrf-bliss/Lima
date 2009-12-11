from lima import DebParams, DebObj
import os, sys, new


def DEB_GLOBAL_FUNCT(fn):
    return DEB_FUNCT(fn, True, 2)

def DEB_MEMBER_FUNCT(fn):
    return DEB_FUNCT(fn, False, 2)

def DEB_FUNCT(fn, in_global=True, frame=1):
    frame = sys._getframe(frame)
    if in_global:
        n_dict = frame.f_globals
    else:
        n_dict = frame.f_locals
    deb_params = n_dict['deb_params']
    code = frame.f_code
    filename =  os.path.basename(code.co_filename)
    lineno = frame.f_lineno
    def real_fn(*arg, **kw):
        sys.exc_clear()
        fn_globals = dict(fn.func_globals)
        fn_globals['deb'] = DebObj(deb_params, fn.func_name, '',
                                   filename, lineno)
        new_fn = new.function(fn.func_code, fn_globals, fn.func_name,
                              fn.func_defaults)
        return new_fn(*arg, **kw)
    return real_fn
        
def DEB_GLOBAL(deb_mod):
    DEB_PARAMS(deb_mod, '', True, 2)

def DEB_CLASS(deb_mod, class_name):
    DEB_PARAMS(deb_mod, class_name, False, 2)
    
def DEB_PARAMS(deb_mod, class_name, in_global=True, frame=1):
    frame = sys._getframe(frame)
    g_dict, l_dict = frame.f_globals, frame.f_locals
    mod_name = g_dict['__name__']
    if mod_name == '__main__':
        file_name = frame.f_code.co_filename
        mod_name = os.path.basename(file_name).strip('.py')
    if in_global:
        d_dict = g_dict
    else:
        d_dict = l_dict
    d_dict['deb_params'] = DebParams(deb_mod, class_name, mod_name)
