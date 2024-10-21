import os
from setuptools import setup, find_packages
from glob import glob


# Define the package directory
package_dir = './ContainerPy'

# Collect all necessary files from the package directory
package_data_files = glob(f"{package_dir}/*")

# Install script
setup(
    name="ContainerPy",
    version="0.0.1",
    author="Ahmed Aredah",
    author_email="AhmedAredah@vt.edu",
    description=("Modeling Containers on CargoNetSim simulator."),
    license="View License on GitHub: 'https://github.com/AhmedAredah/container'",
    keywords="Container library for CargoNetSim simulator",
    packages=['ContainerPy'],
    package_dir={'ContainerPy': package_dir},
    package_data={'ContainerPy': [os.path.basename(f) for f in package_data_files]},
    classifiers=[
        "Development Status :: 0.0.1 - Alpha",
        "Topic :: Utilities",
        "License :: Review License on GitHub",
    ],
    include_package_data=True,
)
