import os, sys

ifname = sys.argv[1]
ifile = open(ifname, "rt")

ofname = ifname + '.out'
ofile = open(ofname, "wt")

Out, InTry, InExcHandler, InDefHandler = \
     'Out', 'InTry', 'InExcHandler', 'InDefHandler'

state = Out
block = 0
linenr = 0
modified = False

ellipsis = '...'
exc_def = 'Exception &sipExceptionRef'
raise_unknown = 'sipRaiseUnknownException();'
raise_exc = ['const char *detail = sipExceptionRef.getErrMsg().c_str();',
             'PyErr_SetString(sipException_Exception, detail);']

for line in ifile.readlines():
    linenr += 1
 
    new_state = state
    
    l = line.strip()
    if l == 'try':
        new_state = InTry
        had_exc_handler = False
    elif 'catch (Exception' in l:
        new_state = InExcHandler
        had_exc_handler = True
    elif l == 'catch (...)':
        new_state = InDefHandler
        def_handler_code = []
    elif state != Out:
        if l == '{':
            block += 1
        elif l == '}':
            block -= 1
            if not block and state == InDefHandler:
                new_state = Out

    if InDefHandler not in [state, new_state] or had_exc_handler:
        ofile.write(line)
    else:
        def_handler_code.append(line)
        if new_state == Out:
            for handler_line in def_handler_code:
                if ellipsis in handler_line:
                    tok = handler_line.split(ellipsis)
                    handler_line = exc_def.join(tok)
                if raise_unknown in handler_line:
                    tok = handler_line.split(raise_unknown)
                    sep = '\n' + tok[0]
                    exc_line = sep.join(raise_exc)
                    handler_line = exc_line.join(tok)
                ofile.write(handler_line)
            for handler_line in def_handler_code:
                ofile.write(handler_line)
            modified = True

    if new_state != state:
        sys.stderr.write("Line %d: %s -> %s\n" % (linenr, state, new_state))
        state = new_state


ifile.close()
ofile.close()

if modified:
    print "File %s was modified" % ifname
    sys.exit(1)

