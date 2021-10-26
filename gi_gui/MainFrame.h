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
#include <wx/checkbox.h>
#include <wx/wfstream.h>
#include <wx/gbsizer.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "ParametersParser.h"

#include "gdcmReader.h"
#include "gdcmGlobal.h"
#include "gdcmDicts.h"
#include "gdcmDict.h"
#include "gdcmAttribute.h"
#include "gdcmStringFilter.h"
#include "gdcmImageReader.h"
#include "gdcmSequenceOfItems.h"
#include "gdcmDataElement.h"
#include "dispatcher.h"
#include "wrapper.h"
#include "additions.h"
#include <set>
#include <iostream>
#include <fstream>
#include "Comparison.h"
#include <wx/cshelp.h>

#include "Label.h"

#include "core_logger.h"
#include "wrapper_logger.h"

// using namespace std;

class MainFrame: public wxFrame
{
public:
  MainFrame();

  wxButton* referenceButton;
  wxTextCtrl* referenceConfim;
  wxButton* targetButton;
  wxTextCtrl* targetConfirm;
  wxButton* saveGIButton;
  wxButton* saveStatsButton;
  wxButton* calculate;
  LabeledCombobox* saveModes;

  wxString refPath;
  wxString tarPath;

  LabeledCombobox* dims;
  LabeledTextCtrl* dd;
  LabeledTextCtrl* dta;
  LabeledTextCtrl* refval;
  LabeledTextCtrl* limit;
  LabeledCombobox* rescale;
  LabeledCombobox* local;
  LabeledCombobox* plane;
  LabeledTextCtrl* refSlice;
  LabeledTextCtrl* tarSlice;
  LabeledTextCtrl* filter;

  LabeledTextCtrl* refA;
  LabeledTextCtrl* refB;
  LabeledTextCtrl* tarA;
  LabeledTextCtrl* tarB;

  LabeledTextCtrl* status;
  LabeledTextCtrl* min_;
  LabeledTextCtrl* avg;
  LabeledTextCtrl* max;
  LabeledTextCtrl* gpr;

  LabeledCombobox* configBox; // to do - is that necessary?
  wxButton* loadConfig;
  wxButton* saveConfig;
  wxButton* deleteConfig;

  void setSomeTextCtrl(wxTextCtrl *what, string value);

private:

  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

  void OnReference(wxCommandEvent& event);
  void OnTarget(wxCommandEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnSaveStats(wxCommandEvent& event);
  void OnCalculate(wxCommandEvent& event);
  void OnManageParameters(wxCommandEvent& event);

  ParametersParser* parser;
  void setState(string status);
  void setMin(string status);
  void setAvg(string status);
  void setMax(string status);
  void setGPR(string status);

  void OnDD(wxCommandEvent& event);
  void OnDTA(wxCommandEvent& event);
  void OnLocal(wxCommandEvent& event);
  void OnRefVal(wxCommandEvent& event);
  void OnLimit(wxCommandEvent& event);
  // void OnPrecision(wxCommandEvent& event);
  void OnDimention(wxCommandEvent& event);
  void OnPlane(wxCommandEvent& event);
  void OnRefSlice(wxCommandEvent& event);
  void OnTarSlice(wxCommandEvent& event);
  void OnRefA(wxCommandEvent& event);
  void OnRefB(wxCommandEvent& event);
  void OnTarA(wxCommandEvent& event);
  void OnTarB(wxCommandEvent& event);
  void OnFilter(wxCommandEvent& event);

  set<string> configsSet;
  Comparison* lastComparison;
  string checkForConfigPresence();
  string checkForConfigCorrectioness(Parameters* parsedParameters);
  wxCheckBox* enableTips;
  void OnEnabledTipsChanged(wxCommandEvent& event);
  void generateErrorDialog(string message);
  string appendWrongParameterToDescription(string source, string parameter);
  int lastSize;
  bool firstPhotoSet;
  string lastInfo;

  wxMenuBar* menuBar;
  wxMenu* options;
  wxMenuItem* enableLogs;
  void OnEnableLoggingChanged(wxCommandEvent& event);


  wxDECLARE_EVENT_TABLE();
};

enum
{
  ID_Reference = 1,
  ID_Target = 2,
  ID_GammaIndex = 3,
  ID_Calculate = 4,

  ID_Dims = 5,
  ID_DD = 6,
  ID_DTA = 7,
  ID_RefVal = 8,
  ID_Limit = 9,
  ID_Rescale = 10,
  ID_Local = 11,

  ID_Status = 12,
  ID_MIN = 13,
  ID_AVG = 14,
  ID_MAX = 15,
  ID_GPR = 16,

  ID_ConfigPicker = 17,
  ID_LoadConfig = 18,
  ID_SaveConfig = 19,
  ID_DeleteConfig = 20,

  ID_Plane = 21,
  ID_RefSlice = 22,
  ID_TarSlice = 23,

  ID_ReferenceConfirm = 24,
  ID_TargetConfirm = 25,
  ID_SaveMode = 26,

  ID_RefA = 27,
  ID_RefB = 28,
  ID_TarA = 29,
  ID_TarB = 30,
  ID_Filter = 31,
  ID_ManageParameters = 32,
  ID_NewConfig = 33,
  ID_EnableTips = 34,
  ID_SaveStats = 35,
  ID_EnableLogs = 36
};
