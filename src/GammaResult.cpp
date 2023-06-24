/********************************************************************************************
 * Copyright (C) 2023 'Yet Another Gamma Index Tool' Developers.
 * 
 * This file is part of 'Yet Another Gamma Index Tool'.
 * 
 * 'Yet Another Gamma Index Tool' is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * 'Yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 'Yet Another Gamma Index Tool'.  If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************************************/

#include "yagit/GammaResult.hpp"

#include <algorithm>
#include <cmath>

namespace yagit{

GammaResult::value_type GammaResult::passingRate() const{
    return static_cast<value_type>(std::count_if(m_data.begin(), m_data.end(), [](value_type el) {
        return !std::isnan(el) && el <= 1;
    })) / nansize();
}

GammaResult::value_type GammaResult::minGamma() const{
    return nanmin();
}

GammaResult::value_type GammaResult::maxGamma() const{
    return nanmax();
}

double GammaResult::sumGamma() const{
    return nansum();
}

double GammaResult::meanGamma() const{
    return nanmean();
}

double GammaResult::varGamma() const{
    return nanvar();
}

}
