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

OS_TYPE = platform.system()
if OS_TYPE not in ['Linux', 'Windows']:
	sys.exit('Platform not supported: ' + OS_TYPE)


class Options:

	defaults = {
		'git': False,
		'find-root-path': '',
		'source-prefix': '',
		'config-file': 'scripts/config.txt',
		'build-prefix': 'build',
		'build-type': ('RelWithDebInfo' if OS_TYPE == 'Linux' 
			       else 'Release'),
		'install-prefix': '',
		'install-python-prefix': '',
	}

	cmd_2_cmake_map = [
		('build-type', 'cmake-build-type'),
		('install-prefix', 'cmake-install-prefix'),
		('install-python-prefix', 'python-site-packages-dir'),
		('find-root-path', 'cmake-find-root-path')
	]

	def __init__(self, argv=None):
		self.opts = {}
		self.extra_opts = []
		self.config_opts = None
		self.cmake_opts = None
		self.git = None

		if argv is not None:
			self.decode_args(argv)

	def decode_args(self, argv):
		for arg in argv:
			if arg in ['--help', '-help', '-h', '-?']:
				print_help()
			for opt in self.defaults:
				prefix = '--%s' % opt
				if arg == prefix:
					self.set(opt, True)
					break
				elif arg.startswith(prefix + '='):
					self.set(opt, arg[len(prefix) + 1:])
					break
			else:
				self.add_extra(arg)

		src = self.get('source-prefix')
		if not self.get('source-prefix'):
			src = os.getcwd()
			self.set('source-prefix', src)
		for opt in ['config-file', 'build-prefix']:
			p = self.get(opt)
			if not os.path.isabs(p):
				self.set(opt, os.path.join(src, p))

	def set(self, x, v):
		self.opts[x] = v

	def get(self, x, check_defaults=True):
		if x in self.opts:
			return self.opts[x]
		return self.defaults[x] if check_defaults else ''

	def add_extra(self, x):
		self.extra_opts.append(x)

	def get_git_options(self):
		return self.extra_opts

	def get_cmd_opts(self, check_defaults=False):
		opts = dict(self.defaults) if check_defaults else {}
		opts.update(self.opts)
		for arg in self.extra_opts:
			for p, d in [("limacamera", "camera"), 
				     ("lima-enable", "third-party")]:
				if arg.startswith(d + '/'):
					arg = p + '-' + arg[len(d) + 1:]
			opts[arg] = str(1)
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
				self.config_opts.append((opt, val))

	# return options in config file activated (=1) if passed as arguments,
	# and also those not specified as empty (=) or disabled (=0) in file
	def get_cmake_opts(self):
		cmd_opts = self.get_cmd_opts()
		if self.config_opts is None:
			self.read_config()

		def is_active(v):
			return val and (val != str(0))

		cmake_opts = []
		for opt, val in self.config_opts:
			for cmd_opt, cmd_val in cmd_opts.items():
				if cmd_opt == opt:
					val = cmd_val
					break
				# arg-passed option must match the end
				# of opt a nd must be preceeded by 
				# the '_' separator
				t = opt.split(cmd_opt)
				if ((len(t) == 2) and 
				    (t[0][-1] == '_') and not t[1]):
					val = cmd_val
					break
			if is_active(val):
				cmake_opts.append((opt, val))

		for cmd_key, cmake_key in self.cmd_2_cmake_map:
			val = self.get(cmd_key)
			if is_active(val) and cmake_key not in dict(cmake_opts):
				cmake_opts.append((cmake_key, val))

		return CMakeOptions(self.get('source-prefix'), cmake_opts)

	def get_cmake_cmd_line(self):
		if self.cmake_opts is None:
			self.cmake_opts = self.get_cmake_opts()
		return self.cmake_opts.get_cmd_line_opts()

	@staticmethod
	def print_help():
		with open("INSTALL.txt") as f:
			print(f.read())
			sys.exit()

	@staticmethod
	def to_underscore(x):
		return x.replace('-', '_')

	@staticmethod
	def from_underscore(x):
		return x.replace('_', '-')


class CMakeOptions:

	def __init__(self, source_prefix, opts):
		self.source_prefix = source_prefix
		self.opts = opts

		if OS_TYPE == 'Linux':
			self.cmake_gen = 'Unix Makefiles'
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
				win_compiler = "Visual Studio 14 2015"
			# now check architecture
			if platform.architecture()[0] == '64bit':
				win_compiler += ' Win64' 

			print ('Found Python ', sys.version)
			print ('Used compiler: ', win_compiler)
			self.cmake_gen = win_compiler

	def get_cmd_line_opts(self):
		opts = [self.source_prefix, '-G"%s"' % self.cmake_gen]
		opts += map(self.cmd_opt, self.opts)
		return ' '.join(opts)

	@staticmethod
	def cmd_opt(opt_val):
		o, v = opt_val
		def quoted(x):
			return (('"%s"' % x) 
				if ' ' in x and not x.startswith('"') else x)
		return '-D%s=%s' % (Options.to_underscore(o).upper(), quoted(v))


class GitHelper:

	not_submodules = (
		'git', 'python', 'tests', 'test', 'cbf', 'lz4', 'fits', 'gz', 
		'tiff', 'hdf5'
	)

	camera_list = (
		'adsc', 'andor3', 'basler', 'dexela', 'frelon', 'hexitec', 
		'marccd', 'merlin', 'mythen3', 'perkinelmer', 'pilatus', 
		'pointgrey', 'rayonixhs', 'ultra', 'xh', 'xspress3', 'andor', 
		'aviex', 'eiger', 'hamamatsu', 'imxpad', 'maxipix', 'mythen', 
		'pco', 'photonicscience','pixirad', 'prosilica', 
		'roperscientific', 'ueye', 'v4l2', 'xpad', 'lambda', 
		'slsdetector', 'fli'
	)

	submodule_map = {
		'espia': 'camera/common/espia',
		'pytango-server': 'applications/tango/python',
	}

	basic_submods = (
		'third-party/Processlib',
	)

	def __init__(self, opts):
		self.opts = opts

	def check_submodules(self, submodules=None):
		if submodules is None:
			submodules = self.opts

		submod_list = []
		for submod in submodules:
			if submod in self.not_submodules:
				continue
			if submod in self.submodule_map:
				submod = self.submodule_map[submod]
			if submod in camera_list:
				submod = 'camera/' + submod
			submod_list.append(s)
		for submod in self.basic_submods:
			if submod not in submod_list:
				submod_list.append(submod)

		try:
			for submod in submod_list:
				action = 'init ' + submod
				ret = os.system('git submodule ' + action)
				if re != 0:
					raise Exception('Could not init')
				action = 'update --recursive ' + submod
				ret = os.system('git submodule ' + action)
				if re != 0:
					raise Exception('Could not update')
		except Exception as e:
			sys.exit('Problem with submodule %s: %s' % (action, e))


def go_build(opts):
	build_prefix = opts.get('build-prefix')
	if not os.path.exists(build_prefix):
		os.mkdir(build_prefix)
	os.chdir(build_prefix)


def build_install_lima_linux(opts):
	go_build(opts)
	try:
		cmake_opts = opts.get_cmake_cmd_line()
		print(cmake_opts)
		sys.stdout.flush()

		action = 'configuration'
                ret = os.system('cmake ' + cmake_opts)
                if ret != 0:
                        raise Exception('Something is wrong in your ' +
					'CMake environement. Make sure ' + 
					'your configuration is good.')

		action = 'compilation'
		nb_cpus = multiprocessing.cpu_count()
                ret = os.system('make -j%d' % (nb_cpus + 1))
                if ret != 0:
                        raise Exception('CMake could not build Lima. ' + 
					'Contact lima@esrf.fr for help.')

		action = 'installation'
                ret = os.system('make install')
                if ret != 0 :
                        raise Exception('CMake could not install libraries. ' + 
					'Make sure you have necessary rights.')
	except Exception as e:
		sys.exit('Problem in CMake %s: %s' % (action, e))


def build_install_lima_windows(opts):
	go_build(opts)
	try :
		cmake_opts = opts.get_cmake_cmd_line()
		print(cmake_opts)
		sys.stdout.flush()

		action = 'configuration'
                ret = os.system('cmake ' + cmake_opts)
		if ret != 0:
			raise Exception('Something went wrong in the CMake ' + 
					'preparation. Make sure ' + 
					'your configuration is good.')

		action = 'compilation or installation'
		cmake_compile_opts = ['--build .', '--target install',
				      '--config %s' % opts.get('build-type')]
		cmake_cmd_line_opts = ' '.join(cmake_compile_opts)
		ret = os.system('cmake ' + cmake_cmd_line_opts)
		if ret != 0:
			raise Exception('CMake could not build or install ' + 
					'libraries. Contact lima@esrf.fr ' + 
					'for help.')
	except Exception as e:
		sys.exit('Problem in CMake %s: %s' % (action, e))
			

def main():
	opts = Options(sys.argv[1:])

	# No git option under windows for obvious reasons.
	if OS_TYPE == 'Linux' and opts.get('git'):
		git_opts = opts.get_git_options()
		git = GitHelper(git_opts)
		git.check_submodules()

	if OS_TYPE == 'Linux':
		build_install_lima_linux(opts)
	elif OS_TYPE == 'Windows':
		build_install_lima_windows(opts)


if __name__ == '__main__':
	main()
	
