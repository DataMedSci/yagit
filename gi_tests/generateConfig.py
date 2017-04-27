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

dimentionPool = ['1', '2', '3']
ddPool = ['1.0', '2.0', '3.0', '4.0', '5.0']
dtaPool = ['1.0', '2.0', '3.0', '4.0', '5.0']
localPool = ['false', 'true']
refvalPool = ['-1']
limitPool = ['2.0', '20.0']
planePool = ['-1']
refslicePool = ['-1']
tarslicePool = ['-1']
comparatorPool = ['onetoonecomparator', 'gpr']
epsilonPool = ['0.1', '0.05', '0.01', '0.005']

comparatorSignature = {'onetoonecomparator' : '1to1', 'gpr':'gpr'}
prefix = "./suits/test-data/configs/2d-automated/"

if not os.path.exists(prefix):
    os.makedirs(prefix)

for dimention in dimentionPool:
    for dd in ddPool:
        for dta in dtaPool:
            for local in localPool:
                for refval in refvalPool:
                    for limit in limitPool:
                        for plane in planePool:
                            for refslice in refslicePool:
                                for tarslice in tarslicePool:
                                    for comparator in comparatorPool:
                                        for epsilon in epsilonPool:
                                            content = "" + \
                                            "noDimentions:" + dimention + "\n" + \
                                            "perc:" + dd + "\n" + \
                                            "dta:" + dta + "\n" + \
                                            "local:" + local + "\n" + \
                                            "refval:" + refval + "\n" + \
                                            "limit:" + limit + "\n" + \
                                            "plane:" + plane + "\n" + \
                                            "refslice:" + refslice + "\n" + \
                                            "tarslice:" + tarslice + "\n" + \
                                            "comparator:" + comparator + "\n" + \
                                            "epsilon:" + epsilon + "\n"

                                            path = prefix + "dim" + dimention + "dd" + dd + "dta" + dta + "loc" + \
                                            local + "lim" + limit + "comp" + comparatorSignature[comparator] + \
                                            "eps" + epsilon + ".txt"

                                            config = open(path, "w")
                                            config.write(content)
                                            config.close()
