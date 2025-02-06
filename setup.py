import os
import sys
import platform
import subprocess
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from setuptools.command.develop import develop

class CMakeExtension(Extension):
	"""Custom extension class for CMake-built modules"""
	def __init__(self, name, sourcedir=""):
		Extension.__init__(self, name, sources=[])
		self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
	"""Custom build command for CMake-built modules"""
	
	def get_cmake_version(self):
		try:
			out = subprocess.check_output(['cmake', '--version'])
			return tuple(map(int, out.decode().split('\n')[0].split(' ')[2].split('.')))
		except:
			return None
			
	def run(self):
		# Check CMake version
		cmake_version = self.get_cmake_version()
		if not cmake_version or cmake_version < (3, 24):
			raise RuntimeError("CMake >= 3.24.0 is required")

		# Get Qt path from environment or use default
		qt_path = os.getenv('QT_CMAKE_PATH', '/home/ahmed/Qt/6.8.0/gcc_64/lib/cmake')
		qt_tools = os.getenv('QT_TOOLS_PATH', '/home/ahmed/Qt/Tools/CMake/bin')

		if not os.path.exists(qt_path):
			raise RuntimeError(f"Qt CMake path not found at {qt_path}. Set QT_CMAKE_PATH environment variable to correct path")
			
		if not os.path.exists(qt_tools):
			raise RuntimeError(f"Qt tools path not found at {qt_tools}. Set QT_TOOLS_PATH environment variable to correct path")

		# Set environment variables
		os.environ["CMAKE_PREFIX_PATH"] = qt_path
		os.environ["PATH"] = f"{qt_tools}:{os.environ['PATH']}"

		# Check if Qt is installed - try multiple possible qmake names 
		qmake_names = ['qmake6', 'qmake-qt6', 'qmake']
		qt_found = False
		for qmake in qmake_names:
			try:
				subprocess.check_call([qmake, '--version'])
				qt_found = True
				break
			except:
				continue

		if not qt_found:
			raise RuntimeError("Qt6 is required but not found")

		for ext in self.extensions:
			self.build_extension(ext)
			
	def build_extension(self, ext):
		extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
		debug = int(os.environ.get("DEBUG", 0)) if self.debug is None else self.debug
		cfg = 'Debug' if debug else 'Release'
		
		cmake_build_dir = os.path.join(ext.sourcedir, "build")
		os.makedirs(cmake_build_dir, exist_ok=True)

		# Ensure the output will be in the proper directory for the wheel
		cmake_args = [
			f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={os.path.join(extdir, 'containerpy')}",
			f"-DPYTHON_EXECUTABLE={sys.executable}",
			f"-DCMAKE_BUILD_TYPE={cfg}",
			"-DBUILD_SHARED_LIBS=ON",
			"-DBUILD_PYTHON_BINDINGS=ON",
			"-DBUILD_TESTING=OFF",
		]

		# Check if Ninja is available
		try:
			subprocess.check_call(['ninja', '--version'])
			cmake_args += ["-GNinja"]
		except (subprocess.CalledProcessError, FileNotFoundError):
			# Fallback to default generator if Ninja is not available
			pass

		# Platform-specific configurations
		if platform.system() == "Windows":
			cmake_args += [
				"-DCMAKE_C_COMPILER=cl",
				"-DCMAKE_CXX_COMPILER=cl",
			]
			if sys.maxsize > 2**32:
				cmake_args += ["-A", "x64"]
		else:
			cmake_args += [
				"-DCMAKE_C_COMPILER=gcc",
				"-DCMAKE_CXX_COMPILER=g++",
			]

		# Remove CMakeCache.txt if it exists
		cache_file = os.path.join(cmake_build_dir, "CMakeCache.txt")
		if os.path.exists(cache_file):
			os.remove(cache_file)

		subprocess.check_call(
			["cmake", ext.sourcedir] + cmake_args, 
			cwd=cmake_build_dir
		)

		build_args = ["--config", cfg]
		subprocess.check_call(
			["cmake", "--build", "."] + build_args,
			cwd=cmake_build_dir
		)
  
class InstallPlatlib(install):
	def finalize_options(self):
		install.finalize_options(self)
		if self.distribution.has_ext_modules():
			self.install_lib = self.install_platlib

class DevelopPlatlib(develop):
	def finalize_options(self):
		develop.finalize_options(self)
		if self.distribution.has_ext_modules():
			self.install_lib = self.install_platlib

def get_long_description():
	with open("README.md", "r", encoding="utf-8") as fh:
		return fh.read()

setup(
	# Basic package information
	name="containerpy",
	version="0.1.0",
	author="Ahmed Aredah",
	author_email="AhmedAredah@vt.edu",
	description="Container management library with Python bindings",
	long_description=get_long_description(),
	long_description_content_type="text/markdown",
	url="https://github.com/AhmedAredah/container",
	
	# Package configuration
	packages=find_packages(where="python"),
	package_dir={"": "python"},
	package_data={
		"containerpy": ["*.pyi", "py.typed", "*.so", "*.pyd", "*.dylib"],
	},
	
	# Python requirements
	python_requires=">=3.8",

	# Build configuration
	ext_modules=[CMakeExtension("containerpy")],
	cmdclass={
		"build_ext": CMakeBuild,
		"install": InstallPlatlib,
		"develop": DevelopPlatlib,
	},
	
	# Classifiers for PyPI
	classifiers=[
		"Development Status :: 4 - Beta",
		"Intended Audience :: Developers",
		"Topic :: Software Development :: Libraries :: Python Modules",
		"License :: OSI Approved :: MIT License",  # Adjust as needed
		"Programming Language :: Python :: 3",
		"Programming Language :: Python :: 3.8",
		"Programming Language :: Python :: 3.9",
		"Programming Language :: Python :: 3.10",
		"Programming Language :: Python :: 3.11",
		"Programming Language :: C++",
		"Operating System :: OS Independent",
		"Environment :: Console",
	],
	
	# Additional metadata
	keywords="container management, simulation, logistics",
	project_urls={
		"System Requirements": "Qt6 development files, CMake 3.24+",
		"Bug Reports": "https://github.com/AhmedAredah/container/issues",
		"Source": "https://github.com/AhmedAredah/container",
		"Documentation": "https://container.readthedocs.io/",
	},
	
	# Entry points (if needed)
	entry_points={
		"console_scripts": [
			# "containerpy-cli=containerpy.cli:main",
		],
	},
	
	# Additional options
	zip_safe=False,  # Required for C extensions
	include_package_data=True,
)