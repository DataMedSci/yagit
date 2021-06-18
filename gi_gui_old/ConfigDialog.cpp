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

#include "ConfigDialog.h"
#include "MainFrame.h"


ConfigDialog::ConfigDialog(MainFrame* parent, const wxString& title)
  : wxDialog(NULL, -1, title), parent_(parent)
{
  logCoreMessage("Initializing configuration management ...");
  wxGridBagSizer* mainSizer = new wxGridBagSizer();

  list = new wxListCtrl(this, -1, wxDefaultPosition, wxSize(250, 350), wxLC_REPORT|wxLC_SINGLE_SEL|wxALL);

  list->InsertColumn(0, _("Config Name"));
  list->SetColumnWidth(0, 250);

  string confsPath = "./config";

  ifstream confs;
  confs.open(confsPath);

  string boofor;

  if(confs.is_open())
  {
    while(!confs.eof())
    {
      getline(confs, boofor);
      if(boofor.length() != 0)
      {
        list->InsertItem(0, wxString(boofor));
      }
    }
  }

  mainSizer->Add(list, wxGBPosition(0, 0), wxGBSpan(7, 2), wxGROW | wxALL, 15);
  mainSizer->Add(new wxButton(this, ID_NewConfig, "New Config", wxDefaultPosition, wxSize(100, 50)), wxGBPosition(0, 3), wxDefaultSpan, wxRIGHT | wxTOP, 15);
  mainSizer->Add(new wxButton(this, ID_SaveConfig, "Save Config", wxDefaultPosition, wxSize(100, 50)), wxGBPosition(1, 3), wxDefaultSpan, wxRIGHT, 15);
  mainSizer->Add(new wxButton(this, ID_LoadConfig, "Load Config", wxDefaultPosition, wxSize(100, 50)), wxGBPosition(2, 3), wxDefaultSpan, wxRIGHT, 15);
  mainSizer->Add(new wxButton(this, ID_DeleteConfig, "Delete Config", wxDefaultPosition, wxSize(100, 50)), wxGBPosition(3, 3), wxDefaultSpan, wxRIGHT, 15);

  for(int i=0; i<4; i++)
    mainSizer->AddGrowableCol(i);

  for(int i=0; i<7; i++)
    mainSizer->AddGrowableRow(i);

  SetSizer(mainSizer);

  Centre();
  ShowModal();
  Destroy();
  logCoreMessage("Configuration management has been closed");
}

wxBEGIN_EVENT_TABLE(ConfigDialog, wxDialog)
  EVT_BUTTON(ID_NewConfig, ConfigDialog::OnNewConfig)
  EVT_BUTTON(ID_SaveConfig, ConfigDialog::OnSaveConfig)
  EVT_BUTTON(ID_LoadConfig, ConfigDialog::OnLoadConfig)
  EVT_BUTTON(ID_DeleteConfig, ConfigDialog::OnDeleteConfig)
wxEND_EVENT_TABLE()

void ConfigDialog::OnNewConfig(wxCommandEvent& event)
{
  logCoreMessage("Saving new config ...");
  wxTextEntryDialog* entry = new wxTextEntryDialog(this, "Enter config name");
  if(entry->ShowModal() == wxID_CANCEL)
  {
    return;
  }

  if(entry->GetValue() == wxEmptyString)
  {
    generateWarningDialog("You must enter config name.");
    return;
  }

  string path = "./configs/" + entry->GetValue().ToStdString() + ".config";
  saveCurrentConfigToFile(path);

  list->InsertItem(0, entry->GetValue());

  string confsPath = "./config";

  std::ofstream configs;
  configs.open(confsPath, std::ios_base::app);
  configs << entry->GetValue().ToStdString() << std::endl;
  configs.close();
  logCoreMessage("New config has been saved under the '" + entry->GetValue().ToStdString() + "' name");
}

void ConfigDialog::OnSaveConfig(wxCommandEvent& event)
{
  wxString pick = getSelectedConfigName();
  if(pick == wxEmptyString)
  {
    generateWarningDialog("You must select config to save on it.");
    return;
  }

  string path = "./configs/" + pick.ToStdString() + ".config";
  saveCurrentConfigToFile(path);
  logCoreMessage("Saved '" + pick.ToStdString() + "' config");
}

void ConfigDialog::generateWarningDialog(string message)
{
  wxMessageDialog *dialog = new wxMessageDialog(NULL,
    wxString(message), wxT("Exclamation"),
    wxOK | wxICON_EXCLAMATION);
  dialog->ShowModal();
}

void ConfigDialog::OnLoadConfig(wxCommandEvent& event)
{
  wxString pick = getSelectedConfigName();
  if(pick == wxEmptyString)
  {
    generateWarningDialog("You must select config to load.");
    return;
  }
  InjectConfig("./configs/" + pick.ToStdString() + ".config");
  logCoreMessage("Loaded '" + pick.ToStdString() + "' config");
}

void ConfigDialog::InjectConfig(string path)
{
  ifstream inputFile;
  inputFile.open(path);

  string boofor;

  if(inputFile.is_open())
  {
    getline(inputFile, boofor);
    parent_->setSomeTextCtrl(parent_->dd, boofor);

    getline(inputFile, boofor);
    parent_->setSomeTextCtrl(parent_->dta, boofor);

    getline(inputFile, boofor);
    parent_->local->SetValue(wxString(boofor));

    getline(inputFile, boofor);
    if(boofor == "wxEmptyString")
      boofor = "";
    parent_->setSomeTextCtrl(parent_->refval, boofor);

    getline(inputFile, boofor);
    parent_->setSomeTextCtrl(parent_->limit, boofor);

    getline(inputFile, boofor);
    parent_->rescale->SetValue(wxString(boofor));

    getline(inputFile, boofor);
    parent_->dims->SetValue(wxString(boofor));

    getline(inputFile, boofor);
    if(boofor == "wxEmptyString")
      boofor = "";
    parent_->plane->SetValue(wxString(boofor));

    getline(inputFile, boofor);
    if(boofor == "wxEmptyString")
      boofor = "";
    parent_->setSomeTextCtrl(parent_->refSlice, boofor);

    getline(inputFile, boofor);
    if(boofor == "wxEmptyString")
      boofor = "";
    parent_->setSomeTextCtrl(parent_->tarSlice, boofor);

    getline(inputFile, boofor);
    parent_->setSomeTextCtrl(parent_->refA, boofor);

    getline(inputFile, boofor);
    parent_->setSomeTextCtrl(parent_->refB, boofor);

    getline(inputFile, boofor);
    parent_->setSomeTextCtrl(parent_->tarA, boofor);

    getline(inputFile, boofor);
    parent_->setSomeTextCtrl(parent_->tarB, boofor);

    getline(inputFile, boofor);
    if(boofor == "wxEmptyString")
      boofor = "";
    parent_->setSomeTextCtrl(parent_->filter, boofor);

    getline(inputFile, boofor);
    parent_->saveModes->SetValue(wxString(boofor));
  }
}


void ConfigDialog::OnDeleteConfig(wxCommandEvent& event)
{
  wxString pick = getSelectedConfigName();
  if(pick == wxEmptyString)
  {
    generateWarningDialog("You must select config to delete.");
    return;
  }

  long index = list->FindItem(-1, pick);
  list->DeleteItem(index);

  string path = "./configs/" + pick.ToStdString() + ".config";

  std::cout << path << std::endl;
  remove(path.c_str());


  string confsPath = "./config";
  string temporaryConfigPath = "./config2";

  ofstream temporaryConfig (temporaryConfigPath);

  ifstream confs;
  confs.open(confsPath);
  string boofor;

  if(confs.is_open())
  {
    while(!confs.eof())
    {
      getline(confs, boofor);
      if(boofor.length() != 0)
      {
        if(boofor != pick.ToStdString())
          temporaryConfig << boofor << endl;
      }
    }
  }

  remove(confsPath.c_str());
  rename("./config2", "./config");
  logCoreMessage("Deleted '" + pick.ToStdString() + "' config");
}

wxString ConfigDialog::getSelectedConfigName()
{
  long item = -1;
  item = list->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

  if(item == -1)
    return wxEmptyString;
  return list->GetItemText(item);
}

wxString ConfigDialog::getDefaultIfEmpty(wxTextCtrl* input, std::string def)
{
  wxString value = input->GetValue();
  if(value.IsSameAs(wxString("")))
    return wxString(def);
  return value;
}

void ConfigDialog::saveCurrentConfigToFile(std::string path)
{
  std::ofstream outfile (path);

  outfile << parent_->dd->GetValue() << std::endl;
  outfile << parent_->dta->GetValue() << std::endl;
  outfile << parent_->local->GetValue() << std::endl;
  outfile << transformIfEmptyString(parent_->refval->GetValue()) << std::endl;
  outfile << parent_->limit->GetValue() << std::endl;
  outfile << parent_->rescale->GetValue() << std::endl;
  outfile << parent_->dims->GetValue() << std::endl;
  outfile << transformIfEmptyString(parent_->plane->GetValue()) << std::endl;
  outfile << transformIfEmptyString(parent_->refSlice->GetValue()) << std::endl;
  outfile << transformIfEmptyString(parent_->tarSlice->GetValue()) << std::endl;

  outfile << getDefaultIfEmpty(parent_->refA, "1") << std::endl;
  outfile << getDefaultIfEmpty(parent_->refB, "0") << std::endl;
  outfile << getDefaultIfEmpty(parent_->tarA, "1") << std::endl;
  outfile << getDefaultIfEmpty(parent_->tarB, "0") << std::endl;
  outfile << transformIfEmptyString(parent_->filter->GetValue()) << std::endl;
  outfile << parent_->saveModes->GetValue() << std::endl;

  outfile.close();
}

wxString ConfigDialog::transformIfEmptyString(wxString value)
{
  if(value.IsSameAs(wxString("")))
    return wxString("wxEmptyString");
  return value;
}
