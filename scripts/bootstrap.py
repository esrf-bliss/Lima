###########################################################################
# This file is part of LImA, a Library for Image Acquisition
#
#  Copyright (C) : 2009-2017
#  European Synchrotron Radiation Facility
#  BP 220, Grenoble 38043
#  FRANCE
# 
#  Contact: lima@esrf.fr
# 
#  This is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
# 
#  This software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
import sys, os
import platform, multiprocessing
from subprocess import Popen, PIPE
import contextlib
import argparse

prog_description = 'Lima build and install tool'
prog_instructions = '''
Description:
    This script will build and eventually install Lima project.

    The build and install process default configuration is determined by
    the scripts/config.txt file, which is created the very first time from
    the scripts/config.txt_default template. The file contains the list of
    variables that are passed to CMake. By default, only Lima-related
    variables are included, but any other can be added. If you plan to
    execute the process several times with the same parameters, you can
    edit this configuration file and fix your options there. By default
    the simulator camera plugin is compiled. You can change that by
    setting the corresponding "LIMACAMERA_SIMULATOR=0" option in
    config.txt.

    Running ./install.sh with no parameter will just build Lima with the
    options in config.txt. No installation will be performed. If at least
    one of --install-prefix or --install-python-prefix option is specified
    the --install=yes option is assumed (unless --install=no is explicitly
    specified)

    If not absolute paths, the --config-file option will be assumed to be
    relative to the source-prefix, and --build-prefix relative to the CWD.

Module/option description:
    It can be any camera name or saving format.
    Available saving formats: edf, cbf, tiff, lz4, gz, hdf5, fits.
    Other otions are:
     + python: Build Python wrapping.
     + pytango-server: install the PyTango server Python code
     + tests: build tests (in order to run them execute "ctest" in <build>)
     + config, sps-image, gldisplay: for the fun!

Examples:
    ./install.[bat | sh] --install=yes basler python cbf
        -> compile and install Lima with cameras simulator and basler with
           Python wrapping and cbf saving format.
        -> install directory for C library and Python library will be in
           default directory.

        This is equivalent to adding the following options in config.txt:
           + LIMACAMERA_BASLER=1
           + LIMA_ENABLE_CBF=1
           + LIMA_ENABLE_PYTHON=1

    ./install.[bat | sh] --install-prefix=${HOME} tests
        -> compile and install Lima with camera simulator, also compiling
           simulator tests.
        -> the install directory is set in the home directory (${HOME})

        This is equivalent to adding the following options in config.txt:
           + LIMA_ENABLE_TESTS=1
           + CMAKE_INSTALL_PREFIX=<path_to_home>

    ONLY ON LINUX:
    ./install.sh --git [options]
        -> clone and update (checkout) on every (sub)module in [options]
'''


OS_TYPE = platform.system()
if OS_TYPE not in ['Linux', 'Windows']:
	sys.exit('Platform not supported: ' + OS_TYPE)

def exec_cmd(cmd, exc_msg=''):
	print('Executing:' + cmd)
	sys.stdout.flush()
	ret = os.system(cmd)
	if ret != 0:
		raise Exception('%s [%s]' % (exc_msg, cmd))


@contextlib.contextmanager
def ch_dir(new_dir):
	cur_dir = os.getcwd()
	os.chdir(new_dir)
	yield
	os.chdir(cur_dir)


class Config:

	bool_map = {'yes': True, 'no': False}

	@classmethod
	def get_bool_opt_default(klass, val):
		for o, v in klass.bool_map.items():
			if val == v:
				return '__%s__' % o
		raise ValueError('Invalid value: ' + val)

	# return (val, explicit), where explicit is True if val was
	# specified as argument, or False if val is the default option value
	@classmethod
	def get_bool_opt(klass, val):
		val = val.lower()
		for o, v in klass.bool_map.items():
			if val == o:
				return v, True
			if val == '__%s__' % o:
				return v, False
		raise ValueError('Invalid value: ' + val)
		
	def __init__(self, argv=None):
		self.cmd_opts = None
		self.config_opts = None
		self.cmake_opts = None
		self.git = None

		if argv is not None:
			self.decode_args(argv)

	def decode_args(self, argv):
		build_type = ('RelWithDebInfo' if OS_TYPE == 'Linux' 
			      else 'Release')
		cwd = os.getcwd()
		src = os.path.realpath(os.path.join(os.path.dirname(argv[0]), 
						    os.path.pardir))
		formatter = argparse.RawDescriptionHelpFormatter
		parser = argparse.ArgumentParser(formatter_class=formatter,
						 description=prog_description,
						 epilog=prog_instructions)
		parser.add_argument('--git', action='store_true',
				    help='init/update Git submodules')
		parser.add_argument('--find-root-path',
				    help='CMake find_package/library root path')
		parser.add_argument('--source-prefix', default=src,
				    help='path to the Lima sources')
		parser.add_argument('--config-file', 
				    default='scripts/config.txt',
				    help='file with configuration options')
		parser.add_argument('--build-prefix', default='build',
				    help='directory where binaries are built')
		parser.add_argument('--build-type', default=build_type,
				    help='CMake build target')
		parser.add_argument('--install', 
				    default=self.get_bool_opt_default(False),
				    help='perform installation [yes, no]')
		parser.add_argument('--install-prefix',
				    help='directory where Lima is installed')
		parser.add_argument('--install-python-prefix',
				    help='install directory for Python code')
		parser.add_argument('mod_opts', metavar='mod_opt', nargs='+',
				    help='module/option to process')
		self.cmd_opts = parser.parse_args(argv[1:])

		# do install if not explicitly specified and user
		# included install-[python-]prefix
		install, explicit = self.get_bool_opt(self.get('install'))
		install_prefix = (self.get('install-prefix') or 
				  self.get('install-python-prefix'))
		install = True if not explicit and install_prefix else install
		self.set_cmd('install', install)

		# if option paths are relative, make them absolute:
		# config-file is rel. to src, build-prefix is rel. to cwd
		src = self.get('source-prefix')
		rel_opt_map = [(src, ['config-file']), (cwd, ['build-prefix'])]
		for base, opt_list in rel_opt_map:
			for opt in opt_list:
				p = self.get(opt)
				if p and not os.path.isabs(p):
					self.set_cmd(opt, os.path.join(base, p))

	def set_cmd(self, x, v):
		setattr(self.cmd_opts, self.to_underscore(x), v)

	def get(self, x):
		return getattr(self.cmd_opts, self.to_underscore(x))

	def get_git_options(self):
		return self.get('mod-opts')

	def get_cmd_options(self):
		opts = dict([(self.from_underscore(k), v)
			     for k, v in self.cmd_opts._get_kwargs()])
		for arg in opts.pop('mod-opts'):
			for oprefix, sdir in [("limacamera", "camera"), 
					      ("lima-enable", "third-party")]:
				sdir += '/'
				if arg.startswith(sdir):
					arg = oprefix + '-' + arg[len(sdir):]
			opts[arg] = True
		return opts

	def read_config(self):
		config_file = self.get('config-file')
		self.config_opts = []
		with open(config_file) as f:
			for line in f:
				line = line.strip()
				if not line or line.startswith('#'):
					continue
				opt, val = line.split('=')
				opt = self.from_underscore(opt).lower()
				val = int(val) if val.isdigit() else val
				self.config_opts.append((opt, val))

	def get_config_options(self):
		if self.config_opts is None:
			self.read_config()
		return self.config_opts

	def is_install_required(self):
		cmd_opts = self.get_cmd_options()
		install_prefix = cmd_opts.get('install-prefix', '')
		return cmd_opts.get('install', install_prefix != '')

	@staticmethod
	def to_underscore(x):
		return x.replace('-', '_')

	@staticmethod
	def from_underscore(x):
		return x.replace('_', '-')

class CMakeOptions:

	cmd_2_cmake_map = [
		('build-type', 'cmake-build-type'),
		('install-prefix', 'cmake-install-prefix'),
		('install-python-prefix', 'python-site-packages-dir'),
		('find-root-path', 'cmake-find-root-path')
	]

	def __init__(self, cfg):
		self.cfg = cfg


	# return options in config file activated (=1) if passed as arguments,
	# and also those not specified as empty (=) or disabled (=0|no) in file
	def get_configure_options(self):
		cmd_opts = self.cfg.get_cmd_options()
		config_opts = self.cfg.get_config_options()

		def is_active(v):
			if type(val) in [bool, int]:
				return val
			return val and (val.lower() not in [str(0), 'no'])

		cmake_opts = []
		for opt, val in config_opts:
			for cmd_opt, cmd_val in cmd_opts.items():
				if cmd_opt == opt:
					val = cmd_val
					break
				# arg-passed option must match the end
				# of opt a nd must be preceeded by 
				# the '_' separator
				t = opt.split(cmd_opt)
				if ((len(t) == 2) and 
				    (t[0][-1] == '-') and not t[1]):
					val = cmd_val
					break
			if is_active(val):
				cmake_opts.append((opt, val))

		for cmd_key, cmake_key in self.cmd_2_cmake_map:
			val = self.cfg.get(cmd_key)
			if is_active(val) and cmake_key not in dict(cmake_opts):
				cmake_opts.append((cmake_key, val))

		if OS_TYPE == 'Linux':
			cmake_gen = 'Unix Makefiles'
		elif OS_TYPE == 'Windows':
			# for windows check compat between installed python 
			# and mandatory vc++ compiler
			# See, https://wiki.python.org/moin/WindowsCompilers
			if sys.version_info < (2, 6):
				sys.exit("Only python > 2.6 supported")
			elif sys.version_info <= (3, 2):
				win_compiler = "Visual Studio 9 2008"
			elif sys.version_info <= (3, 4):
				win_compiler = "Visual Studio 10 2010" 
			else:
				win_compiler = "Visual Studio 15 2017"
			# now check architecture
			if platform.architecture()[0] == '64bit':
				win_compiler += ' Win64' 

			print ('Found Python ', sys.version)
			print ('Used compiler: ', win_compiler)
			cmake_gen = win_compiler

		source_prefix = self.cfg.get('source-prefix')
		opts = [source_prefix, '-G"%s"' % cmake_gen]
		opts += map(self.cmd_option, cmake_opts)
		return self.get_cmd_line_from_options(opts)

	def get_build_options(self):
		opts = ['--build .']
		if OS_TYPE == 'Linux':
			nb_jobs = multiprocessing.cpu_count() + 1
			opts += ['--', '-j %d' % nb_jobs]
		if OS_TYPE == 'Windows':
			opts += ['--config %s' %  self.cfg.get('build-type')]
		return self.get_cmd_line_from_options(opts)

	def get_install_options(self):
		opts = ['--build .', '--target install']
		if OS_TYPE == 'Windows':
			opts += ['--config %s' %  self.cfg.get('build-type')]
		return self.get_cmd_line_from_options(opts)

	@staticmethod
	def get_cmd_line_from_options(opts):
		return ' '.join(['cmake'] + opts)

	@staticmethod
	def cmd_option(opt_val):
		o, v = opt_val
		def quoted(x):
			if type(x) is bool:
				x = int(x)
			if type(x) is not str:
				x = str(x)
			return (('"%s"' % x) 
				if ' ' in x and not x.startswith('"') else x)
		return '-D%s=%s' % (Config.to_underscore(o).upper(), quoted(v))


class GitHelper:

	not_submodules = (
		'git', 'python', 'tests', 'test', 'cbf', 'lz4', 'fits', 'gz', 
		'tiff', 'hdf5'
	)

	submodule_map = {
		'espia': 'camera/common/espia',
		'pytango-server': 'applications/tango/python',
		'sps-image': 'Sps'
	}

	basic_submods = (
		'Processlib',
	)

	def __init__(self, cfg):
		self.cfg = cfg
		self.opts = self.cfg.get_git_options()

	def check_submodules(self, submodules=None):
		if submodules is None:
			submodules = self.opts
		submodules = list(submodules)
		for submod in self.basic_submods:
			if submod not in submodules:
				submodules.append(submod)

		root = self.cfg.get('source-prefix')
		with ch_dir(root):
			submod_list = []
			for submod in submodules:
				if submod in self.not_submodules:
					continue
				if submod in self.submodule_map:
					submod = self.submodule_map[submod]
				for sdir in ['third-party', 'camera']:
					s = os.path.join(sdir, submod)
					if os.path.isdir(s):
						submod = s
						break
				if os.path.isdir(submod):
					submod_list.append(submod)

			for submod in submod_list:
				self.update_submodule(submod)

	def update_submodule(self, submod):
		try:
			action = 'init ' + submod
			exec_cmd('git submodule ' + action)
			action = 'update ' + submod
			exec_cmd('git submodule ' + action)
			with ch_dir(submod):
				exec_cmd('git submodule init')
				cmd = ['git', 'submodule']
				p = Popen(cmd, stdout=PIPE)
				for l in p.stdout.readlines():
					tok = l.strip().split()
					self.update_submodule(tok[1])
		except Exception as e:
			sys.exit('Problem with submodule %s: %s' % (submod, e))

def build_install_lima(cfg):
	build_prefix = cfg.get('build-prefix')
	if not os.path.exists(build_prefix):
		os.mkdir(build_prefix)
	os.chdir(build_prefix)

	cmake_opts = CMakeOptions(cfg)
	cmake_cmd = cmake_opts.get_configure_options()
	exec_cmd(cmake_cmd, ('Something is wrong in CMake environment. ' +
			     'Make sure your configuration is good.'))

	cmake_cmd = cmake_opts.get_build_options()
	exec_cmd(cmake_cmd, ('CMake could not build Lima. ' + 
			     'Pleae contact lima@esrf.fr for help.'))

	if not cfg.is_install_required():
		return

	cmake_cmd = cmake_opts.get_install_options()
	exec_cmd(cmake_cmd, ('CMake could not install libraries. ' + 
			     'Make sure you have necessary rights.'))


def main():
	cfg = Config(sys.argv)

	# No git option under windows for obvious reasons.
	if OS_TYPE == 'Linux' and cfg.get('git'):
		git = GitHelper(cfg)
		git.check_submodules()

	try:
		build_install_lima(cfg)
	except Exception as e:
		sys.exit('Problem building/installing Lima: %s' % e)



if __name__ == '__main__':
	main()
	
