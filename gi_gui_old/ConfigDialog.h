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

#include <iostream>
#include <wx/wxprec.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include <wx/gbsizer.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <set>
#include <iostream>
#include <fstream>
#include <wx/listctrl.h>
#include "core_logger.h"

class MainFrame;

class ConfigDialog : public wxDialog
{
public:
  ConfigDialog(MainFrame* parent, const wxString& title);
  void OnNewConfig(wxCommandEvent& event);
  void OnSaveConfig(wxCommandEvent& event);
  void OnLoadConfig(wxCommandEvent& event);
  void OnDeleteConfig(wxCommandEvent& event);
  wxDECLARE_EVENT_TABLE();
private:
  MainFrame* parent_;
  wxListCtrl* list;
  wxString getSelectedConfigName();
  void saveCurrentConfigToFile(std::string name);
  wxString transformIfEmptyString(wxString value);
  void generateWarningDialog(std::string message);
  void InjectConfig(std::string path);
  wxString getDefaultIfEmpty(wxTextCtrl* input, std::string def);
};
