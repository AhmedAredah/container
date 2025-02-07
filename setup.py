import os
import sys
import platform
import subprocess
import glob
import shutil
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
			version_str = out.decode().split('\n')[0].split(' ')[2]
			# Strip any suffix (rc1, beta, etc) keeping only digits and dots
			version_str = ''.join(c for c in version_str if c.isdigit() or c == '.')
			return tuple(map(int, version_str.split('.')))
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
		final_lib_dir = os.path.join(extdir, 'containerpy')
		
		print(f"CMake build directory: {cmake_build_dir}")
		print(f"Final library directory: {final_lib_dir}")
		
		os.makedirs(cmake_build_dir, exist_ok=True)
		os.makedirs(final_lib_dir, exist_ok=True)
  
		# Get generator and architecture from environment variables
		generator = os.getenv('CMAKE_GENERATOR', '') # e.g., 'Ninja'. 
		architecture = os.getenv('CMAKE_ARCHITECTURE', '')  # e.g., 'x64' or 'Win32'


		cmake_args = [
			f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={final_lib_dir}",
			f"-DPYTHON_EXECUTABLE={sys.executable}",
			f"-DCMAKE_BUILD_TYPE={cfg}",
			"-DBUILD_SHARED_LIBS=ON",
			"-DBUILD_PYTHON_BINDINGS=ON",
			"-DBUILD_TESTING=OFF",
		]
  
		# Set generator
		if generator:
			cmake_args.extend(["-G", generator])

		# Set architecture for Windows
		if platform.system() == "Windows" and architecture:
			cmake_args.extend(["-A", architecture])

		# Set RPATH settings for non-Windows platforms
		if platform.system() != "Windows":
			cmake_args += [
				"-DCMAKE_INSTALL_RPATH=$ORIGIN",
				"-DCMAKE_BUILD_WITH_INSTALL_RPATH=ON",
				"-DCMAKE_SKIP_BUILD_RPATH=OFF",
			]

		if platform.system() == "Windows":
			if generator == "Ninja":
				cmake_args += ["-GNinja", "-DCMAKE_C_COMPILER=cl", "-DCMAKE_CXX_COMPILER=cl"]
		else:
			if generator == "Ninja":
				cmake_args += ["-GNinja", "-DCMAKE_C_COMPILER=gcc", "-DCMAKE_CXX_COMPILER=g++"]

		build_args = ["--config", cfg]

		subprocess.check_call(["cmake", ext.sourcedir] + cmake_args, cwd=cmake_build_dir)
		subprocess.check_call(["cmake", "--build", "."] + build_args, cwd=cmake_build_dir)

		# Copy libraries
		lib_ext = '.dll' if platform.system() == 'Windows' else '.so' if platform.system() == 'Linux' else '.dylib'
		source_dir = os.path.join(cmake_build_dir, 'lib')  # CMake output directory
		
		print(f"Looking for libraries in: {source_dir}")
		
		if platform.system() == 'Windows':
			source_dirs = [
				os.path.join(cmake_build_dir, 'Release'),
				os.path.join(cmake_build_dir, 'lib', 'Release'),
				os.path.join(cmake_build_dir, 'Debug'),
				os.path.join(cmake_build_dir, 'lib', 'Debug')
			]
   
			# Add Qt DLL directory
			qt_bin_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.getenv('QT_CMAKE_PATH')))), 'bin')
   
			# Required Qt DLLs
			qt_dlls = ['Qt6Core.dll', 'Qt6Sql.dll']
			
			# Copy Qt DLLs
			if os.path.exists(qt_bin_dir):
				for dll in qt_dlls:
					qt_dll_path = os.path.join(qt_bin_dir, dll)
					if os.path.exists(qt_dll_path):
						dest_file = os.path.join(final_lib_dir, dll)
						print(f"Copying Qt DLL: {qt_dll_path} to {dest_file}")
						shutil.copy2(qt_dll_path, dest_file)
					else:
						print(f"Warning: Could not find {dll} in {qt_bin_dir}")

			for src_dir in source_dirs:
				if not os.path.exists(src_dir):
					continue
					
				for pattern in ['*.dll', '*.pyd']:
					for lib_file in glob.glob(os.path.join(src_dir, pattern)):
						dest_file = os.path.join(final_lib_dir, os.path.basename(lib_file))
						print(f"Copying {lib_file} to {dest_file}")
						shutil.copy2(lib_file, dest_file)
		else:
			for lib_pattern in [
				'libContainer*',
				f'ContainerPy.cpython-{sys.version_info.major}{sys.version_info.minor}*'
			]:
				pattern = os.path.join(source_dir, f'{lib_pattern}{lib_ext}')
				print(f"Searching with pattern: {pattern}")
				for lib_file in glob.glob(pattern):
					dest_file = os.path.join(final_lib_dir, os.path.basename(lib_file))
					
					# Skip if source and destination are identical
					try:
						if os.path.exists(dest_file) and os.path.samefile(lib_file, dest_file):
							print(f"Skipping copy as source and destination are identical: {lib_file}")
							continue
					except FileNotFoundError:
						pass
						
					print(f"Copying {lib_file} to {dest_file}")
					shutil.copy2(lib_file, dest_file)
					
					if platform.system() == 'Linux':
						try:
							# Get actual Python lib path from the target environment
							python_lib = os.path.join(os.path.dirname(os.path.dirname(sys.executable)), 'lib')
							rpath = f"$ORIGIN:{python_lib}"
							subprocess.check_call(['patchelf', '--set-rpath', rpath, dest_file])
							print(f"Set RPATH for {dest_file} to {rpath}")
							
							# Verify RPATH
							rpath_check = subprocess.check_output(['patchelf', '--print-rpath', dest_file]).decode().strip()
							print(f"Verified RPATH: {rpath_check}")
						except Exception as e:
							print(f"Warning: Could not set RPATH for {dest_file}: {e}")
  
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
	license="AGPL-V3", 
	
	# Package configuration
	packages=find_packages(where="python"),
	package_dir={"": "python"},
	package_data={
		"containerpy": ["*.pyi", "py.typed", "*.so", "*.dll", "*.pyd", "*.dylib"],
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
		"License :: OSI Approved :: GNU Affero General Public License v3",
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