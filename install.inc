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

ifndef INSTALL_DIR
export INSTALL_DIR = $(shell pwd)/install
endif

ifndef INSTALL_LIMA_DIR
INSTALL_LIMA_DIR = $(INSTALL_DIR)/Lima
endif

ifndef INSTALL_PLAT
GET_OS_SHARE = $(shell ls /csadmin/common/scripts/get_os.share 2> /dev/null)
ifeq ($(GET_OS_SHARE),)
INSTALL_PLAT=
else
INSTALL_PLAT=$(shell $(GET_OS_SHARE))
endif
endif

ifndef INSTALL_PLAT_DIR
export INSTALL_PLAT_DIR = $(INSTALL_LIMA_DIR)/$(INSTALL_PLAT)
endif

ifndef INSTALL_LIB_DIR
export INSTALL_LIB_DIR = $(INSTALL_PLAT_DIR)/lib
endif

INSTALL_INCLUDE_DIR = $(INSTALL_PLAT_DIR)/include
INSTALL_PKG_CONFIG_DIR = $(INSTALL_LIB_DIR)/pkgconfig
INSTALL_CONFIG_DEFINES =
INSTALL_LIBADD =
INSTALL_DEPS = install.mkdir

ifeq ($(LINK_STRICT_VERSION),0)
ENV_LINK_STRICT_VER = 'MINOR'
else
ENV_LINK_STRICT_VER = 'FULL'
endif

GLOB_PYTHON_ENV = \(os.environ\['LIMA_LINK_STRICT_VERSION'\][ \t]*=[ \t]*\)
GLOB_PYTHON_INIT_SED = "s/$(GLOB_PYTHON_ENV)'[A-Z]\+'/\1$(ENV_LINK_STRICT_VER)/"

CORE_VERSION_NAME = common/VERSION
install_core_dep = cp $(CORE_VERSION_NAME) $(1)/CORE_VERSION

ifneq ($(COMPILE_CORE), 0)
INSTALL_CORE_DIR = $(INSTALL_PLAT_DIR)/Core
CORE_FULL_VERSION = $(shell cat $(CORE_VERSION_NAME))
INSTALL_CORE_VDIR = $(INSTALL_CORE_DIR)/v$(CORE_FULL_VERSION)
endif

ifneq ($(COMPILE_SIMULATOR), 0)
INSTALL_SIMU_DIR = $(INSTALL_PLAT_DIR)/Simulator
SIMU_FULL_VERSION = $(shell cat camera/simulator/VERSION)
INSTALL_SIMU_VDIR = $(INSTALL_SIMU_DIR)/v$(SIMU_FULL_VERSION)
endif

ESPIA_VERSION_NAME = camera/common/espia/VERSION
install_espia_dep = cp $(ESPIA_VERSION_NAME) $(1)/ESPIA_VERSION

ifneq ($(COMPILE_ESPIA), 0)
INSTALL_ESPIA_DIR = $(INSTALL_PLAT_DIR)/Espia
ESPIA_FULL_VERSION = $(shell cat $(ESPIA_VERSION_NAME))
INSTALL_ESPIA_VDIR = $(INSTALL_ESPIA_DIR)/v$(ESPIA_FULL_VERSION)
endif

ifneq ($(COMPILE_FRELON), 0)
INSTALL_FRELON_DIR = $(INSTALL_PLAT_DIR)/Frelon
FRELON_FULL_VERSION = $(shell cat camera/frelon/VERSION)
INSTALL_FRELON_VDIR = $(INSTALL_FRELON_DIR)/v$(FRELON_FULL_VERSION)
endif

ifneq ($(COMPILE_MAXIPIX), 0)
INSTALL_MAXIPIX_DIR = $(INSTALL_PLAT_DIR)/Maxipix
MAXIPIX_FULL_VERSION = $(shell cat camera/maxipix/VERSION)
INSTALL_MAXIPIX_VDIR = $(INSTALL_MAXIPIX_DIR)/v$(MAXIPIX_FULL_VERSION)
endif

ifneq ($(COMPILE_PILATUS), 0)
INSTALL_PILATUS_DIR = $(INSTALL_PLAT_DIR)/Pilatus
PILATUS_FULL_VERSION = $(shell cat camera/pilatus/VERSION)
INSTALL_PILATUS_VDIR = $(INSTALL_PILATUS_DIR)/v$(PILATUS_FULL_VERSION)
endif

ifneq ($(COMPILE_BASLER), 0)
INSTALL_BASLER_DIR = $(INSTALL_PLAT_DIR)/Basler
BASLER_FULL_VERSION = $(shell cat camera/basler/VERSION)
INSTALL_BASLER_VDIR = $(INSTALL_BASLER_DIR)/v$(BASLER_FULL_VERSION)
endif

ifneq ($(COMPILE_PROSILICA), 0)
INSTALL_PROSILICA_DIR = $(INSTALL_PLAT_DIR)/Prosilica
PROSILICA_FULL_VERSION = $(shell cat camera/prosilica/VERSION)
INSTALL_PROSILICA_VDIR = $(INSTALL_PROSILICA_DIR)/v$(PROSILICA_FULL_VERSION)
endif

ifneq ($(COMPILE_ROPERSCIENTIFIC), 0)
INSTALL_ROPERSCIENTIFIC_DIR = $(INSTALL_PLAT_DIR)/RoperScientific
ROPERSCIENTIFIC_FULL_VERSION = $(shell cat camera/roperscientific/VERSION)
INSTALL_ROPERSCIENTIFIC_VDIR = $(INSTALL_ROPERSCIENTIFIC_DIR)/v$(ROPERSCIENTIFIC_FULL_VERSION)
endif

ifneq ($(COMPILE_ADSC), 0)
INSTALL_ADSC_DIR = $(INSTALL_PLAT_DIR)/Adsc
ADSC_FULL_VERSION = $(shell cat camera/adsc/VERSION)
INSTALL_ADSC_VDIR = $(INSTALL_ADSC_DIR)/v$(ADSC_FULL_VERSION)
endif


ifneq ($(COMPILE_MYTHEN), 0)
INSTALL_MYTHEN_DIR = $(INSTALL_PLAT_DIR)/Mythen
MYTHEN_FULL_VERSION = $(shell cat camera/mythen/VERSION)
INSTALL_MYTHEN_VDIR = $(INSTALL_MYTHEN_DIR)/v$(MYTHEN_FULL_VERSION)
endif

ifneq ($(COMPILE_UEYE), 0)
INSTALL_UEYE_DIR = $(INSTALL_PLAT_DIR)/Ueye
UEYE_FULL_VERSION = $(shell cat camera/ueye/VERSION)
INSTALL_UEYE_VDIR = $(INSTALL_UEYE_DIR)/v$(UEYE_FULL_VERSION)
endif

ifneq ($(COMPILE_ANDOR), 0)
INSTALL_ANDOR_DIR = $(INSTALL_PLAT_DIR)/Andor
ANDOR_FULL_VERSION = $(shell cat camera/andor/VERSION)
INSTALL_ANDOR_VDIR = $(INSTALL_ANDOR_DIR)/v$(ANDOR_FULL_VERSION)
endif

ifneq ($(COMPILE_XH), 0)
INSTALL_XH_DIR = $(INSTALL_PLAT_DIR)/Xh
XH_FULL_VERSION = $(shell cat camera/xh/VERSION)
INSTALL_XH_VDIR = $(INSTALL_XH_DIR)/v$(XH_FULL_VERSION)
endif

ifneq ($(COMPILE_XSPRESS3), 0)
INSTALL_XSPRESS3_DIR = $(INSTALL_PLAT_DIR)/Xspress3
XSPRESS3_FULL_VERSION = $(shell cat camera/xspress3/VERSION)
INSTALL_XSPRESS3_VDIR = $(INSTALL_XSPRESS3_DIR)/v$(XSPRESS3_FULL_VERSION)
endif

ifneq ($(COMPILE_ULTRA), 0)
INSTALL_ULTRA_DIR = $(INSTALL_PLAT_DIR)/Ultra
ULTRA_FULL_VERSION = $(shell cat camera/ultra/VERSION)
INSTALL_ULTRA_VDIR = $(INSTALL_ULTRA_DIR)/v$(ULTRA_FULL_VERSION)
endif

ifneq ($(COMPILE_XPAD), 0)
INSTALL_XPAD_DIR = $(INSTALL_PLAT_DIR)/Xpad
XPAD_FULL_VERSION = $(shell cat camera/xpad/VERSION)
INSTALL_XPAD_VDIR = $(INSTALL_XPAD_DIR)/v$(XPAD_FULL_VERSION)
endif

ifneq ($(COMPILE_MARCCD), 0)
INSTALL_MARCCD_DIR = $(INSTALL_PLAT_DIR)/Marccd
MARCCD_FULL_VERSION = $(shell cat camera/marccd/VERSION)
INSTALL_MARCCD_VDIR = $(INSTALL_MARCCD_DIR)/v$(MARCCD_FULL_VERSION)
endif

ifneq ($(COMPILE_POINTGREY), 0)
INSTALL_POINTGREY_DIR = $(INSTALL_PLAT_DIR)/PointGrey
POINTGREY_FULL_VERSION = $(shell cat camera/pointgrey/VERSION)
INSTALL_POINTGREY_VDIR = $(INSTALL_POINTGREY_DIR)/v$(POINTGREY_FULL_VERSION)
endif

ifneq ($(COMPILE_IMXPAD), 0)
INSTALL_IMXPAD_DIR = $(INSTALL_PLAT_DIR)/imXpad
IMXPAD_FULL_VERSION = $(shell cat camera/imxpad/VERSION)
INSTALL_IMXPAD_VDIR = $(INSTALL_IMXPAD_DIR)/v$(IMXPAD_FULL_VERSION)
endif

ifneq ($(COMPILE_RAYONIXHS), 0)
INSTALL_RAYONIXHS_DIR = $(INSTALL_PLAT_DIR)/RayonixHs
RAYONIXHS_FULL_VERSION = $(shell cat camera/rayonixhs/VERSION)
INSTALL_RAYONIXHS_VDIR = $(INSTALL_RAYONIXHS_DIR)/v$(RAYONIXHS_FULL_VERSION)
endif

ifneq ($(COMPILE_SPS_IMAGE),0)
INSTALL_CONFIG_DEFINES += -DWITH_SPS_IMAGE
endif

ifneq ($(COMPILE_CBF_SAVING),0)
INSTALL_CONFIG_DEFINES += -DWITH_CBF_SAVING
endif

ifneq ($(COMPILE_NXS_SAVING),0)
INSTALL_CONFIG_DEFINES += -DWITH_NXS_SAVING
endif

ifneq ($(COMPILE_FITS_SAVING),0)
INSTALL_CONFIG_DEFINES += -DWITH_FITS_SAVING
#INSTALL_LIBADD += -lCCfits
INSTALL_DEPS += cfitsio.install CCfits.install
endif

ifneq ($(COMPILE_TIFF_SAVING),0)
INSTALL_CONFIG_DEFINES += -DWITH_TIFF_SAVING
endif

ifneq ($(COMPILE_CONFIG),0)
INSTALL_CONFIG_DEFINES += -DWITH_CONFIG
INSTALL_DEPS += libconfig.install
endif
##################
#  INSTALL DEPS  #
##################

install.mkdir:
	mkdir -p $(INSTALL_LIMA_DIR)
	sed $(GLOB_PYTHON_INIT_SED) python/__init__.py > \
                                    $(INSTALL_LIMA_DIR)/__init__.py
	mkdir -p $(INSTALL_PLAT_DIR)
	cp python/module_helper.py $(INSTALL_PLAT_DIR)
	mkdir -p $(INSTALL_LIB_DIR)
	mkdir -p $(INSTALL_INCLUDE_DIR)
	mkdir -p $(INSTALL_PKG_CONFIG_DIR)

cfitsio.install:
	$(MAKE) -C third-party cfitsio.install

CCfits.install:
	$(MAKE) -C third-party CCfits.install

libconfig.install:
	$(MAKE) -C third-party libconfig.install

##################
# INSTALL TARGET #
##################

.PHONY: install

PROCESSOR_TYPE := $(shell uname -i)

install: $(INSTALL_DEPS)
	sed -e "s,@prefix@,${INSTALL_DIR},g" \
		-e "s,@libdir@,${INSTALL_LIB_DIR},g" \
		-e "s,@libadd@,${INSTALL_LIBADD},g" \
		-e "s,@includedir@,${INSTALL_INCLUDE_DIR},g" \
		-e "s,@config_defines@,${INSTALL_CONFIG_DEFINES},g" \
		-e "s,@VERSION@,`cat $(CORE_VERSION_NAME)`,g" \
		Lima.pc.in > $(INSTALL_PKG_CONFIG_DIR)/Lima.pc

ifneq ($(COMPILE_CORE), 0)
	mkdir -p $(INSTALL_CORE_VDIR)
	cp common/python/__init__.py $(INSTALL_CORE_DIR)
	cp -d third-party/Processlib/build/libprocesslib.so* $(INSTALL_LIB_DIR)
	cp third-party/Processlib/sip/processlib.so $(INSTALL_CORE_VDIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimacore.so*
	cp -d build/liblimacore.so* $(INSTALL_LIB_DIR)
	cp sip/core/limacore.so $(INSTALL_CORE_VDIR)
	cp common/python/Debug.py $(INSTALL_CORE_VDIR)
	cp common/include/*.h $(INSTALL_INCLUDE_DIR)
	cp control/include/*.h $(INSTALL_INCLUDE_DIR)
	cp control/software_operation/include/*.h $(INSTALL_INCLUDE_DIR)
	cp hardware/include/*.h $(INSTALL_INCLUDE_DIR)
	cp third-party/Processlib/core/include/*.h $(INSTALL_INCLUDE_DIR)
	cp third-party/Processlib/core/include/*.i $(INSTALL_INCLUDE_DIR)
endif
ifneq ($(COMPILE_SIMULATOR), 0)
	mkdir -p $(INSTALL_SIMU_VDIR)
	cp camera/simulator/python/__init__.py $(INSTALL_SIMU_DIR)
	$(call install_core_dep,$(INSTALL_SIMU_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimasimulator.so*
	cp -d build/liblimasimulator.so* $(INSTALL_LIB_DIR)
	cp sip/simulator/limasimulator.so $(INSTALL_SIMU_VDIR)
	cp camera/simulator/include/*.h $(INSTALL_INCLUDE_DIR)
endif
ifneq ($(COMPILE_ESPIA), 0)
	mkdir -p $(INSTALL_ESPIA_VDIR)
	cp camera/common/espia/python/__init__.py $(INSTALL_ESPIA_DIR)
	$(call install_core_dep,$(INSTALL_ESPIA_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimaespia.so*
	cp -d build/liblimaespia.so* $(INSTALL_LIB_DIR)
	cp sip/espia/limaespia.so $(INSTALL_ESPIA_VDIR)
	cp camera/common/espia/include/*.h $(INSTALL_INCLUDE_DIR)
endif
ifneq ($(COMPILE_FRELON), 0)
	mkdir -p $(INSTALL_FRELON_VDIR)
	cp camera/frelon/python/__init__.py $(INSTALL_FRELON_DIR)
	$(call install_espia_dep,$(INSTALL_FRELON_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimafrelon.so*
	cp -d build/liblimafrelon.so* $(INSTALL_LIB_DIR)
	cp sip/frelon/limafrelon.so $(INSTALL_FRELON_VDIR)
	cp camera/frelon/python/FrelonAcq.py $(INSTALL_FRELON_VDIR)
	cp camera/frelon/include/*.h $(INSTALL_INCLUDE_DIR)
endif
ifneq ($(COMPILE_MAXIPIX), 0)
	mkdir -p $(INSTALL_MAXIPIX_VDIR)
	cp camera/maxipix/python/__init__.py $(INSTALL_MAXIPIX_DIR)
	$(call install_espia_dep,$(INSTALL_MAXIPIX_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimamaxipix.so*
	cp -d build/liblimamaxipix.so* $(INSTALL_LIB_DIR)
	cp sip/maxipix/limamaxipix.so $(INSTALL_MAXIPIX_VDIR)
	cp camera/maxipix/python/Mpx*.py $(INSTALL_MAXIPIX_VDIR)
	cp camera/maxipix/include/*.h $(INSTALL_INCLUDE_DIR)
endif
ifneq ($(COMPILE_PILATUS), 0)
	mkdir -p $(INSTALL_PILATUS_VDIR)
	cp camera/pilatus/python/__init__.py $(INSTALL_PILATUS_DIR)
	$(call install_core_dep,$(INSTALL_PILATUS_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimapilatus.so*
	cp -d build/liblimapilatus.so* $(INSTALL_LIB_DIR)
	cp sip/pilatus/limapilatus.so $(INSTALL_PILATUS_VDIR)
	cp camera/pilatus/include/*.h $(INSTALL_INCLUDE_DIR)
endif
ifneq ($(COMPILE_BASLER), 0)
	mkdir -p $(INSTALL_BASLER_VDIR)
	cp camera/basler/python/__init__.py $(INSTALL_BASLER_DIR)
	$(call install_core_dep,$(INSTALL_BASLER_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimabasler.so*
	cp -d build/liblimabasler.so* $(INSTALL_LIB_DIR)
	cp sip/basler/limabasler.so $(INSTALL_BASLER_VDIR)
	cp camera/basler/include/*.h $(INSTALL_INCLUDE_DIR)
endif
ifneq ($(COMPILE_PROSILICA), 0)
	mkdir -p $(INSTALL_PROSILICA_VDIR)
	cp camera/prosilica/python/__init__.py $(INSTALL_PROSILICA_DIR)
	$(call install_core_dep,$(INSTALL_PROSILICA_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimaprosilica.so*
	rm -rf $(INSTALL_LIB_DIR)/libPvAPI.so
	cp -d build/liblimaprosilica.so* $(INSTALL_LIB_DIR)
	cp camera/prosilica/include/*.h $(INSTALL_INCLUDE_DIR)
ifeq ($(PROCESSOR_TYPE),i386)
ifeq ($(INSTALL_PLAT),redhate4) 
	cp camera/prosilica/sdk/bin/x86/old_libPvApi.so $(INSTALL_LIB_DIR)/libPvAPI.so
else
	cp camera/prosilica/sdk/bin/x86/libPvAPI.so $(INSTALL_LIB_DIR)
endif
else
ifeq ($(INSTALL_PLAT),redhate5) 
	cp camera/prosilica/sdk/bin/x64/old_libPvApi.so $(INSTALL_LIB_DIR)/libPvAPI.so
else
	cp camera/prosilica/sdk/bin/x64/libPvAPI.so $(INSTALL_LIB_DIR)
endif
endif
	cp sip/prosilica/limaprosilica.so $(INSTALL_PROSILICA_VDIR)
endif

ifneq ($(COMPILE_ROPERSCIENTIFIC), 0)
	mkdir -p $(INSTALL_ROPERSCIENTIFIC_VDIR)
	cp camera/roperscientific/python/__init__.py $(INSTALL_ROPERSCIENTIFIC_DIR)
	$(call install_core_dep,$(INSTALL_ROPERSCIENTIFIC_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimaroperscientific.so*
	cp -d build/liblimaroperscientific.so* $(INSTALL_LIB_DIR)
	cp sip/roperscientific/limaroperscientific.so $(INSTALL_ROPERSCIENTIFIC_VDIR)
	cp camera/roperscientific/include/*.h $(INSTALL_INCLUDE_DIR)
endif

ifneq ($(COMPILE_ADSC), 0)
	mkdir -p $(INSTALL_ADSC_VDIR)
	cp camera/adsc/python/__init__.py $(INSTALL_ADSC_DIR)
	$(call install_core_dep,$(INSTALL_ADSC_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimaadsc.so*
	cp -d build/liblimaadsc.so* $(INSTALL_LIB_DIR)
	cp sip/adsc/limaadsc.so $(INSTALL_ADSC_VDIR)
	cp camera/adsc/sdk/adsc_sources/lib/linux/libauxlib.so $(INSTALL_LIB_DIR)/libauxlib.so
	cp camera/adsc/sdk/adsc_sources/lib/linux/libdetcon_th.so $(INSTALL_LIB_DIR)/libdetcon_th.so
	cp camera/adsc/include/*.h $(INSTALL_INCLUDE_DIR)
endif

ifneq ($(COMPILE_MYTHEN), 0)
	mkdir -p $(INSTALL_MYTHEN_VDIR)
	cp camera/mythen/python/__init__.py $(INSTALL_MYTHEN_DIR)
	$(call install_core_dep,$(INSTALL_MYTHEN_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimamythen.so*
	cp -d build/liblimamythen.so* $(INSTALL_LIB_DIR)
	cp sip/mythen/limamythen.so $(INSTALL_MYTHEN_VDIR)
	cp camera/mythen/include/*.h $(INSTALL_INCLUDE_DIR)
endif

ifneq ($(COMPILE_UEYE), 0)
	mkdir -p $(INSTALL_UEYE_VDIR)
	cp camera/ueye/python/__init__.py $(INSTALL_UEYE_DIR)
	$(call install_core_dep,$(INSTALL_UEYE_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimaueye.so*
	cp -d build/liblimaueye.so* $(INSTALL_LIB_DIR)
	cp sip/ueye/limaueye.so $(INSTALL_UEYE_VDIR)
	cp camera/ueye/include/*.h $(INSTALL_INCLUDE_DIR)
endif
ifneq ($(COMPILE_ANDOR), 0)
	mkdir -p $(INSTALL_ANDOR_VDIR)
	cp camera/andor/python/__init__.py $(INSTALL_ANDOR_DIR)
	$(call install_core_dep,$(INSTALL_ANDOR_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimaandor.so*
	cp -d build/liblimaandor.so* $(INSTALL_LIB_DIR)
	cp sip/andor/limaandor.so $(INSTALL_ANDOR_VDIR)
	cp camera/andor/include/*.h $(INSTALL_INCLUDE_DIR)
endif

ifneq ($(COMPILE_XH), 0)
	mkdir -p $(INSTALL_XH_VDIR)
	cp camera/xh/python/__init__.py $(INSTALL_XH_DIR)
	$(call install_core_dep,$(INSTALL_XH_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimaxh.so*
	cp -d build/liblimaxh.so* $(INSTALL_LIB_DIR)
	cp sip/xh/limaxh.so $(INSTALL_XH_VDIR)
	cp camera/xh/include/*.h $(INSTALL_INCLUDE_DIR)
endif

ifneq ($(COMPILE_XSPRESS3), 0)
	mkdir -p $(INSTALL_XSPRESS3_VDIR)
	cp camera/xspress3/python/__init__.py $(INSTALL_XSPRESS3_DIR)
	$(call install_core_dep,$(INSTALL_XSPRESS3_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimaxspress3.so*
	cp -d build/liblimaxspress3.so* $(INSTALL_LIB_DIR)
	cp sip/xspress3/limaxspress3.so $(INSTALL_XSPRESS3_VDIR)
	cp camera/xspress3/include/*.h $(INSTALL_INCLUDE_DIR)
endif

ifneq ($(COMPILE_ULTRA), 0)
	mkdir -p $(INSTALL_ULTRA_VDIR)
	cp camera/ultra/python/__init__.py $(INSTALL_ULTRA_DIR)
	$(call install_core_dep,$(INSTALL_ULTRA_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimaultra.so*
	cp -d build/liblimaultra.so* $(INSTALL_LIB_DIR)
	cp sip/ultra/limaultra.so $(INSTALL_ULTRA_VDIR)
	cp camera/ultra/include/*.h $(INSTALL_INCLUDE_DIR)
endif

ifneq ($(COMPILE_XPAD), 0)
	mkdir -p $(INSTALL_XPAD_VDIR)
	cp camera/xpad/python/__init__.py $(INSTALL_XPAD_DIR)
	$(call install_core_dep,$(INSTALL_XPAD_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimaxpad.so*
	cp -d build/liblimaxpad.so* $(INSTALL_LIB_DIR)
	cp sip/xpad/limaxpad.so $(INSTALL_XPAD_VDIR)
	cp camera/xpad/include/*.h $(INSTALL_INCLUDE_DIR)
endif

ifneq ($(COMPILE_MARCCD), 0)
	mkdir -p $(INSTALL_MARCCD_VDIR)
	cp camera/marccd/python/__init__.py $(INSTALL_MARCCD_DIR)
	$(call install_core_dep,$(INSTALL_MARCCD_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimamarccd.so*
	cp -d build/liblimamarccd.so.* $(INSTALL_LIB_DIR)
	cp -d third-party/yat/src/.libs/libyat.so* $(INSTALL_LIB_DIR)
	cp sip/marccd/limamarccd.so $(INSTALL_MARCCD_VDIR)
	cp camera/marccd/include/*.h $(INSTALL_INCLUDE_DIR)
endif

ifneq ($(COMPILE_POINTGREY), 0)
	mkdir -p $(INSTALL_POINTGREY_VDIR)
	cp camera/pointgrey/python/__init__.py $(INSTALL_POINTGREY_DIR)
	$(call install_core_dep,$(INSTALL_POINTGREY_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimapointgrey.so*
	cp -d build/liblimapointgrey.so* $(INSTALL_LIB_DIR)
	cp sip/pointgrey/limapointgrey.so $(INSTALL_POINTGREY_VDIR)
	cp camera/pointgrey/include/*.h $(INSTALL_INCLUDE_DIR)
endif

ifneq ($(COMPILE_IMXPAD), 0)
	mkdir -p $(INSTALL_IMXPAD_VDIR)
	cp camera/imxpad/python/__init__.py $(INSTALL_IMXPAD_DIR)
	$(call install_core_dep,$(INSTALL_IMXPAD_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimaimxpad.so*
	cp -d build/liblimaimxpad.so* $(INSTALL_LIB_DIR)
	cp sip/imxpad/limaimxpad.so $(INSTALL_IMXPAD_VDIR)
	cp camera/imxpad/include/*.h $(INSTALL_INCLUDE_DIR)
endif

ifneq ($(COMPILE_RAYONIXHS), 0)
	mkdir -p $(INSTALL_RAYONIXHS_VDIR)
	cp camera/rayonixhs/python/__init__.py $(INSTALL_RAYONIXHS_DIR)
	$(call install_core_dep,$(INSTALL_RAYONIXHS_VDIR))
	rm -rf $(INSTALL_LIB_DIR)/liblimarayonixhs.so*
	cp -d build/liblimarayonixhs.so* $(INSTALL_LIB_DIR)
	cp -d camera/rayonixhs/sdk/lib64/libcraydl.so* $(INSTALL_LIB_DIR)
	cp -d camera/rayonixhs/sdk/lib64/libcraydl_gpu.so* $(INSTALL_LIB_DIR)
	cp -d camera/rayonixhs/sdk/lib64/libcbf.so* $(INSTALL_LIB_DIR)
	cp -dr camera/rayonixhs/sdk/lib64/libcraydl_nocuda $(INSTALL_LIB_DIR)
	cp sip/rayonixhs/limarayonixhs.so $(INSTALL_RAYONIXHS_VDIR)
	cp camera/rayonixhs/include/*.h $(INSTALL_INCLUDE_DIR)
endif
