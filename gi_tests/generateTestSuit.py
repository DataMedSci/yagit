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

import sys
import os
import re

if( not (len(sys.argv) == 5 or len(sys.argv) == 6)):
    print("Wrong number of arguments. Apropriate are: ")
    print("\t- 1 - RELATIVE path to directory with configs")
    print("\t- 2 - UNIQUE name for test-suit (uniqueness won't be checked)")
    print("\t- 3 & 4 - RELATIVE paths for correspondingly ref and target images")
    print("\t- 5 - RELATIVE path to result image (OPTIONAL)")
    sys.exit(1)

dims = '2'
dds = '1.0|2.0|3.0|4.0|5.0'
dtas = '1.0|2.0|3.0|4.0|5.0'
locs = 'true|false'
lims = '2.0|20.0'
comps = '1to1|gpr'
epses = '0.1|0.05|0.01|0.005'


pattern = re.compile("dim(" + dims + ")dd(" + dds + ")dta(" + dtas + ")loc(" + locs + ")lim(" + lims + ")comp(" + comps + ")eps(" + epses + ")")

prefix = "./suits/test-cases/"
testSuitePath = "./suits/test-suits/" + sys.argv[2] + ".txt"
testSuitePaths = []

for config in os.listdir(sys.argv[1]):
    configPath = sys.argv[1] + config
    testCaseName = config
    if(pattern.match(testCaseName) != None):
        if not os.path.exists(prefix + sys.argv[2]):
            os.makedirs(prefix + sys.argv[2])

        testCasePath = prefix + sys.argv[2] + "/" + testCaseName

        testCase = open(testCasePath, "w")
        content = sys.argv[3] + "\n" + sys.argv[4] + "\n" + configPath + "\n"
        if(len(sys.argv) == 6):
            content = content + sys.argv[5] + "\n"
        testCase.write(content)
        testCase.close()

        testSuitePaths.append(testCasePath)

if not os.path.exists("./suits/test-suits/"):
    os.makedirs("./suits/test-suits/")

suiteFile = open(testSuitePath, "w")
for case in testSuitePaths:
    suiteFile.write(case + "\n")

suiteFile.close()

print("Saving in: " + testSuitePath)
