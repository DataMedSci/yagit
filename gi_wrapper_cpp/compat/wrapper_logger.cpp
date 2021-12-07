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

#include "wrapper_logger.h"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

const string FILENAME = "log.txt";

string createWrapperTimestamp()
{
    using namespace std::chrono;

    const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    const std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    return std::ctime(&now_time);
}

void enableWrapperLogging()
{
	WRAPPER_LOGGER_ON = true;
}

void disableWrapperLogging()
{
	WRAPPER_LOGGER_ON = false;
}

void initializeWrapperLogger(bool clearLogs)
{
	ofstream logger;

	if (clearLogs)
	{
		logger.open(FILENAME.c_str(), ios::out | ios::trunc);
	}
	else
	{
		logger.open(FILENAME.c_str(), ios::out | ios::app);
	}

	logger << "GAMMA INDEX LOG" << endl;
	logger.close();
	enableWrapperLogging();
}

void initializeWrapperLogger()
{
	initializeWrapperLogger(false);
}

void logWrapperMessage(string message)
{
	if (WRAPPER_LOGGER_ON)
	{
		ofstream logger;
		logger.open(FILENAME.c_str(), ios::out | ios::app);
		string entry;
		entry += createWrapperTimestamp();
		entry += " : ";
		entry += message;
		logger << entry << endl;
		logger.close();
	}
}
