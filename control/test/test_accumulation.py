import time
import numpy as np
from tempfile import gettempdir

import pytest

from Lima import Core


ACQ_EXPO_TIME = 0.1
ACQ_NB_FRAMES = 5
ACC_NB_FRAMES = 10 # Number of frames per window

def wait_acq_finished(simu: Core.CtControl, timeout = 5.0):
    """Wait for the end of the acquisition
    
    raise:
        RuntimeError if the wait timeout

    """

    SLEEP = 0.1
    retry = int(timeout / SLEEP)

    while simu.getStatus().AcquisitionStatus == Core.AcqRunning and retry > 0:
        time.sleep(SLEEP)
        retry -= 1

    if retry == 0:
        raise RuntimeError("Acquisition finished TIMEOUT")

def prepare(simu: Core.CtControl, acc_output_type = None):


    acq = simu.acquisition()
    acq.setAcqMode(Core.Accumulation)
    acq.setAcqExpoTime(ACQ_EXPO_TIME)
    acq.setAcqNbFrames(ACQ_NB_FRAMES)
    acq.setAccMaxExpoTime(ACQ_EXPO_TIME / ACC_NB_FRAMES)

    acc = simu.accumulation()
    if (acc_output_type):
        acc.setOutputType(acc_output_type)

    assert acc.getMode() ==  Core.CtAccumulation.Parameters.STANDARD
    assert acc.getThresholdBefore() == 0.
    assert acc.getOffsetBefore() == 0.

    assert acq.getAccNbFrames() == ACC_NB_FRAMES
    assert acq.getAccExpoTime() == ACQ_EXPO_TIME / ACC_NB_FRAMES

    #sav = simu.saving()
    #sav.setDirectory(gettempdir())
    #sav.setPrefix("test_acc")
    #sav.setFormat(Core.CtSaving.FileFormat.HDF5)
    #sav.setSavingMode(Core.CtSaving.SavingMode.AutoFrame)
    #sav.setOverwritePolicy(Core.CtSaving.Overwrite)

    simu.prepareAcq()

def test_accumulation_standard(debug, simu):

    prepare(simu)

    simu.startAcq()
    wait_acq_finished(simu, timeout = (ACQ_EXPO_TIME * ACQ_NB_FRAMES + 1))

    status  = simu.getImageStatus()
    assert status.LastImageAcquired + 1 == ACQ_NB_FRAMES

    img_size = simu.image().getImageDim().getSize()
    
    frm = simu.ReadImage()
    assert frm.buffer.shape == (img_size.getHeight(), img_size.getWidth())
    
    comparison = frm.buffer == np.full(frm.buffer.shape, ACC_NB_FRAMES)
    assert comparison.all()

def is_signed(img_type):
    if img_type == Core.Bpp16S or img_type == Core.Bpp8S:
        return True
    else:
        return False

def test_accumulation_standard_with_output_type(debug, simu):

    img_type = simu.image().getImageType()

    if is_signed(img_type):
        prepare(simu, Core.Bpp16S)
    else:
        prepare(simu, Core.Bpp16)

    simu.prepareAcq()

    simu.startAcq()
    wait_acq_finished(simu, timeout = (ACQ_EXPO_TIME * ACQ_NB_FRAMES + 1))
    time.sleep(0.1)

    status  = simu.getImageStatus()
    assert status.LastImageAcquired + 1 == ACQ_NB_FRAMES

    img_size = simu.image().getImageDim().getSize()
    
    frm = simu.ReadImage()
    assert frm.buffer.shape == (img_size.getHeight(), img_size.getWidth())
    
    comparison = frm.buffer == np.full(frm.buffer.shape, ACC_NB_FRAMES)
    assert comparison.all()

    