from lima import core


def test_write_read():
    """Check that we can write and read a ROI"""
    myroi = core.Roi(0, 0, 100, 100)
    my_opt_ext = core.SoftOpExternalMgr()

    roictmgr = my_opt_ext.addOp(core.SoftOpId.ROICOUNTERS, "titi", 0)

    roictmgr.updateRois([("myroi", myroi)])

    roi_check = roictmgr.getRois()
    assert roi_check[0][0] == "myroi"
    assert roi_check[0][1] == myroi
