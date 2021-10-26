#!/bin/bash

# This file is part of 'yet Another Gamma Index Tool'.
#
# 'yet Another Gamma Index Tool' is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# 'yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with 'yet Another Gamma Index Tool'; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

echo "Installing wxWidgets"
sudo apt-get -y install libwxgtk3.0-gtk3-dev

if [ -d "imebra" ]; then
  echo 'Imebra is already installed, skip building Imebra'
else
  echo "Building imebra"
  mkdir imebra
  cd imebra
  wget https://github.com/binarno/imebra_mirror/archive/refs/heads/4.0.8.zip
  unzip *.zip
  cd imebra_mirror-4.0.8/
  cp -a . ..
  cd ..
  mkdir artifacts
  cd artifacts
  cmake ../library -DIMEBRA_SHARED_STATIC=STATIC
  cmake --build .
  cd ..
  cd ..
fi

echo "Starting build"
echo "Building core module"
cd gi_core
make
cd ..
echo "Building additions module"
cd gi_additions
make
cd ..
echo "Building wrapper module"
cd gi_wrapper_cpp
make
#cd ..
#cd examples
#echo "Building gi_core_2d_client"
#make build_core_2d_client
#echo "Building gi_wrapper_client"
#make build_wrapper_client
cd ..
echo "Building GUI module"
cd gi_gui
make
cd ..
echo "Finish building"
echo "Trying to run GUI"
cd gi_gui
./yAGIT
