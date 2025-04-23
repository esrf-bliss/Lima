import pytest
from Lima import Core
from .lima_helper import LimaHelper


@pytest.fixture
def lima_helper():
    helper = LimaHelper()
    yield helper
    helper.release_all()


def pytest_addoption(parser):
    parser.addoption(
        "--lima-all-flags",
        action="store_true",
        default=False,
        help="Active Lima logging of all debug flags",
    )


@pytest.fixture(scope="session", autouse=True)
def session_setup(request):
    lima_all_flags = request.config.getoption("--lima-all-flags")
    if lima_all_flags:
        Core.DebParams.setTypeFlags(Core.DebParams.AllFlags)
