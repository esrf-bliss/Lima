#
#   Copyright (c) 2007, Simon Edwards <simon@simonzone.com>
#    All rights reserved.
#
#    Redistribution and use in source and binary forms, with or without
#    modification, are permitted provided that the following conditions are met:
#        * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#        * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#        * Neither the name of the  Simon Edwards <simon@simonzone.com> nor the
#        names of its contributors may be used to endorse or promote products
#        derived from this software without specific prior written permission.
#
#    THIS SOFTWARE IS PROVIDED BY Simon Edwards <simon@simonzone.com> ''AS IS'' AND ANY
#    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#    DISCLAIMED. IN NO EVENT SHALL Simon Edwards <simon@simonzone.com> BE LIABLE FOR ANY
#    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# FindSIP.py
# Copyright (c) 2007, Simon Edwards <simon@simonzone.com>
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

import os
import sys
import sysconfig

import sipbuild

print("sip_version:%06.0x" % sipbuild.version.SIP_VERSION)
print("sip_version_num:%d" % sipbuild.version.SIP_VERSION)
print("sip_version_str:%s" % sipbuild.version.SIP_VERSION_STR)

if "deb_system" in sysconfig.get_scheme_names():
    python_modules_dir = sysconfig.get_path("purelib", "deb_system")
else:
    python_modules_dir = sysconfig.get_path("purelib")
print("default_sip_dir:%s" % python_modules_dir)


def abi_version_from_installed_sip():
    """ABI version sip itself ships support for, used as a fallback when
    lima.sip doesn't expose SIP_ABI_VERSION directly."""
    for major in range(20, 0, -1):
        source_path = sysconfig.get_path("purelib") + f"/sipbuild/module/source/{major}"
        if os.path.isdir(source_path):
            minors = [int(name) for name in os.listdir(source_path) if name.isdigit()]
            minor = max(minors) if minors else 0
            return (major << 16) | (minor << 8)
    return None


try:
    import lima.sip
except ModuleNotFoundError:
    # lima.sip is only published as the lima-sip6 conda package - it has no
    # source of its own, it's a generic sip-generated stub (same idea as
    # PyQt5.sip) built on the fly from whatever `sip` is installed, purely to
    # pin an ABI version and provide sip.h. When it isn't installed (e.g.
    # building from source outside conda), generate that same stub ourselves
    # instead of requiring the package.
    import subprocess
    import tempfile

    gen_dir = os.path.join(tempfile.gettempdir(), "lima-sip-module")
    os.makedirs(gen_dir, exist_ok=True)
    subprocess.run(
        [sys.executable, "-m", "sipbuild.tools.module",
         "--sdist", "--sip-h", "--target-dir", gen_dir, "lima.sip"],
        check=True,
    )
    print(f"sip_inc_dir:{gen_dir}")

    lima_sip_abi_version_num = abi_version_from_installed_sip()
    if lima_sip_abi_version_num is None:
        lima_sip_abi_version_num = sipbuild.version.SIP_VERSION
else:
    # lima-sip6's conda recipe drops sip.h next to the usual include dir of
    # the environment it's installed into.
    print(f"sip_inc_dir:{os.path.join(sys.prefix, 'include')}")

    if hasattr(lima.sip, "SIP_ABI_VERSION"):
        lima_sip_abi_version_num = lima.sip.SIP_ABI_VERSION
    else:
        lima_sip_abi_version_num = abi_version_from_installed_sip()
        if lima_sip_abi_version_num is None:
            lima_sip_abi_version_num = lima.sip.SIP_VERSION

lima_sip_abi_version = f"{lima_sip_abi_version_num >> 16}.{(lima_sip_abi_version_num & 0x00ff00) >> 8}"
print(f"lima_sip_abi_version:{lima_sip_abi_version}")
