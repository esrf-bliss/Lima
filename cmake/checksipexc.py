############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2011
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
import os
import sys
import logging
import tempfile
import shutil

logger = logging.getLogger(__file__)

raise_linenr = None
raise_exc = []

def checksipexc(ifname, inplace=False):
    global raise_linenr, raise_exc

    lfname = ifname
    sip_fname = os.path.normpath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '../common/sip/Exceptions.sip'))
    sip_fname = sip_fname.replace('\\', '/')
    if lfname.startswith('/'):
        sip_fname = os.path.realpath(sip_fname)
    else:
        lfname = os.path.join(os.curdir, lfname)
    
    if not raise_exc:
        bfile = open(sip_fname)
        in_raise_code = False
        for i, l in enumerate(bfile.readlines()):
            l = l.strip()
            if l == '%RaiseCode':
                in_raise_code = True
                raise_linenr = i + 2
            elif l == '%End':
                in_raise_code = False
            elif in_raise_code:
                raise_exc.append(l)
        bfile.close()

    ifile = open(ifname, "rt")

    if inplace:
        ofile = tempfile.NamedTemporaryFile(delete=False)
        ofname = ofile.name
    else:
        ofname = ifname + '.out'
        ofile = open(ofname, "wt")

    Out, InTry, InExcHandler, InDefHandler = (
        'Out', 'InTry', 'InExcHandler', 'InDefHandler')

    state = Out
    block = 0
    linenr = 0
    olinenr = 1
    modified = False

    ellipsis = '...'
    exc_def = 'Exception &sipExceptionRef'
    raise_unknown = 'sipRaiseUnknownException();'

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
            ofile.write(line.encode())
            olinenr += line.count('\n')
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
                        sip_head = '#line %d "%s"\n' % (raise_linenr, sip_fname)
                        new_olinenr = olinenr + len(raise_exc) + 2
                        sip_tail = '#line %d "%s"\n\n' % (new_olinenr, lfname)
                        handler_line = sip_head + exc_line.join(tok) + sip_tail
                    ofile.write(handler_line.encode())
                    olinenr += handler_line.count('\n')
                for handler_line in def_handler_code:
                    ofile.write(handler_line.encode())
                    olinenr += handler_line.count('\n')
                modified = True

        if new_state != state:
            logger.debug("Line %d: %s -> %s", linenr, state, new_state)
            state = new_state

    ifile.close()
    ofile.close()
    if modified:
        if inplace:
            shutil.move(ofname, ifname)
        print(("File %s was modified" % ifname))
    return modified


if __name__ == '__main__':
    for src in sys.argv[1:]:
        if 'camera' in src:
            checksipexc(src, inplace=True)
        else:
            continue
    sys.exit(0)
