/** @file */

/*********************************************************************************************************************
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
 *********************************************************************************************************************/

#ifndef CORE_LOGGER_H
#define CORE_LOGGER_H

#include<cstddef>
#include<string>


static bool CORE_LOGGER_ON = false;

/// \brief Enables core logging service.
///
///////////////////////////////////////////////////////////////////////////////
void enableCoreLogging();

/// \brief Disables core logging service.
///
///////////////////////////////////////////////////////////////////////////////
void disableCoreLogging();

/// \brief Initializes core logger service.
///
/// \param clearLogs Indicates whether logs from previous calculations should be cleared.
///
///////////////////////////////////////////////////////////////////////////////
void initializeCoreLogger(bool clearLogs);

/// \brief Initializes core logger service.
///
///////////////////////////////////////////////////////////////////////////////
void initializeCoreLogger();

/// \brief Puts an information in a log file.
///
/// \note initializeCoreLogger() should be called beforehand.
///
/// \param message Text that should be put in a log file (timestamp will be included automatically).
///
///////////////////////////////////////////////////////////////////////////////
void logCoreMessage(std::string message);

/// \brief Converts double to string.
///
/// \param value Value that needs to be converted to string.
///
/// \return Value converted to string.
///
///////////////////////////////////////////////////////////////////////////////
std::string toString(double value);

/// \brief Converts plane number to string.
///
/// \param value Value that needs to be converted to string.
///
/// \return Value converted to string.
///
///////////////////////////////////////////////////////////////////////////////
std::string planeToString(int value);

/// \brief Converts algorithm number to string.
///
/// \param value Value that needs to be converted to string.
///
/// \return Value converted to string.
///
///////////////////////////////////////////////////////////////////////////////
std::string algorithmToString(int value);

#endif //CORE_LOGGER_H
