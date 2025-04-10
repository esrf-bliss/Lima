from __future__ import annotations

import os.path
import logging
from Lima import Core
from .mocked_camera import MockedCamera
from .lima_helper import LimaHelper

try:
    import h5py
except ImportError:
    h5py = None


_logger = logging.getLogger(__name__)


def test_h5(lima_helper: LimaHelper, tmp_path):
    cam = MockedCamera()
    ct_control = lima_helper.control(cam)

    saving = ct_control.saving()
    saving.setDirectory(str(tmp_path))
    saving.setPrefix("test")
    saving.setSuffix(".h5")
    saving.setFormat(Core.CtSaving.FileFormat.HDF5)
    saving.setSavingMode(Core.CtSaving.SavingMode.AutoFrame)

    lima_helper.process_acquisition(ct_control)

    filename = str(tmp_path / "test0000.h5")
    assert os.path.exists(filename)

    if h5py is None:
        _logger.warning("h5py not installed. Some checks are skipped.")
        return

    with h5py.File(filename) as h5:
        measurement_group = h5["/entry_0000/measurement"]
        assert measurement_group["data"].shape == (1, 8, 16)
        instrument_group = h5["/entry_0000/instrument/Mock"]
        assert instrument_group["image_operation/bin_mode"].asstr()[()] == "Bin_Sum"
