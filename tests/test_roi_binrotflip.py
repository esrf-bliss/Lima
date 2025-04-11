import pytest
import typing
from Lima import Core
from .mocked_camera import MockedCamera
from .lima_helper import LimaHelper


def transformations() -> typing.Generator[typing.Any, None, None]:
    rois = {
        "full": Core.Roi(0, 0, 600, 300),
        "crop1": Core.Roi(1, 1, 598, 298),
    }
    rots = {
        "0deg": Core.Rotation_0,
        "90deg": Core.Rotation_90,
        "180deg": Core.Rotation_180,
        "270deg": Core.Rotation_270,
    }
    binnings = {
        "1x1": Core.Bin(1, 1),
        "1x2": Core.Bin(1, 2),
        "2x1": Core.Bin(2, 1),
        "2x2": Core.Bin(2, 2),
    }
    flips = {
        "no": Core.Flip(False, False),
        "v": Core.Flip(False, True),
        "h": Core.Flip(True, False),
        "hv": Core.Flip(True, True),
    }

    def permutation(lst):
        """Print permutations of a given list (from SO)"""
        if len(lst) == 0:
            return []

        if len(lst) == 1:
            return [lst]

        result = []
        for i in range(len(lst)):
            m = lst[i]

            remLst = lst[:i] + lst[i+1:]

            for p in permutation(remLst):
                result.append([m] + p)
        return result

    permutations = permutation(['b', 'f', 'r'])
    # For every permutation of every Bin Rot Flip parameters applied in any order
    for roi_name, roi in rois.items():
        for rot_name, rot in rots.items():
            for bin_name, bin in binnings.items():
                for flip_name, flip in flips.items():
                    for perm in permutations:
                        perm_name = "".join(perm)
                        yield pytest.param(roi, perm, bin, rot, flip, id=f"{roi_name}_{perm_name}_{rot_name}_{bin_name}_{flip_name}")


@pytest.mark.parametrize(
    "roi, perm, bin, rot, flip", transformations()
)
def test_transformations(lima_helper: LimaHelper, roi, perm, bin, rot, flip):
    """
    Check that basic transformations does not fail.
    """
    cam = MockedCamera(
        supports_sum_binning=True,
        supports_roi=True,
    )
    cam.width = 600
    cam.height = 300
    cam.bpp = Core.Bpp32

    image = lima_helper.image(cam)
    image.setRoi(roi)

    for op in perm:
        if op == 'f':
            image.setFlip(flip)
        elif op == 'b':
            image.setBin(bin)
        elif op == 'r':
            image.setRotation(rot)

    # Final check
    image.resetBin()
    image.resetFlip()
    image.resetRotation()
