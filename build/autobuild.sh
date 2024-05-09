#!/bin/bash
set -e
cd .. &&
rm -rf CMakeFiles/ &&
cmake . &&
make