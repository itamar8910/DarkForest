#!/bin/bash

git clone git@github.com:ozkl/doomgeneric.git
cd doomgeneric
git apply ../doom.patch
cd doomgeneric
make
