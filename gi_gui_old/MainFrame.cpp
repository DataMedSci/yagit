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

#include "MainFrame.h"
#include "ConfigDialog.h"
#include <wx/richtooltip.h>

MainFrame::MainFrame():wxFrame(NULL, wxID_ANY, "yet Another Gamma Index Tool")
{

  parser = new ParametersParser();

  configsSet = {};

  wxGridSizer* mainSizer = new wxGridSizer(1, 2, 15, 15);

  wxGridSizer* leftSizer = new wxGridSizer(3, 1, 10, 10);
  wxGridBagSizer* rightSizer = new wxGridBagSizer();

  wxGridSizer* filesSizer = new wxGridSizer(4, 1, 5, 5);
  wxGridSizer* computationSizer = new wxGridSizer(1, 2, 0, 0);
  wxGridSizer* statisticsSizer = new wxGridSizer(5, 2, 5, 5);
  wxGridSizer* calculationSizer = new wxGridSizer(2, 1, 5, 5);
  wxGridSizer* savingSizer = new wxGridSizer(2, 1, 5, 5);

  wxGridSizer* parametersSizer = new wxGridSizer(5, 4, 0, 0); // size is a guess

  mainSizer->Add(leftSizer, 0, wxEXPAND);
  mainSizer->Add(rightSizer, 0, wxEXPAND);

  // for logo
  wxBitmap bitmap("gammaBig.bmp", wxBITMAP_TYPE_BMP);
  wxStaticBitmap* logo = new wxStaticBitmap(this, wxID_ANY, bitmap, wxDefaultPosition, wxDefaultSize, wxEXPAND);
  // wxButton* image = new wxButton(this, -1, "LOGO");
  leftSizer->Add(logo, 0, wxALL | wxEXPAND, 15);

  // leftSizer->Add(new wxButton(this, -1, "LOGO"), 0, wxEXPAND);
  leftSizer->Add(filesSizer, 0, wxALL | wxEXPAND, 15);
  leftSizer->Add(computationSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 15);

  computationSizer->Add(statisticsSizer, 0, wxALL | wxEXPAND, 5);
  computationSizer->Add(calculationSizer, 0, wxALL | wxEXPAND, 5);

  // rightSizer->Add(parametersSizer, 0, wxEXPAND);
  // rightSizer->Add(new wxButton(this, -1, "Manage Parameters"), 0, wxEXPAND);


  // for filesSizer
  referenceButton = new wxButton(this, ID_Reference, "Set Reference File");
  referenceConfim = new wxTextCtrl(this, ID_ReferenceConfirm, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_LEFT | wxTE_MULTILINE);
  referenceConfim->AppendText("Reference Path: None");
  targetButton = new wxButton(this, ID_Target, "Set Target File");
  targetConfirm = new wxTextCtrl(this, ID_TargetConfirm, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_LEFT | wxTE_MULTILINE);
  targetConfirm->AppendText("Target Path: None");

  filesSizer->Add(referenceButton, 0, wxEXPAND);
  filesSizer->Add(referenceConfim, 0, wxEXPAND);
  filesSizer->Add(targetButton, 0, wxEXPAND);
  filesSizer->Add(targetConfirm, 0, wxEXPAND);


  // for statisticsSizer
  string statusDescription = "The current status of the yAGIT program.";
  string minDescription = "Minimal value of Gamma Index in the latest comparison.";
  string avgDescription = "Average value of Gamma Index in the latest comparison.";
  string maxDescription = "Maximum value of Gamma Index in the latest comparison.";
  string gprDescription = "Gamma Passing Rate (percentage of voxels with value below 1.0)\nof Gamma Index in the latest comparison.";

  status = new LabeledTextCtrl("Status", statusDescription, this, ID_Status, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE | wxTE_CAPITALIZE);
  min_ = new LabeledTextCtrl("Minimal", minDescription, this, ID_MIN, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
  avg = new LabeledTextCtrl("Average", avgDescription, this, ID_AVG, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
  max = new LabeledTextCtrl("Maximal", maxDescription, this, ID_MAX, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
  gpr = new LabeledTextCtrl("Gamma Passing Rate", gprDescription, this, ID_GPR, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);

  status->AppendText("Waiting");
  min_->AppendText("-");
  avg->AppendText("-");
  max->AppendText("-");
  gpr->AppendText("-");

  statisticsSizer->Add(new wxStaticText(this, -1, wxT("Status:")), 0, wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE_HORIZONTAL);
  statisticsSizer->Add(status, 0, wxEXPAND);
  statisticsSizer->Add(new wxStaticText(this, -1, wxT("Minimal:")), 0, wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE_HORIZONTAL);
  statisticsSizer->Add(min_, 0, wxEXPAND);
  statisticsSizer->Add(new wxStaticText(this, -1, wxT("Average:")), 0, wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE_HORIZONTAL);
  statisticsSizer->Add(avg, 0, wxEXPAND);
  statisticsSizer->Add(new wxStaticText(this, -1, wxT("Maximal:")), 0, wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE_HORIZONTAL);
  statisticsSizer->Add(max, 0, wxEXPAND);
  statisticsSizer->Add(new wxStaticText(this, -1, wxT("Gamma\nPassing Rate:")), 0, wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE_HORIZONTAL);
  statisticsSizer->Add(gpr, 0, wxEXPAND);

  // for calculationSizer
  calculate = new wxButton(this, ID_Calculate, "Calculate Gamma Index");
  calculate->Disable();
  saveGIButton = new wxButton(this, ID_GammaIndex, "Save Gamma Index");
  saveGIButton->Disable();
  saveStatsButton = new wxButton(this, ID_SaveStats, "Save Stats");
  saveStatsButton->Disable();

  savingSizer->Add(saveGIButton, 0, wxEXPAND);
  savingSizer->Add(saveStatsButton, 0, wxEXPAND);

  calculationSizer->Add(calculate, 0, wxEXPAND);
  calculationSizer->Add(savingSizer, 0, wxEXPAND);

  // for rightSizer
  string ddDescription = "Normalization factor for Dose Difference (DD) component [in %].";
  string dtaDescription = "Normalization factor for Distance To Agreement (DTA) component [in mm].";
  dd = new LabeledTextCtrl("Dose Difference", wxString(ddDescription), this, ID_DD, wxEmptyString, wxDefaultPosition, wxSize(120, 30), wxTE_CENTRE);
  dta = new LabeledTextCtrl("Distance To Agreement", wxString(dtaDescription), this, ID_DTA, wxEmptyString, wxDefaultPosition, wxSize(120, 30), wxTE_CENTRE);

  wxArrayString localItems;
  localItems.Add(wxT("Local"));
  localItems.Add(wxT("Global"));
  string localDescription = "States whether DD is multiplied by refVal (global)\nor current reference image value (local) in the Dose Difference component denominator.";
  local = new LabeledCombobox("Algorithm Version", localDescription, this, ID_Local, "Global", wxDefaultPosition, wxSize(120, 30), localItems, wxCB_SORT|wxTE_READONLY);

  string refvalDescription = "With DD creates normalization factor in global version of Gamma Index algorithm.\nIf passed 0 or less it is set to maximal value in reference image.";
  string limitDescription = "Initial value of Gamma Index.\nThe greater the limit is, the longer the computation will take.";
  refval = new LabeledTextCtrl("Reference Value", refvalDescription, this, ID_RefVal, wxEmptyString, wxDefaultPosition, wxSize(120, 30), wxTE_CENTRE);
  limit = new LabeledTextCtrl("Initial Gamma\nIndex Value", limitDescription, this, ID_Limit, wxEmptyString, wxDefaultPosition, wxSize(120, 30), wxTE_CENTRE);

  wxArrayString rescaleItems;
  rescaleItems.Add(wxT("1"));
  rescaleItems.Add(wxT("2"));
  rescaleItems.Add(wxT("3"));
  rescaleItems.Add(wxT("4"));
  rescaleItems.Add(wxT("5"));
  rescaleItems.Add(wxT("6")); // know can be done with fore. It temporaray to do
  string rescaleDescription = "Number of digits after decimal point in the resulting Gamma Index values.\nIt only affects saving image.";
  rescale = new LabeledCombobox("Precision", rescaleDescription, this, ID_Rescale, "3", wxDefaultPosition, wxSize(120, 30), rescaleItems, wxTE_READONLY | wxTE_CENTRE);

  wxArrayString dimentionItems;
  dimentionItems.Add(wxT("1D"));
  dimentionItems.Add(wxT("2D"));
  dimentionItems.Add(wxT("3D"));
  dimentionItems.Add(wxT("2.5D"));
  dimentionItems.Add(wxT("2D From 3D"));
  string dimsDescription = "Type of comparison to perform.";
  dims = new LabeledCombobox("Dimension", dimsDescription, this, ID_Dims, "3D", wxDefaultPosition, wxSize(120, 30), dimentionItems, wxTE_READONLY | wxTE_CENTRE);

  wxArrayString planeItems;
  planeItems.Add(wxT("XY"));
  planeItems.Add(wxT("XZ"));
  planeItems.Add(wxT("YZ"));
  string planeDescription = "Plane of the comparison.\nRequired only for 2.5D and 2Dfrom3D comparisons.";
  plane = new LabeledCombobox("Plane", planeDescription, this, ID_Plane, wxEmptyString, wxDefaultPosition, wxSize(120, 30), planeItems, wxTE_READONLY | wxTE_CENTRE);

  string refSliceDescription = "The number of slice of reference image under comparison.\nRequired only for 2.5D and 2Dfrom3D comparisons.";
  string tarSliceDescription = "The number of slice of target image under comparison.\nRequired only 2Dfrom3D comparison.";
  string refADescription = "Value that the reference image will be multiplied by before comparison.";
  string refBDescription = "Value that will be added to the reference image before comparison.";
  string tarADescription = "Value that the target image will be multiplied by before comparison.";
  string tarBDescription = "Value that will be added to the target image before comparison.";
  string filterDescription = "The percentage of the maximum value in the input images below\nwhich the voxels will be considered as a noise and excluded from the comparison.";

  refSlice = new LabeledTextCtrl("Reference Slice", refSliceDescription, this, ID_RefSlice, wxEmptyString, wxDefaultPosition, wxSize(120, 30), wxTE_CENTRE);
  tarSlice = new LabeledTextCtrl("Target Slice", tarSliceDescription, this, ID_TarSlice, wxEmptyString, wxDefaultPosition, wxSize(120, 30), wxTE_CENTRE);
  refA = new LabeledTextCtrl("Reference Multiplier", refADescription, this, ID_RefA, "1", wxDefaultPosition, wxSize(120, 30), wxTE_CENTRE);
  refB = new LabeledTextCtrl("Reference Offset", refBDescription, this, ID_RefB, "0", wxDefaultPosition, wxSize(120, 30), wxTE_CENTRE);
  tarA = new LabeledTextCtrl("Target Multiplier ", tarADescription, this, ID_TarA, "1", wxDefaultPosition, wxSize(120, 30), wxTE_CENTRE);
  tarB = new LabeledTextCtrl("Target Offset", tarBDescription, this, ID_TarB, "0", wxDefaultPosition, wxSize(120, 30), wxTE_CENTRE);
  filter = new LabeledTextCtrl("Filter", filterDescription, this, ID_Filter, wxEmptyString, wxDefaultPosition, wxSize(120, 30), wxTE_CENTRE);

  plane->Disable();
  refSlice->Disable();
  tarSlice->Disable();

  wxArrayString saveModesItems;
  saveModesItems.Add(wxT("All tags"));
  saveModesItems.Add(wxT("Any tag"));
  saveModesItems.Add(wxT("All except private"));
  string saveModesDescription = "The strategy of transfering tags from reference DICOM to the output one.";
  saveModes = new LabeledCombobox("Save modes", saveModesDescription, this, ID_SaveMode, wxString("All tags"), wxDefaultPosition, wxSize(120, 30), saveModesItems, wxTE_READONLY | wxTE_CENTRE);

  wxStaticText* filterLabel = new wxStaticText(this, -1, wxT("Filter:")); // to clean

// TODO: Resolve problem with wxWidgets on Windows enviroment connected with recapturing mouse in the same window
#if defined(unix) || defined(__unix) || defined(__unix__)
  enableTips = new wxCheckBox(this, ID_EnableTips, "Enable parameters\ndescription");
#endif

  wxStaticText* parametersLabel = new wxStaticText(this, -1, wxT("Parameters"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  parametersLabel->SetFont(wxFont(16, wxDEFAULT, wxNORMAL, wxNORMAL));
  rightSizer->Add(parametersLabel, wxGBPosition(0, 0), wxGBSpan(1, 4), wxALIGN_CENTRE);
  rightSizer->Add(new wxStaticText(this, -1, wxT("Dose\nDifference:")), wxGBPosition(1, 0), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(dd, wxGBPosition(1, 1));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Distance To\nAgreement:")), wxGBPosition(1, 2), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(dta, wxGBPosition(1, 3));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Algorithm\nversion:")), wxGBPosition(2, 0), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(local, wxGBPosition(2, 1));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Reference\nValue:")), wxGBPosition(2, 2), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(refval, wxGBPosition(2, 3));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Initial Gamma\nIndex value:")), wxGBPosition(3, 0), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(limit, wxGBPosition(3, 1));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Precision:")), wxGBPosition(3, 2), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(rescale, wxGBPosition(3, 3));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Dimension:")), wxGBPosition(4, 0), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(dims, wxGBPosition(4, 1));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Plane:")), wxGBPosition(4, 2), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(plane, wxGBPosition(4, 3));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Reference\nSlice:")), wxGBPosition(5, 0), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(refSlice, wxGBPosition(5, 1));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Target\nSlice:")), wxGBPosition(5, 2), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(tarSlice, wxGBPosition(5, 3));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Reference\nMultiplier:")), wxGBPosition(6, 0), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(refA, wxGBPosition(6, 1));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Reference\nOffset:")), wxGBPosition(6, 2), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(refB, wxGBPosition(6, 3));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Target\nMultiplier:")), wxGBPosition(7, 0), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(tarA, wxGBPosition(7, 1));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Target\nOffset:")), wxGBPosition(7, 2), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(tarB, wxGBPosition(7, 3));
  rightSizer->Add(filterLabel, wxGBPosition(8, 0), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(filter, wxGBPosition(8, 1));
  rightSizer->Add(new wxStaticText(this, -1, wxT("Save\nMode:")), wxGBPosition(8, 2), wxDefaultSpan, wxALIGN_CENTRE_HORIZONTAL);
  rightSizer->Add(saveModes, wxGBPosition(8, 3));

// TODO: Resolve problem with wxWidgets on Windows enviroment connected with recapturing mouse in the same window
#if defined(unix) || defined(__unix) || defined(__unix__)
  rightSizer->Add(enableTips, wxGBPosition(10, 0), wxGBSpan(1, 2), wxGROW | wxALL, 10);
#endif

  rightSizer->Add(new wxButton(this, ID_ManageParameters, "Manage Parameters"), wxGBPosition(10, 2), wxGBSpan(1, 2), wxGROW | wxALL, 10);

  for(int i=0; i<4; i++)
    rightSizer->AddGrowableCol(i);

  for(int i=0; i<9; i++)
    rightSizer->AddGrowableRow(i);
  rightSizer->AddGrowableRow(10);

  menuBar = new wxMenuBar;
  options = new wxMenu;
  enableLogs = options->AppendCheckItem(ID_EnableLogs, wxT("Enable Logs"));
  menuBar->Append(options, wxT("Options"));
  SetMenuBar(menuBar);

  firstPhotoSet = false;
  SetSizer(mainSizer);
  wxSize* size = new wxSize(900, 620);
  SetMinSize((*size));
  SetIcon(wxIcon(wxT("gamma.ico"), wxBITMAP_TYPE_ICO));
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);

  initializeCoreLogger();
  initializeWrapperLogger();
  disableCoreLogging();
  disableWrapperLogging();
}

void MainFrame::OnExit(wxCommandEvent& event)
{
  Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
  wxMessageBox("This is yAGIT", "About yAGIT", wxOK | wxICON_INFORMATION);
}

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_BUTTON(ID_Reference, MainFrame::OnReference)
  EVT_BUTTON(ID_Target, MainFrame::OnTarget)
  EVT_BUTTON(ID_GammaIndex, MainFrame::OnSave)
  EVT_BUTTON(ID_Calculate, MainFrame::OnCalculate)
  EVT_BUTTON(ID_ManageParameters, MainFrame::OnManageParameters)
  EVT_BUTTON(ID_SaveStats, MainFrame::OnSaveStats)
  EVT_TEXT(ID_Dims, MainFrame::OnDimention)
  EVT_COMBOBOX(ID_Local, MainFrame::OnLocal)

  EVT_TEXT(ID_DD, MainFrame::OnDD)
  EVT_TEXT(ID_DTA, MainFrame::OnDTA)
  EVT_TEXT(ID_RefVal, MainFrame::OnRefVal)
  EVT_TEXT(ID_Limit, MainFrame::OnLimit)
  EVT_TEXT(ID_Plane, MainFrame::OnPlane)
  EVT_TEXT(ID_RefSlice, MainFrame::OnRefSlice)
  EVT_TEXT(ID_TarSlice, MainFrame::OnTarSlice)
  EVT_TEXT(ID_RefA, MainFrame::OnRefA)
  EVT_TEXT(ID_RefB, MainFrame::OnRefB)
  EVT_TEXT(ID_TarA, MainFrame::OnTarA)
  EVT_TEXT(ID_TarB, MainFrame::OnTarB)

  EVT_CHECKBOX(ID_EnableTips, MainFrame::OnEnabledTipsChanged)
  EVT_MENU(ID_EnableLogs, MainFrame::OnEnableLoggingChanged)
wxEND_EVENT_TABLE()

void MainFrame::OnReference(wxCommandEvent& event)
{
  logCoreMessage("Setting reference image ...");
  wxFileDialog openFileDialog(this, _("Open reference file"), "", "",
      "DICOM files (*.dcm;*.DCM)|*.dcm;*.DCM|IMA images (*.ima;*.IMA)|*.ima;*.IMA", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

  if(openFileDialog.ShowModal() == wxID_CANCEL)
    return;

  wxFileInputStream input_stream(openFileDialog.GetPath());
  if(!input_stream.IsOk())
  {
    wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
    return;
  }

  refPath = openFileDialog.GetPath();
  setSomeTextCtrl(referenceConfim, "Reference Path: " + refPath.ToStdString());

  if(firstPhotoSet)
    calculate->Enable();
  firstPhotoSet = true;

  logCoreMessage("Reference image has been set to " + refPath.ToStdString());
}

void MainFrame::OnTarget(wxCommandEvent& event)
{
  logCoreMessage("Setting target image ...");
  wxFileDialog openFileDialog(this, _("Open target file"), "", "",
      "DICOM files (*.dcm;*.DCM)|*.dcm;*.DCM|IMA images (*.ima;*.IMA)|*.ima;*.IMA", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if(openFileDialog.ShowModal() == wxID_CANCEL)
    return;

  wxFileInputStream input_stream(openFileDialog.GetPath());
  if(!input_stream.IsOk())
  {
    wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
    return;
  }

  tarPath = openFileDialog.GetPath();
  setSomeTextCtrl(targetConfirm, "Target Path: " + tarPath.ToStdString());

  if(firstPhotoSet)
    calculate->Enable();
  firstPhotoSet = true;

  logCoreMessage("Target image has been set to " + tarPath.ToStdString());
}

void MainFrame::OnSave(wxCommandEvent& event)
{
  logCoreMessage("Selecting image path to write Gamma Index comparison ...");
  wxFileDialog saveFileDialog(this, _("Save Gamma Index"), "", "",
                  "DICOM files (*.dcm)|*.dcm", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
  if(saveFileDialog.ShowModal() == wxID_CANCEL)
    return;

  logCoreMessage("Path to image with Gamma Index comparison has been set to " + saveFileDialog.GetPath().ToStdString());

  int strategy = -1;

  if(saveModes->GetValue().ToStdString().compare("All tags") == 0)
  {
    strategy = 1;
  }

  if(saveModes->GetValue().ToStdString().compare("Any tag") == 0)
  {
    strategy = 2;
  }

  if(saveModes->GetValue().ToStdString().compare("All except private") == 0)
  {
    strategy = 3;
  }

  set <pair<int, int>> emptyTagSet = {};

  logCoreMessage("Saving Gamma Index result ...");
  saveImage(lastComparison->dims, lastComparison->refDataSet, saveFileDialog.GetPath().ToStdString(), lastComparison->gamma, lastComparison->refXNumber, lastComparison->refYNumber, lastComparison->refZNumber, lastComparison->rescale, lastComparison->limit * 1.1, strategy, emptyTagSet);
  logCoreMessage("Gamma Index result has been saved");

}

void MainFrame::OnCalculate(wxCommandEvent& event)
{
  logCoreMessage("Parsing parameters ...");

  string notEmptyParams = checkForConfigPresence();
  if(notEmptyParams != "")
  {
    generateErrorDialog("Missing mandatory parameters in:\n" + notEmptyParams);
    return;
  }
  setState("Calculating");

  Parameters* parsedParameters;
  try
  {
    parsedParameters = parser->resolveParameters(dims->GetValue(), dd->GetValue(), dta->GetValue(), refval->GetValue(),
     local->GetValue(), limit->GetValue(), rescale->GetValue(), plane->GetValue(), refSlice->GetValue(), tarSlice->GetValue(),
     refA->GetValue(), refB->GetValue(), tarA->GetValue(), tarB->GetValue());
  }
  catch (const invalid_argument& ia)
  {
    generateErrorDialog("Invalid value in parameters");
    setState("Waiting");
    return;
  }

  string wrongParams = checkForConfigCorrectioness(parsedParameters);
  if(wrongParams != "")
  {
    generateErrorDialog("Incorrect negative values in:\n" + wrongParams);
    setState("Waiting");
    return;
  }
  logCoreMessage("Parameters has been parsed");

  int refNDims, tarNDims, refXNumber, refYNumber, refZNumber, tarXNumber, tarYNumber, tarZNumber;
  double refXStart, refYStart, refZStart, tarXStart, tarYStart, tarZStart,
          refXSpacing, refYSpacing, refZSpacing, tarXSpacing, tarYSpacing, tarZSpacing;
  logCoreMessage("Loading reference image ...");
  unique_ptr<imebra::DataSet> refDataSet(loadDicom(refPath.ToStdString()));
  logCoreMessage("Reference image has been loaded");
  logCoreMessage("Loading target image ...");
  unique_ptr<imebra::DataSet> tarDataSet(loadDicom(tarPath.ToStdString()));
  logCoreMessage("Target image has been loaded");
  logCoreMessage("Processing reference image ...");
  auto reference = acquireImage(*refDataSet, refNDims,
                                   refXStart, refXSpacing, refXNumber,
                                   refYStart, refYSpacing, refYNumber,
                                   refZStart, refZSpacing, refZNumber);
  logCoreMessage("Reference image has been processed");
  logCoreMessage("Processing target image ...");
  auto target = acquireImage(*tarDataSet, tarNDims,
                                tarXStart, tarXSpacing, tarXNumber,
                                tarYStart, tarYSpacing, tarYNumber,
                                tarZStart, tarZSpacing, tarZNumber);
  logCoreMessage("Target image has been processed");
  if (refNDims != tarNDims || (tarNDims != parsedParameters->dims && (tarNDims != 3 || parsedParameters->dims < 3)))
  {
      cerr << "Error. Dimensions mismatch." << endl;
      exit(-1);
  }
  int algorithm = 2;

  int refSize = calculateGammaArraySize(parsedParameters->dims, refXNumber, refYNumber, refZNumber, parsedParameters->plane);
  int tarSize = calculateGammaArraySize(parsedParameters->dims, tarXNumber, tarYNumber, tarZNumber, parsedParameters->plane);

  logCoreMessage("Applying linear mapping to both images ...");
  applyLinearMappingToImage(refSize, reference.get(), parsedParameters->refA, parsedParameters->refB);
  applyLinearMappingToImage(tarSize, target.get(), parsedParameters->tarA, parsedParameters->tarB);
  logCoreMessage("Linear mapping has been applied");

  string filterValue = filter->GetValue().ToStdString();

  if(filterValue != "")
  {
    logCoreMessage("Appling filter to both images ...");
    applyNoiseFilteringToImage(refSize, reference.get(), stod(filterValue));
    applyNoiseFilteringToImage(tarSize, target.get(), stod(filterValue));
    logCoreMessage("Filter has been applied");
  }

  logCoreMessage("Calculating Gamma Index ...");
  double* gamma = calculateGamma(algorithm, parsedParameters->dims, reference.get(), target.get(),
                                 refXStart, refXSpacing, refXNumber,
                                 refYStart, refYSpacing, refYNumber,
                                 refZStart, refZSpacing, refZNumber,
                                 tarXStart, tarXSpacing, tarXNumber,
                                 tarYStart, tarYSpacing, tarYNumber,
                                 tarZStart, tarZSpacing, tarZNumber,
                                 parsedParameters->dd, parsedParameters->dta, parsedParameters->local,
                                 parsedParameters->refVal, parsedParameters->limit);
  logCoreMessage("Gamma Index has been calculated");
  setState("Finished");

  string info =  createComputationDescription(refPath.ToStdString(), tarPath.ToStdString(), algorithm);

  logCoreMessage("Setting comparison statistics ...");
  setMin(std::to_string(imageMin(refSize, gamma)));
  setAvg(std::to_string(imageAvg(refSize, gamma)));
  setMax(std::to_string(imageMax(refSize, gamma)));
  setGPR(std::to_string(gammaPassingRate(refSize, gamma)));
  logCoreMessage("Statistics has been set");

  lastComparison = new Comparison(parsedParameters->dims, CodecFactory::load(refPath.ToStdString()), gamma,
    refXNumber, refYNumber, refZNumber, parsedParameters->rescale, info, refSize, parsedParameters->limit);

  saveGIButton->Enable();
  saveStatsButton->Enable();
}

void MainFrame::setState(string state)
{
  status->Clear();
  status->AppendText(wxString(state));
  status->Refresh();
  Update();
}

void MainFrame::setMin(string state)
{
  min_->Clear();
  min_->AppendText(wxString(state));
}

void MainFrame::setAvg(string state)
{
  avg->Clear();
  avg->AppendText(wxString(state));
}

void MainFrame::setMax(string state)
{
  max->Clear();
  max->AppendText(wxString(state));
}

void MainFrame::setGPR(string state)
{
  gpr->Clear();
  gpr->AppendText(wxString(state));
}


void MainFrame::setSomeTextCtrl(wxTextCtrl *what, string value)
{
  what->Clear();
  what->AppendText(wxString(value));
}

void MainFrame::OnManageParameters(wxCommandEvent& event)
{
  ConfigDialog *dialog = new ConfigDialog(this, wxT("Config Dialog"));
  dialog->Show(true);
}

void MainFrame::OnDimention(wxCommandEvent& event)
{
  string state = dims->GetValue().ToStdString();
  if(state.compare("1D") == 0 || state.compare("2D") == 0 || state.compare("3D") == 0)
  {
    plane->Disable();
    refSlice->Disable();
    tarSlice->Disable();
  }
  else if(state.compare("2.5D") == 0)
  {
    plane->Enable();
    refSlice->Enable();
    tarSlice->Disable();
  }
  else
  {
    plane->Enable();
    refSlice->Enable();
    tarSlice->Enable();
  }
  setState("Waiting");
}

void MainFrame::OnLocal(wxCommandEvent& event)
{
  if(local->GetValue().ToStdString().compare("Local") == 0)
  {
    refval->Disable();
  } else {
    refval->Enable();
  }
  setState("Waiting");
}

void MainFrame::OnDD(wxCommandEvent& event)
{
  setState("Waiting");
}

void MainFrame::OnDTA(wxCommandEvent& event)
{
  setState("Waiting");
}

void MainFrame::OnRefVal(wxCommandEvent& event)
{
  setState("Waiting");
}

void MainFrame::OnLimit(wxCommandEvent& event)
{
  setState("Waiting");
}

void MainFrame::OnPlane(wxCommandEvent& event)
{
  setState("Waiting");
}

void MainFrame::OnRefSlice(wxCommandEvent& event)
{
  setState("Waiting");
}

void MainFrame::OnTarSlice(wxCommandEvent& event)
{
  setState("Waiting");
}

void MainFrame::OnRefA(wxCommandEvent& event)
{
  setState("Waiting");
}

void MainFrame::OnRefB(wxCommandEvent& event)
{
  setState("Waiting");
}

void MainFrame::OnTarA(wxCommandEvent& event)
{
  setState("Waiting");
}

void MainFrame::OnTarB(wxCommandEvent& event)
{
  setState("Waiting");
}

void MainFrame::OnFilter(wxCommandEvent& event)
{
  setState("Waiting");
}

string MainFrame::checkForConfigPresence()
{
  logCoreMessage("Checking for configuration presence ...");
  string wrongParameters = "";
  if(dd->GetValue().IsSameAs(wxString("")))
    wrongParameters = appendWrongParameterToDescription(wrongParameters, "Dose Difference");
  if(dta->GetValue().IsSameAs(wxString("")))
    wrongParameters = appendWrongParameterToDescription(wrongParameters, "Distance To Agreement");
  if(limit->GetValue().IsSameAs(wxString("")))
    wrongParameters = appendWrongParameterToDescription(wrongParameters, "Initial Gamma Index Value");
  if((dims->GetValue().IsSameAs(wxString("2.5D")) || dims->GetValue().IsSameAs(wxString("2D From 3D")))
      && plane->GetValue().IsSameAs(wxString("")))
    wrongParameters = appendWrongParameterToDescription(wrongParameters, "Plane");
  if((dims->GetValue().IsSameAs(wxString("2.5D")) || dims->GetValue().IsSameAs(wxString("2D From 3D")))
      && refSlice->GetValue().IsSameAs(wxString("")))
    wrongParameters = appendWrongParameterToDescription(wrongParameters, "Reference Slice");
  if(dims->GetValue().IsSameAs(wxString("2D From 3D")) && tarSlice->GetValue().IsSameAs(wxString("")))
    wrongParameters = appendWrongParameterToDescription(wrongParameters, "Target Slice");
  logCoreMessage("Configuration presence has been checked");
  return wrongParameters;
}

string MainFrame::checkForConfigCorrectioness(Parameters* parsedParameters)
{
  logCoreMessage("Checking for configuration correctness ...");
  string wrongParameters = "";
  if(parsedParameters->dd <= 0)
    wrongParameters = appendWrongParameterToDescription(wrongParameters, "Dose Difference");
  if(parsedParameters->dta <= 0)
    wrongParameters = appendWrongParameterToDescription(wrongParameters, "Dose Difference");
  if(parsedParameters->limit < 0)
    wrongParameters = appendWrongParameterToDescription(wrongParameters, "Initial Gamma Index Value");
  if((parsedParameters->dims == 4 || parsedParameters->dims == 5) &&
      parsedParameters->refSlice < 0)
    wrongParameters = appendWrongParameterToDescription(wrongParameters, "Reference Slice");
  if(parsedParameters->dims == 5 && parsedParameters->tarSlice < 0)
    wrongParameters = appendWrongParameterToDescription(wrongParameters, "Target Slice");
  string filterString = filter->GetValue().ToStdString();
  if(filterString != "")
  {
    double parsedFilter;
    bool properValue = true;
    try
    {
      parsedFilter = stod(filterString);
    }
    catch (const invalid_argument& ia)
    {
      properValue = false;
    }

    if((properValue && parsedFilter < 0) || !properValue)
      wrongParameters = appendWrongParameterToDescription(wrongParameters, "Filter");
  }
  logCoreMessage("Configuration correctness has been checked");
  return wrongParameters;
}

string MainFrame::appendWrongParameterToDescription(string source, string parameter)
{
  if(source == "")
    return parameter;
  return source + "\n" + parameter;
}

void MainFrame::OnEnabledTipsChanged(wxCommandEvent& event)
{
  logCoreMessage("Turning on/off hints ...");
  dims->setListening(enableTips->IsChecked());
  dd->setListening(enableTips->IsChecked());
  dta->setListening(enableTips->IsChecked());
  refval->setListening(enableTips->IsChecked());
  limit->setListening(enableTips->IsChecked());
  rescale->setListening(enableTips->IsChecked());
  local->setListening(enableTips->IsChecked());
  plane->setListening(enableTips->IsChecked());
  refSlice->setListening(enableTips->IsChecked());
  tarSlice->setListening(enableTips->IsChecked());
  filter->setListening(enableTips->IsChecked());
  refA->setListening(enableTips->IsChecked());
  refB->setListening(enableTips->IsChecked());
  tarA->setListening(enableTips->IsChecked());
  tarB->setListening(enableTips->IsChecked());
  saveModes->setListening(enableTips->IsChecked());

  status->setListening(enableTips->IsChecked());
  min_->setListening(enableTips->IsChecked());
  avg->setListening(enableTips->IsChecked());
  max->setListening(enableTips->IsChecked());
  gpr->setListening(enableTips->IsChecked());
  logCoreMessage("Hints has been turned on/off");

}

void MainFrame::OnSaveStats(wxCommandEvent& event)
{
  logCoreMessage("Saving comparison statistics");
  wxFileDialog saveFileDialog(this, _("Save statistics"), "", "",
                  "Text files (*.txt)|*.txt", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
  if(saveFileDialog.ShowModal() == wxID_CANCEL)
    return;

  prepareStatisticsFile(lastComparison->size, lastComparison->gamma, lastComparison->info, saveFileDialog.GetPath().ToStdString());
  logCoreMessage("Comparison statistics has been saved in " + saveFileDialog.GetPath().ToStdString());
}

void MainFrame::generateErrorDialog(string message)
{
  wxMessageDialog *dialog = new wxMessageDialog(NULL,
    wxString(message), wxT("Error"),
    wxOK | wxICON_ERROR);
  dialog->ShowModal();
}

void MainFrame::OnEnableLoggingChanged(wxCommandEvent& event)
{
  if(enableLogs->IsChecked())
  {
    enableCoreLogging();
    enableWrapperLogging();
  }
  else
  {
    disableCoreLogging();
    disableWrapperLogging();
  }
}
