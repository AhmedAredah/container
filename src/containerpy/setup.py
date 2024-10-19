from setuptools import setup, find_packages
import sys
import platform
import os
import sysconfig

# Get the path to the Python site-packages directory
site_packages_dir = sysconfig.get_paths()['purelib']

# Function to determine the platform-specific shared library extension
def get_shared_lib_extension():
    if platform.system() == "Windows":
        return ".pyd"
    elif platform.system() == "Darwin":  # macOS
        return ".so"
    else:  # Linux or other Unix-like
        return ".so"

# Detect Python version and architecture to construct the filename
python_version = f"{sys.version_info.major}{sys.version_info.minor}"
architecture = platform.architecture()[0]
shared_lib_ext = get_shared_lib_extension()

# Construct the filename dynamically
shared_lib_name = f"ContainerPy.cpython-{python_version}-x86_64-linux-gnu{shared_lib_ext}"

# Update for Windows and macOS naming conventions if needed
if platform.system() == "Windows":
    shared_lib_name = f"ContainerPy.cp{python_version}-{architecture}.pyd"
elif platform.system() == "Darwin":
    shared_lib_name = f"ContainerPy.cpython-{python_version}-darwin{shared_lib_ext}"

# Full path to the shared library, assuming it will be installed to site-packages
shared_lib_path = os.path.join(site_packages_dir, "ContainerPy", shared_lib_name)

# Check if the expected shared library exists (for better error handling)
if not os.path.exists(shared_lib_path):
    print(f"Warning: Expected shared library '{shared_lib_path}' not found. Please ensure it is correctly built.")
    sys.exit(1)

# Define additional dynamic libraries for each platform with full paths to site-packages
if platform.system() == "Windows":
    # Windows libraries
    extra_dynamic_libs = [
        os.path.join(site_packages_dir, "ContainerPy", "Qt6Xml.dll"),
        os.path.join(site_packages_dir, "ContainerPy", "Qt6Sql.dll"),
        os.path.join(site_packages_dir, "ContainerPy", "Qt6Network.dll"),
        os.path.join(site_packages_dir, "ContainerPy", "Qt6Core.dll"),
        os.path.join(site_packages_dir, "ContainerPy", "Container.dll")
    ]
elif platform.system() == "Darwin":
    # macOS libraries
    extra_dynamic_libs = [
        os.path.join(site_packages_dir, "ContainerPy", "libQt6Xml.6.dylib"),
        os.path.join(site_packages_dir, "ContainerPy", "libQt6Sql.6.dylib"),
        os.path.join(site_packages_dir, "ContainerPy", "libQt6Network.6.dylib"),
        os.path.join(site_packages_dir, "ContainerPy", "libQt6Core.6.dylib"),
        os.path.join(site_packages_dir, "ContainerPy", "libContainer.dylib")
    ]
else:  # Linux/Unix
    # Linux libraries
    extra_dynamic_libs = [
        os.path.join(site_packages_dir, "ContainerPy", "libQt6Xml.so.6"),
        os.path.join(site_packages_dir, "ContainerPy", "libQt6Sql.so.6"),
        os.path.join(site_packages_dir, "ContainerPy", "libQt6Network.so.6"),
        os.path.join(site_packages_dir, "ContainerPy", "libQt6Core.so.6"),
        os.path.join(site_packages_dir, "ContainerPy", "libContainer.so")
    ]

# Ensure that __init__.py exists in the site-packages directory
init_file_path = os.path.join(site_packages_dir, "ContainerPy", "__init__.py")
if not os.path.exists(init_file_path):
    with open(init_file_path, "w") as f:
        f.write("# Import the main shared library\n")
        f.write("from .ContainerPy import *\n")

# Include the main shared library and any additional dynamic libraries
setup(
    name='ContainerPy',
    version='1.0',
    packages=find_packages(),  # Automatically find packages
    package_dir={'ContainerPy': '.'},
    package_data={'ContainerPy': [shared_lib_path] + extra_dynamic_libs},
)
