/***************************************************************
 * Name:      cbprofilerconfig.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Dark Lord & Zlika
 * Created:   07/20/05 15:36:50
 * Copyright: (c) Dark Lord & Zlika
 * Thanks:    Yiannis Mandravellos and his Source code formatter (AStyle) sources
 * License:   GPL
 **************************************************************/

    #include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/checkbox.h>
    #include <wx/event.h>
    #include <wx/spinctrl.h>
    #include <wx/textctrl.h>
    #include <wx/xrc/xmlres.h>
    #include "manager.h"
    #include "configmanager.h"
#endif
#include "cbprofilerconfig.h"

BEGIN_EVENT_TABLE(CBProfilerConfigDlg, wxPanel)
    EVT_CHECKBOX(XRCID("chkAnnSource"), CBProfilerConfigDlg::CheckBoxEvent)
    EVT_CHECKBOX(XRCID("chkMinCount"),  CBProfilerConfigDlg::CheckBoxEvent)
END_EVENT_TABLE()

CBProfilerConfigDlg::CBProfilerConfigDlg(wxWindow* parent)
{
    //ctor
    wxXmlResource::Get()->LoadPanel(this, parent, wxT_2("dlgCBProfilerConfig"));
    LoadSettings();
}

CBProfilerConfigDlg::~CBProfilerConfigDlg()
{
    //dtor
}

void CBProfilerConfigDlg::CheckBoxEvent(wxCommandEvent& event)
{
    if(event.GetId() == XRCID("chkAnnSource"))
        XRCCTRL(*this, "txtAnnSource", wxTextCtrl)->Enable(XRCCTRL(*this, "chkAnnSource", wxCheckBox)->GetValue());
    if(event.GetId() == XRCID("chkMinCount"))
        XRCCTRL(*this, "spnMinCount", wxSpinCtrl)->Enable(XRCCTRL(*this, "chkMinCount", wxCheckBox)->GetValue());
}

void CBProfilerConfigDlg::LoadSettings()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(wxT_2("cbprofiler"));

    // Values to be used many times along
    bool annSource=cfg->ReadBool(wxT_2("/ann_source_chk"), false);
    bool minCount=cfg->ReadBool(wxT_2("/min_count_chk"), false);

    // Output Options
    XRCCTRL(*this, "chkAnnSource", wxCheckBox)->SetValue(annSource);
    XRCCTRL(*this, "txtAnnSource", wxTextCtrl)->SetValue(cfg->Read(wxT_2("/ann_source_txt"), wxT_2("")));
    XRCCTRL(*this, "chkBrief", wxCheckBox)->SetValue(cfg->ReadBool(wxT_2("/brief"), false));
    XRCCTRL(*this, "chkFileInfo", wxCheckBox)->SetValue(cfg->ReadBool(wxT_2("/file_info"), false));

    // Analysis Options
    XRCCTRL(*this, "chkNoStatic", wxCheckBox)->SetValue(cfg->ReadBool(wxT_2("/no_static"), false));
    XRCCTRL(*this, "chkMinCount", wxCheckBox)->SetValue(minCount);
    XRCCTRL(*this, "spnMinCount", wxSpinCtrl)->SetValue(cfg->ReadInt(wxT_2("/min_count_spn"), 0));

    // Miscellaneous Options
    XRCCTRL(*this, "chkSum", wxCheckBox)->SetValue(cfg->ReadBool(wxT_2("/sum"), false));

    // Extra Options
    XRCCTRL(*this, "txtExtra", wxTextCtrl)->SetValue(cfg->Read(wxT_2("/extra_txt"), wxEmptyString));

    // Enable/Disable the TextCtrl(s) as well as SpinCtrl(s)
    XRCCTRL(*this, "txtAnnSource", wxTextCtrl)->Enable(annSource);
    XRCCTRL(*this, "spnMinCount", wxSpinCtrl)->Enable(minCount);
}

void CBProfilerConfigDlg::SaveSettings()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(wxT_2("cbprofiler"));

    // Output Options
    cfg->Write(wxT_2("/ann_source_chk"), XRCCTRL(*this, "chkAnnSource", wxCheckBox)->GetValue());
    cfg->Write(wxT_2("/ann_source_txt"), XRCCTRL(*this, "txtAnnSource", wxTextCtrl)->GetValue());
    cfg->Write(wxT_2("/brief"), XRCCTRL(*this, "chkBrief", wxCheckBox)->GetValue());
    cfg->Write(wxT_2("/file_info"), XRCCTRL(*this, "chkFileInfo", wxCheckBox)->GetValue());

    // Analysis Options
    cfg->Write(wxT_2("/no_static"), XRCCTRL(*this, "chkNoStatic", wxCheckBox)->GetValue());
    cfg->Write(wxT_2("/min_count_chk"), XRCCTRL(*this, "chkMinCount", wxCheckBox)->GetValue());
    cfg->Write(wxT_2("/min_count_spn"), XRCCTRL(*this, "spnMinCount", wxSpinCtrl)->GetValue());

    // Miscellaneous Options
    cfg->Write(wxT_2("/sum"), XRCCTRL(*this, "chkSum", wxCheckBox)->GetValue());

    // Extra Options
    cfg->Write(wxT_2("/extra_txt"), XRCCTRL(*this, "txtExtra", wxTextCtrl)->GetValue());
}

void CBProfilerConfigDlg::OnApply()
{
    // user pressed OK; save settings
    SaveSettings();
}
