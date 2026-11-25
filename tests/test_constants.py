from __future__ import annotations

from lima import core


def test_det_status_is_bitwise_combinable():
    # DetStatus values are bit flags (HwInterface::Status.det combines several,
    # e.g. camera/frelon ORs in DetWaitForTrigger on top of the current phase).
    # The sip binding must support &/| or any bitwise test on status.det raises
    # TypeError (see applications/tango/python's ready_for_next_image attribute).
    combined = core.DetStatus.DetExposure | core.DetStatus.DetWaitForTrigger
    assert combined & core.DetStatus.DetWaitForTrigger
    assert combined & core.DetStatus.DetExposure
    assert not (combined & core.DetStatus.DetFault)
    assert not (core.DetStatus.DetIdle & core.DetStatus.DetWaitForTrigger)
