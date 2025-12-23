import pytest
from lima import core
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
    parser.addoption(
        "--lima-simulator",
        action="store_true",
        default=False,
        help="Use the lima simulator for testing, instead of a mock (when test was setup for)",
    )


@pytest.fixture(scope="session", autouse=True)
def session_setup(request):
    lima_all_flags = request.config.getoption("--lima-all-flags")
    if lima_all_flags:
        core.DebParams.setTypeFlags(core.DebParams.AllFlags)
