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

#include "Label.h"

wxBEGIN_EVENT_TABLE(LabeledTextCtrl, wxTextCtrl)
  EVT_ENTER_WINDOW(LabeledTextCtrl::OnMouseEnter)
wxEND_EVENT_TABLE()

LabeledTextCtrl::LabeledTextCtrl(wxString title_, wxString content_, wxWindow* parent_, wxWindowID id_, const wxString& label_, const wxPoint& pos_=wxDefaultPosition, const wxSize& size_=wxDefaultSize, long style_=0) : wxTextCtrl(parent_, id_, label_, pos_, size_, style_), title(title_), content(content_)
  {
    enabled = false;
  };

void LabeledTextCtrl::OnMouseEnter(wxMouseEvent& event)
{
  if(enabled)
  {
    wxRichToolTip* tip = new wxRichToolTip(title, content);

    tip->SetIcon();
    tip->SetTimeout(3000, 1000);
    tip->ShowFor(this);
  }
}

void LabeledTextCtrl::setListening(bool state)
{
  enabled = state;
}

wxBEGIN_EVENT_TABLE(LabeledCombobox, wxComboBox)
  EVT_ENTER_WINDOW(LabeledCombobox::OnMouseEnter)
wxEND_EVENT_TABLE()

LabeledCombobox::LabeledCombobox(wxString title_, wxString content_, wxWindow* parent_, wxWindowID id_, const wxString& value_, const wxPoint& pos_, const wxSize& size_, const wxArrayString& choices_, long style_=0) : wxComboBox(parent_, id_, value_, pos_, size_, choices_, style_), title(title_), content(content_)
{
  enabled = false;
};

void LabeledCombobox::OnMouseEnter(wxMouseEvent& event)
{
  if(enabled)
  {
    wxRichToolTip* tip = new wxRichToolTip(title, content);

    tip->SetIcon();
    tip->SetTimeout(3000, 1000);
    tip->ShowFor(this);
  }
}

void LabeledCombobox::setListening(bool state)
{
  enabled = state;
}
