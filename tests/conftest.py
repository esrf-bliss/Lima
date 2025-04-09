import pytest
from .lima_helper import LimaHelper


@pytest.fixture
def lima_helper():
    helper = LimaHelper()
    yield helper
    helper.release_all()
