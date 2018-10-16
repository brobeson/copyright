#!/bin/bash

set -v

# Lizard doesn't properly scan Python files without the extension. So, create a copy
# with the extension, and scan that.
cp application/copyright application/copyright.py && \
    lizard --CCN=10 --length=20 --arguments=6 --warnings_only --modified application/copyright.py && \
    rm application/copyright.py
