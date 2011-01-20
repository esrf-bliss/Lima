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
import gc
from Lima.Core import *
from Lima import Espia
from limafrelon import *

class FrelonAcq:

    DEB_CLASS(DebModApplication, "FrelonAcq")

    @DEB_MEMBER_FUNCT
    def __init__(self, espia_dev_nb):
        self.m_cam_inited    = False
        
        self.m_edev          = Espia.Dev(espia_dev_nb)
        self.m_acq           = Espia.Acq(self.m_edev)
        self.m_buffer_cb_mgr = Espia.BufferMgr(self.m_acq)
        self.m_eserline      = Espia.SerialLine(self.m_edev)
        self.m_cam           = Frelon.Camera(self.m_eserline)
        self.m_buffer_mgr    = BufferCtrlMgr(self.m_buffer_cb_mgr)
        self.m_hw_inter      = Frelon.Interface(self.m_acq, self.m_buffer_mgr,
                                                self.m_cam)

        self.m_ct            = CtControl(self.m_hw_inter)
        self.m_ct_acq        = self.m_ct.acquisition()
        self.m_ct_saving     = self.m_ct.saving()
        self.m_ct_image      = self.m_ct.image()
        self.m_ct_buffer     = self.m_ct.buffer()
        self.m_ct_display    = self.m_ct.display()
        
        self.m_cam_inited    = True
        
    @DEB_MEMBER_FUNCT
    def __del__(self):
        if self.m_cam_inited:
            del self.m_ct_display, self.m_ct_buffer, self.m_ct_image, \
                self.m_ct_saving, self.m_ct_acq
            del self.m_ct;			gc.collect()

            del self.m_hw_inter;		gc.collect()
            del self.m_buffer_mgr;		gc.collect()
            del self.m_cam;			gc.collect()
            
        del self.m_eserline;		gc.collect()
        del self.m_buffer_cb_mgr;	gc.collect()
        del self.m_acq;			gc.collect()
        del self.m_edev;		gc.collect()

    def getEspiaDev(self):
        return self.m_edev

    def getEspiaAcq(self):
        return self.m_acq

    def getEspiaBufferMgr(self):
        return self.m_buffer_cb_mgr

    def getEspiaSerialLine(self):
        return self.m_eserline

    def getFrelonCamera(self):
        return self.m_cam

    def getBufferMgr(self):
        return self.m_buffer_mgr

    def getFrelonInterface(self):
        return self.m_hw_inter

    def getGlobalControl(self):
        return self.m_ct

    def getAcqControl(self):
        return self.m_ct_acq
    
    def getSavingControl(self):
        return self.m_ct_saving

    def getImageControl(self):
        return self.m_ct_image

    def getBufferControl(self):
        return self.m_ct_buffer

    def getDisplayControl(self):
        return self.m_ct_display
