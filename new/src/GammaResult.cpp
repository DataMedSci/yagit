/******************************************************************************************
 * This file is part of 'yet Another Gamma Index Tool'.
 *
 * 'yet Another Gamma Index Tool' is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * 'yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 'yet Another Gamma Index Tool'; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 ******************************************************************************************/

#include "GammaResult.hpp"

#include <algorithm>
#include <cmath>

namespace yagit{

GammaResult::value_type GammaResult::passingRate() const{
    return static_cast<value_type>(std::count_if(m_data.begin(), m_data.end(), [](const auto& el) {
        return !std::isnan(el) && el <= 1;
    })) / nansize();
}

GammaResult::value_type GammaResult::minGamma() const{
    return nanmin();
}

GammaResult::value_type GammaResult::maxGamma() const{
    return nanmax();
}

GammaResult::value_type GammaResult::sumGamma() const{
    return nansum();
}

GammaResult::value_type GammaResult::meanGamma() const{
    return nanmean();
}

GammaResult::value_type GammaResult::varGamma() const{
    return nanvar();
}

}
