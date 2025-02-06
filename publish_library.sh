#!/bin/bash

# Build wheel
python -m build

# Build conda package
conda build conda/

# Publish to PyPI
twine upload dist/*

# Publish to Conda
anaconda upload /path/to/conda/package.tar.bz2