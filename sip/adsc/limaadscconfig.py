import sipconfig

# These are installation specific values created when lima was configured.
# The following line will be replaced when this template is used to create
# the final configuration module.
_pkg_config = {
    'lima_sip_dir':  '/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/share/sip'
}

_default_macros = None

class Configuration(sipconfig.Configuration):
    """The class that represents lima configuration values.
    """
    def __init__(self, sub_cfg=None):
        """Initialise an instance of the class.

        sub_cfg is the list of sub-class configurations.  It should be None
        when called normally.
        """
        # This is all standard code to be copied verbatim except for the
        # name of the module containing the super-class.
        if sub_cfg:
            cfg = sub_cfg
        else:
            cfg = []

        cfg.append(_pkg_config)

        sipconfig.Configuration.__init__(self, cfg)

class limaModuleMakefile(sipconfig.ModuleMakefile):
    """The Makefile class for modules that %Import lima.
    """
    def finalise(self):
        """Finalise the macros.
        """
        # In case a C++ library is needed for link.
	# Not necessary here, it's done in the configure.py script
        # self.extra_libs.append("lima")

        # Let the super-class do what it needs to.
        sipconfig.ModuleMakefile.finalise(self)

