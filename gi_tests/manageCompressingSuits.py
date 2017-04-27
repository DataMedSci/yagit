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

import os
import zipfile
import sys

def compress():
    suits = zipfile.ZipFile('suits.zip', 'w', zipfile.ZIP_DEFLATED)
    suitsPath = "./suits/"

    for root, dirs, files in os.walk(suitsPath):
        for file in files:
            suits.write(os.path.join(root, file))

    suits.close()

def decompress():
    suits = zipfile.ZipFile('suits.zip', 'r')
    suits.extractall(".")
    suits.close()

if(len(sys.argv) != 2):
    print("Wrong number of arguments: 'compress' or 'decompress'")
else:
    if(sys.argv[1] == 'compress'):
        compress()
    elif(sys.argv[1] == 'decompress'):
        decompress()
