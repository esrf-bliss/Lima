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
import gc, weakref
from Lima.Core import *
from Lima import Espia
from limafrelon import *
from processlib import Tasks

class FrelonAcq:

    DEB_CLASS(DebModApplication, 'FrelonAcq')


    class BinChangedCallback(Frelon.BinChangedCallback):

        DEB_CLASS(DebModApplication, "FrelonAcq.BinChangedCallback")
        
        @DEB_MEMBER_FUNCT
        def __init__(self, e2v_corr_update):
            self.m_e2v_corr_update = weakref.ref(e2v_corr_update)
            Frelon.BinChangedCallback.__init__(self)
            
        @DEB_MEMBER_FUNCT
        def hwBinChanged(self, hw_bin):
            e2v_corr_update = self.m_e2v_corr_update()
            if e2v_corr_update:
                e2v_corr_update.hwBinChanged(hw_bin);

            
    class RoiChangedCallback(Frelon.RoiChangedCallback):
        
        DEB_CLASS(DebModApplication, 'FrelonAcq.RoiChangedCallback')

        @DEB_MEMBER_FUNCT
        def __init__(self, e2v_corr_update):
            self.m_e2v_corr_update = weakref.ref(e2v_corr_update)
            Frelon.RoiChangedCallback.__init__(self)

        @DEB_MEMBER_FUNCT
        def hwRoiChanged(self, hw_roi):
            e2v_corr_update = self.m_e2v_corr_update()
            if e2v_corr_update:
                e2v_corr_update.hwRoiChanged(hw_roi);


    class E2VCorrectionUpdate:

        DEB_CLASS(DebModApplication, 'FrelonAcq.E2VCorrectionUpdate')

        @DEB_MEMBER_FUNCT
        def __init__(self, e2v_corr, hw_inter):
            self.m_reg_act = False
            self.m_e2v_corr = e2v_corr

            self.m_det_info_ctrl_obj = hw_inter.getHwCtrlObj(HwCap.DetInfo)
            self.m_bin_ctrl_obj      = hw_inter.getHwCtrlObj(HwCap.Bin)
            self.m_roi_ctrl_obj      = hw_inter.getHwCtrlObj(HwCap.Roi)
            
            self.m_hw_bin = Bin()
            self.m_bin_cb = FrelonAcq.BinChangedCallback(self)
            self.m_roi_cb = FrelonAcq.RoiChangedCallback(self)

        @DEB_MEMBER_FUNCT
        def __del__(self):
            self.setRegistrationActive(False)

        @DEB_MEMBER_FUNCT
        def setRegistrationActive(self, reg_act):
            if reg_act and not self.m_reg_act:
                self.m_bin_ctrl_obj.registerBinChangedCallback(self.m_bin_cb)
                self.m_roi_ctrl_obj.registerRoiChangedCallback(self.m_roi_cb)
                self.m_reg_act = True
            elif not reg_act and self.m_reg_act:
                self.m_bin_ctrl_obj.unregisterBinChangedCallback(self.m_bin_cb)
                self.m_roi_ctrl_obj.unregisterRoiChangedCallback(self.m_roi_cb)
                self.m_reg_act = False
            
        @DEB_MEMBER_FUNCT
        def hwBinChanged(self, hw_bin):
            deb.Param('hw_bin=%s' % hw_bin)
            self.m_hw_bin = hw_bin
            self.m_e2v_corr.setHwBin(hw_bin);
            
        @DEB_MEMBER_FUNCT
        def hwRoiChanged(self, hw_roi):
            deb.Param('hw_roi=%s' % hw_roi)
            if hw_roi.isEmpty():
                deb.Trace('Empty Roi, getting MaxImageSize')
                max_size = self.m_det_info_ctrl_obj.getMaxImageSize()
                w = max_size.getWidth()  / self.m_hw_bin.getX()
                h = max_size.getHeight() / self.m_hw_bin.getY()
                hw_roi = Roi(Point(0, 0), Size(w, h))
                deb.Trace('hw_roi=%s' % hw_roi)
            self.m_e2v_corr.setHwRoi(hw_roi);

            
    @DEB_MEMBER_FUNCT
    def __init__(self, espia_dev_nb):
        self.m_cam_inited    = False

        self.m_e2v_corr      = None
        self.m_e2v_corr_update = None
        self.m_e2v_corr_act  = True
        
        self.m_bpm_mgr       = Tasks.BpmManager()
        self.m_bpm_task      = Tasks.BpmTask(self.m_bpm_mgr)
        
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

        self.checkE2VCorrection()
        
    @DEB_MEMBER_FUNCT
    def __del__(self):
        if self.m_cam_inited:
            del self.m_ct_display, self.m_ct_buffer, self.m_ct_image, \
                self.m_ct_saving, self.m_ct_acq
            del self.m_ct;		gc.collect()

            del self.m_hw_inter;	gc.collect()
            del self.m_buffer_mgr;	gc.collect()
            del self.m_cam;		gc.collect()
            
        del self.m_eserline;		gc.collect()
        del self.m_buffer_cb_mgr;	gc.collect()
        del self.m_acq;			gc.collect()
        del self.m_edev;		gc.collect()

        if self.m_e2v_corr:
            del self.m_e2v_corr_update
            del self.m_e2v_corr;	gc.collect()
            
        del self.m_bpm_task;		gc.collect()
        del self.m_bpm_mgr;		gc.collect()
        
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

    @DEB_MEMBER_FUNCT
    def checkE2VCorrection(self):
        deb.Trace('Checking E2V correction')
        ct_status = self.m_ct.getStatus()
        if ct_status.AcquisitionStatus == AcqRunning:
            raise Exception, 'Acquisition is running'
        
        chip_type = self.m_cam.getModel().getChipType()
        is_e2v = (chip_type == Frelon.E2V)
        corr_act = (is_e2v and self.m_e2v_corr_act)
        deb.Param('is_e2v=%s, self.m_e2v_corr_act=%s' % (is_e2v,
                                                         self.m_e2v_corr_act))
        if corr_act and not self.m_e2v_corr:
            deb.Trace('Enabling E2V correction')
            self.m_e2v_corr = Frelon.E2VCorrection()
            self.m_e2v_corr_update = self.E2VCorrectionUpdate(self.m_e2v_corr,
                                                              self.m_hw_inter)
            self.m_e2v_corr_update.setRegistrationActive(True)
            self.m_ct.setReconstructionTask(self.m_e2v_corr)
        elif not corr_act and self.m_e2v_corr:
            deb.Trace('Disabling E2V correction')
            self.m_ct.setReconstructionTask(None)
            self.m_e2v_corr_update.setRegistrationActive(False)
            self.m_e2v_corr_update = None
            self.m_e2v_corr = None
        
    @DEB_MEMBER_FUNCT
    def reset(self):
        deb.Trace('Reseting the device!')
        self.m_ct.reset()

    @DEB_MEMBER_FUNCT
    def getCameraModel(self):
        model = self.m_cam.getModel()
        deb.Return('Getting camera model')
        return model

    @DEB_MEMBER_FUNCT
    def getCameraModelStr(self):
        model = self.m_cam.getModel()
        model_str = 'Frelon %s #%d, FW:%s' % \
                    (model.getName(), model.getSerialNb(),
                     model.getFirmware().getVersionStr())
        deb.Return('Getting camera model str: %s' % model_str)
        return model_str

    @DEB_MEMBER_FUNCT
    def getStatus(self):
        ct_status = self.m_ct.getStatus()
        deb.Return('Getting global status: %s' % ct_status)
        return ct_status
        
    @DEB_MEMBER_FUNCT
    def getCcdStatus(self):
        ccd_status = self.m_cam.getStatus()
        deb.Return('Getting CCD status: 0x%02X' % ccd_status)
        return ccd_status
    
    @DEB_MEMBER_FUNCT
    def getFrameDim(self, max_dim=False):
        if max_dim:
            max_size = self.m_ct_image.getMaxImageSize()
            fdim = FrameDim(max_size, self.m_ct_image.getImageType())
        else:
            fdim = self.m_ct_image.getImageDim()
        deb.Return('Frame dim: %s' % fdim)
        return fdim
    
    @DEB_MEMBER_FUNCT
    def setFlip(self, flip):
        deb.Param('Setting flip mode: %s' % flip)
        self.m_ct_image.setFlip(flip)
    
    @DEB_MEMBER_FUNCT
    def getFlip(self):
        flip = self.m_ct_image.getFlip()
        deb.Return('Getting flip mode: %s' % flip)
        return flip
    
    @DEB_MEMBER_FUNCT
    def setBin(self, bin):
        deb.Param('Setting binning: %s' % bin)
        self.m_ct_image.setBin(bin)

    @DEB_MEMBER_FUNCT
    def getBin(self):
        bin = self.m_ct_image.getBin()
        deb.Return('Getting binning: %s' % bin)
        return bin

    @DEB_MEMBER_FUNCT
    def getMaxRoi(self):
        max_roi_size = self.m_ct_image.getMaxImageSize()
        max_roi_size /= Point(self.m_ct_image.getBin())
        max_roi = Roi(Point(0, 0), max_roi_size)
        deb.Return('Max roi: %s' % max_roi)
        return max_roi
        
    @DEB_MEMBER_FUNCT
    def setRoi(self, roi):
        deb.Param('Setting roi: %s' % roi)
        if roi == self.getMaxRoi():
            roi = Roi()
        self.m_ct_image.setRoi(roi)

    @DEB_MEMBER_FUNCT
    def getRoi(self):
        roi = self.m_ct_image.getRoi()
        if roi.isEmpty():
            roi = self.getMaxRoi()
        deb.Return('Getting roi: %s' % roi)
        return roi

    @DEB_MEMBER_FUNCT
    def setRoiMode(self, roi_mode):
        deb.Param('Setting roi mode: %s' % roi_mode)
        self.m_cam.setRoiMode(roi_mode)
        
    @DEB_MEMBER_FUNCT
    def getRoiMode(self):
        roi_mode = self.m_cam.getRoiMode()
        deb.Return('Getting roi mode: %s' % roi_mode)
        return roi_mode

    @DEB_MEMBER_FUNCT
    def setKinPars(self, kin_win_size, kin_line_beg, kin_stripes):
        deb.Param('Setting kin pars: ' +
                  'kin_win_size=%s, kin_line_beg=%s, kin_stripes=%s' % \
                  (kin_win_size, kin_line_beg, kin_stripes))
        if kin_stripes > 1:
            deb.Warning('Ignoring kin_stripes=%d' % kin_stripes)
            
        bin = self.m_ct_image.getBin()
        if kin_win_size % bin.getY() != 0:
            msg = 'Invalid kinetics window size (%d): ' % kin_win_size + \
                  'must be multiple of vert. bin (%d)' % bin.getY()
            raise Exception, msg

        roi = self.m_ct_image.getRoi()
        if roi.isEmpty():
            roi = self.getMaxRoi()
        roi = roi.getUnbinned(bin)

        tl = Point(roi.getTopLeft().x, kin_line_beg)
        tl_aligned = Point(tl)
        tl_aligned.alignTo(Point(bin), Floor)
        size = Size(roi.getSize().getWidth(), kin_win_size)

        roi = Roi(tl_aligned, size)
        roi = roi.getBinned(bin)
        self.m_ct_image.setRoi(roi)
        
        roi_bin_offset  = tl
        roi_bin_offset -= tl_aligned

        self.m_cam.setRoiBinOffset(roi_bin_offset)
        
    @DEB_MEMBER_FUNCT
    def getKinPars(self):
        bin = self.m_ct_image.getBin()
        roi = self.m_ct_image.getRoi()
        if roi.isEmpty():
            roi = self.getMaxRoi()
        roi = roi.getUnbinned(bin)
        kin_win_size = roi.getSize().getHeight()
        tl  = roi.getTopLeft()
        tl += self.m_cam.getRoiBinOffset()
        kin_line_beg = tl.y
        kin_stripes = 1
        deb.Return('Getting kin pars: ' +
                   'kin_win_size=%s, kin_line_beg=%s, kin_stripes=%s' % \
                   (kin_win_size, kin_line_beg, kin_stripes))
        return kin_win_size, kin_line_beg, kin_stripes

    @DEB_MEMBER_FUNCT
    def setFrameTransferMode(self, ftm):
        deb.Param('Setting frame transfer mode: ftm=%s' % ftm)
        self.m_cam.setFrameTransferMode(ftm)
    
    @DEB_MEMBER_FUNCT
    def getFrameTransferMode(self):
        ftm = self.m_cam.getFrameTransferMode()
        deb.Return('Getting frame transfer mode: ftm=%s' % ftm)
        return ftm
    
    @DEB_MEMBER_FUNCT
    def setInputChan(self, input_chan):
        input_chan = Frelon.InputChan(input_chan)
        ftm = self.m_cam.getFrameTransferMode()
        mode_name = self.m_cam.getInputChanModeName(ftm, input_chan)
        deb.Param('Setting input channel: %s [%s]' % (input_chan, mode_name))
        self.m_cam.setInputChan(input_chan)
    
    @DEB_MEMBER_FUNCT
    def getInputChan(self):
        input_chan = self.m_cam.getInputChan()
        ftm = self.m_cam.getFrameTransferMode()
        mode_name = self.m_cam.getInputChanModeName(ftm, input_chan)
        deb.Return('Getting input channel: %s [%s]' % (input_chan, mode_name))
        return input_chan
        
    @DEB_MEMBER_FUNCT
    def setTriggerMode(self, trig_mode):
        deb.Param('Setting trigger mode: %s' % trig_mode)
        exp_time = prev_exp_time = self.m_ct_acq.getAcqExpoTime()
        if trig_mode == IntTrig and prev_exp_time == 0:
            exp_time = 1.0
        self.m_ct_acq.setTriggerMode(trig_mode)
        if exp_time != prev_exp_time:
            self.m_ct_acq.setAcqExpoTime(exp_time)
    
    @DEB_MEMBER_FUNCT
    def getTriggerMode(self):
        trig_mode = self.m_ct_acq.getTriggerMode()
        deb.Return('Getting trigger mode: %s' % trig_mode)
        return trig_mode
    
    @DEB_MEMBER_FUNCT
    def setNbFrames(self, nb_frames):
        deb.Param('Setting nb. frames: %s' % nb_frames)
        self.m_ct_acq.setAcqNbFrames(nb_frames)
    
    @DEB_MEMBER_FUNCT
    def getNbFrames(self):
        nb_frames = self.m_ct_acq.getAcqNbFrames()
        deb.Return('Getting nb. frames: %s' % nb_frames)
        return nb_frames
    
    @DEB_MEMBER_FUNCT
    def setExpTime(self, exp_time):
        deb.Param('Setting exp. time: %s' % exp_time)
        trig_mode = self.m_ct_acq.getTriggerMode()
        if exp_time == 0 and trig_mode == ExtTrigSingle:
            self.m_ct_acq.setTriggerMode(ExtGate)
        elif exp_time > 0 and trig_mode == ExtGate:
            self.m_ct_acq.setTriggerMode(ExtTrigSingle)
        self.m_ct_acq.setAcqExpoTime(exp_time)
    
    @DEB_MEMBER_FUNCT
    def getExpTime(self):
        exp_time = self.m_ct_acq.getAcqExpoTime()
        deb.Return('Getting exp. time: %s' % exp_time)
        return exp_time

    @DEB_MEMBER_FUNCT
    def setFilePar(self, file_par):
        deb.Param('Setting file par: %s' % file_par)
        self.m_ct_saving.setParameters(file_par)
        
    @DEB_MEMBER_FUNCT
    def getFilePar(self):
        file_par = self.m_ct_saving.getParameters()
        deb.Return('Getting file par: %s' % file_par)
        return file_par
        
    @DEB_MEMBER_FUNCT
    def writeFile(self, frame_nb):
        deb.Param('Writing frame %s to file' % frame_nb)
        
    @DEB_MEMBER_FUNCT
    def setAutosave(self, autosave_act):
        deb.Param('Setting autosave active: %s' % autosave_act)
        if autosave_act:
            saving_mode = CtSaving.AutoFrame
        else:
            saving_mode = CtSaving.Manual
        self.m_ct_saving.setSavingMode(saving_mode)
    
    @DEB_MEMBER_FUNCT
    def getAutosave(self):
        saving_mode = self.m_ct_saving.getSavingMode()
        autosave_act = (saving_mode == CtSaving.AutoFrame)
        deb.Return('Getting autosave active: %s' % autosave_act)
        return autosave_act

    @DEB_MEMBER_FUNCT
    def setLiveDisplay(self, livedisplay_act):
        deb.Param('Setting live display active: %s' % livedisplay_act)
        self.m_ct_display.setNames('_ccd_ds_', 'frelon_live')
        self.m_ct_display.setActive(livedisplay_act)
        
    @DEB_MEMBER_FUNCT
    def getLiveDisplay(self):
        livedisplay_act = self.m_ct_display.isActive()
        deb.Return('Getting live display active: %s' % livedisplay_act)
        return livedisplay_act

    @DEB_MEMBER_FUNCT
    def startLive(self):
        deb.Trace('Starting live mode')
        self.setNbFrames(0)
        self.startAcq()
        
    @DEB_MEMBER_FUNCT
    def stopLive(self):
        deb.Trace('Stoping live mode')
        self.stopAcq()
        
    @DEB_MEMBER_FUNCT
    def startAcq(self):
        deb.Trace('Starting the device')
        self.m_ct.prepareAcq()
        self.m_ct.startAcq()
        
    @DEB_MEMBER_FUNCT
    def stopAcq(self):
        deb.Trace('Stopping the device')
        self.m_ct.stopAcq()

    @DEB_MEMBER_FUNCT
    def readFrame(self, frame_nb):
        img_data = self.m_ct.ReadImage(frame_nb)
        data = img_data.buffer
        s = data.tostring()
        deb.Return('Getting frame #%s: %s bytes' % (frame_nb, len(s)))
        return s
        
    @DEB_MEMBER_FUNCT
    def execFrelonSerialCmd(self, cmd):
        deb.Param('Executing Frelon serial cmd: %s' % cmd)
        ser_line = self.m_cam.getSerialLine()
        ser_line.write(cmd)
        resp = ser_line.readLine()
        deb.Return('Received response:')
        for line in resp.split('\r\n'):
            deb.Return(line)
        return resp
        
    @DEB_MEMBER_FUNCT
    def readBeamParams(self):
        frame_nb = -1
        img_data = self.m_ct.ReadImage(frame_nb)
        beam_params = self.calcBeamParams(img_data)
        deb.Return('Beam params: %s' % beam_params)
        return beam_params

    @DEB_MEMBER_FUNCT
    def calcBeamParams(self, img_data):
        self.m_bpm_task.process(img_data)
        timeout = 1
        bpm_pars = self.m_bpm_mgr.getResult(timeout)
        if bpm_pars.errorCode != self.m_bpm_mgr.OK:
            raise Exception, ('Error calculating beam params: %d' %
                              bpm_pars.errorCode)
        
        nr_spots = 1
        auto_cal = -1
        exp_time = self.getExpTime()
        if exp_time > 0:
            norm_intensity = bpm_pars.beam_intensity / exp_time
        else:
            norm_intensity = 0

        beam_params = [nr_spots,
                       bpm_pars.beam_intensity,
                       bpm_pars.beam_center_x,
                       bpm_pars.beam_center_y,
                       bpm_pars.beam_fwhm_x,
                       bpm_pars.beam_fwhm_y,
                       bpm_pars.AOI_max_x - bpm_pars.AOI_min_x,
                       bpm_pars.AOI_max_y - bpm_pars.AOI_min_y,
                       bpm_pars.max_pixel_value,
                       bpm_pars.max_pixel_x,
                       bpm_pars.max_pixel_y,
                       bpm_pars.AOI_min_x,
                       bpm_pars.AOI_min_y,
                       bpm_pars.AOI_max_x,
                       bpm_pars.AOI_max_y,
                       bpm_pars.beam_center_x - bpm_pars.beam_fwhm_x / 2,
                       bpm_pars.beam_center_y - bpm_pars.beam_fwhm_y / 2,
                       bpm_pars.beam_center_x + bpm_pars.beam_fwhm_x / 2,
                       bpm_pars.beam_center_y + bpm_pars.beam_fwhm_y / 2,
                       norm_intensity,
                       auto_cal]
        deb.Return('Getting beam params: %s' % beam_params)
        return beam_params

    @DEB_MEMBER_FUNCT
    def setE2VCorrectionActive(self, e2v_corr_act):
        deb.Param('Setting e2v_corr_act to %s' % e2v_corr_act)
        self.m_e2v_corr_act = e2v_corr_act
        self.checkE2VCorrection()

    @DEB_MEMBER_FUNCT
    def getE2VCorrectionActive(self):
        e2v_corr_act = self.m_e2v_corr_act
        deb.Param('Getting e2v_corr_act: %s' % e2v_corr_act)
        return e2v_corr_act
