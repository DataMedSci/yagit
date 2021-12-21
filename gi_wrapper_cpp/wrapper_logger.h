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

#ifndef WRAPPER_LOGGER_H
#define WRAPPER_LOGGER_H

#include<cstddef>
#include<string>


static bool WRAPPER_LOGGER_ON = false;

/// \brief Enables wrapper logging service.
///
///////////////////////////////////////////////////////////////////////////////
void enableWrapperLogging();

/// \brief Disables wrapper logging service.
///
///////////////////////////////////////////////////////////////////////////////
void disableWrapperLogging();

/// \brief Initializes wrapper logger service.
///
/// \param clearLogs Indicates whether logs from previous calculations should be cleared.
///
///////////////////////////////////////////////////////////////////////////////
void initializeWrapperLogger(bool clearLogs);

/// \brief Initializes wrapper logger service.
///
///////////////////////////////////////////////////////////////////////////////
void initializeWrapperLogger();

/// \brief Puts an information in a log file.
///
/// \note initializeWrapperLogger() should be called beforehand.
///
/// \param message Text that should be put in a log file (timestamp will be included automatically).
///
///////////////////////////////////////////////////////////////////////////////
void logWrapperMessage(std::string message);

#endif //WRAPPER_LOGGER_H