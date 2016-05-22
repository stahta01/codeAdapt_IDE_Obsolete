/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk.h"
#include <wx/xrc/xmlres.h>
#include <manager.h>
#include <configmanager.h>
#include <pluginmanager.h>
#include <api/plugin.h>
#include <wx/intl.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/checklst.h>
#include <wx/filename.h>
#include <wx/imaglist.h>
#include <wx/sizer.h>	// SetSizeHints
#include <wx/stattext.h>	// wxStaticText

#include "configurationpanel.h"
#include "compilersettingsdlg.h"
#include "appglobals.h"
#ifdef __WXMSW__
    #include "associations.h"
#endif

// images by order of pages
const wxString base_imgs[] =
{
    _T("batch"),
};
const int IMAGES_COUNT = sizeof(base_imgs) / sizeof(wxString);

BEGIN_EVENT_TABLE(CompilerSettingsDlg, wxDialog)
    EVT_LISTBOOK_PAGE_CHANGING(XRCID("nbMain"), CompilerSettingsDlg::OnPageChanging)
    EVT_LISTBOOK_PAGE_CHANGED(XRCID("nbMain"), CompilerSettingsDlg::OnPageChanged)
END_EVENT_TABLE()

// This dialog initially contains only the batch-build settings.
// So we 'll add all compiler-related configuration before it
// and all debugger-related after it...

CompilerSettingsDlg::CompilerSettingsDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgCompilerSettings"));
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    wxImageList* images = new wxImageList(80, 80);
    lb->AssignImageList(images);
    int sel = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/environment/settings_size"), 0);
    SetSettingsIconsStyle(lb->GetListView(), (SettingsIconsStyle)sel);

    // tab "Batch builds"
    if(platform::windows)
        XRCCTRL(*this, "txtBatchBuildsCmdLine", wxTextCtrl)->SetValue(Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/batch_build_args"), appglobals::DefaultBatchBuildArgs));
    else
        XRCCTRL(*this, "txtBatchBuildsCmdLine", wxTextCtrl)->Enable(false);

    // fill plugins list
    ConfigManager *bbcfg = Manager::Get()->GetConfigManager(_T("plugins"));
    wxArrayString bbplugins = bbcfg->ReadArrayString(_T("/batch_build_plugins"));
    if (!bbplugins.GetCount())
    {
        // defaults
        if(platform::windows)
            bbplugins.Add(_T("compiler.dll"));
        else
            bbplugins.Add(_T("libcompiler.so"));
    }
    wxCheckListBox* clb = XRCCTRL(*this, "chkBBPlugins", wxCheckListBox);
    clb->Clear();
    clb->SetMinSize(wxSize(-1, 150));
    const PluginElementsArray& plugins = Manager::Get()->GetPluginManager()->GetPlugins();
    for (size_t i = 0; i < plugins.GetCount(); ++i)
    {
        PluginElement* elem = plugins[i];
        if (!elem)
            continue;
        caPlugin* plugin = elem->plugin;
        if (!plugin || !plugin->IsAttached())
            continue;
        wxString filename = wxFileName(elem->fileName).GetFullName();
        size_t index = clb->Append(elem->info.title);
        // check item if any wildcard matches
        for (size_t n = 0; n < bbplugins.GetCount(); ++n)
        {
            if (filename.CmpNoCase(bbplugins[n]) == 0)
            {
                clb->Check(index, plugin->IsAttached());
                break;
            }
        }
    }

    // add all plugins configuration panels
    AddPluginPanels();

    // make sure everything is laid out properly
    GetSizer()->SetSizeHints(this);
    CentreOnParent();
}

CompilerSettingsDlg::~CompilerSettingsDlg()
{
    //dtor
}

void CompilerSettingsDlg::AddPluginPanels()
{
    const wxString base = _T("images/settings/");
    const wxString noimg = _T("images/settings/generic-plugin");
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);

    // we 'll remove the existing page and add it when appropriate
    wxWindow* existingPage = lb->GetPage(0);
    wxString existingTitle = lb->GetPageText(0);
    lb->RemovePage(0);

    // we 'll do this in two steps:
    // 1) get all compiler-related panels and put them before batch-builds
    // 2) get all debugger panels and put them after batch-builds

    ConfigurationPanelsArray local;

    // step 1
    Manager::Get()->GetPluginManager()->GetConfigurationPanels(cgCompiler, lb, local);
    for (size_t i = 0; i < local.GetCount(); ++i)
    {
        caConfigurationPanel* panel = local[i];
        lb->AddPage(panel, panel->GetTitle());

        wxString onFile = ConfigManager::LocateDataFile(base + panel->GetBitmapBaseName() + _T(".png"), sdDataGlobal | sdDataUser);
        if (onFile.IsEmpty())
			onFile = ConfigManager::LocateDataFile(noimg + _T(".png"), sdDataGlobal | sdDataUser);
        wxString offFile = ConfigManager::LocateDataFile(base + panel->GetBitmapBaseName() + _T("-off.png"), sdDataGlobal | sdDataUser);
        if (offFile.IsEmpty())
			offFile = ConfigManager::LocateDataFile(noimg + _T("-off.png"), sdDataGlobal | sdDataUser);

        lb->GetImageList()->Add(cbLoadBitmap(onFile));
        lb->GetImageList()->Add(cbLoadBitmap(offFile));
        lb->SetPageImage(lb->GetPageCount() - 1, lb->GetImageList()->GetImageCount() - 2);

        // add it in our central container too
        m_PluginPanels.Add(panel);
    }

    // now load the builtin pages' images
    lb->AddPage(existingPage, existingTitle);
	wxString onFile = ConfigManager::LocateDataFile(base + base_imgs[0] + _T(".png"), sdDataGlobal | sdDataUser);
	if (onFile.IsEmpty())
		onFile = ConfigManager::LocateDataFile(noimg + _T(".png"), sdDataGlobal | sdDataUser);
	wxString offFile = ConfigManager::LocateDataFile(base + base_imgs[0] + _T("-off.png"), sdDataGlobal | sdDataUser);
	if (offFile.IsEmpty())
		offFile = ConfigManager::LocateDataFile(noimg + _T("-off.png"), sdDataGlobal | sdDataUser);

	lb->GetImageList()->Add(cbLoadBitmap(onFile));
	lb->GetImageList()->Add(cbLoadBitmap(offFile));
    lb->SetPageImage(lb->GetPageCount() -1, lb->GetImageList()->GetImageCount() - 2);

    // step 2
    Manager::Get()->GetPluginManager()->GetConfigurationPanels(cgDebugger, lb, local);
    for (size_t i = 0; i < local.GetCount(); ++i)
    {
        caConfigurationPanel* panel = local[i];
        lb->AddPage(panel, panel->GetTitle());

        wxString onFile = ConfigManager::LocateDataFile(base + panel->GetBitmapBaseName() + _T(".png"), sdDataGlobal | sdDataUser);
        if (onFile.IsEmpty())
			onFile = ConfigManager::LocateDataFile(noimg + _T(".png"), sdDataGlobal | sdDataUser);
        wxString offFile = ConfigManager::LocateDataFile(base + panel->GetBitmapBaseName() + _T("-off.png"), sdDataGlobal | sdDataUser);
        if (offFile.IsEmpty())
			offFile = ConfigManager::LocateDataFile(noimg + _T("-off.png"), sdDataGlobal | sdDataUser);

        lb->GetImageList()->Add(cbLoadBitmap(onFile));
        lb->GetImageList()->Add(cbLoadBitmap(offFile));
        lb->SetPageImage(lb->GetPageCount() - 1, lb->GetImageList()->GetImageCount() - 2);

        // add it in our central container too
        m_PluginPanels.Add(panel);
    }

    UpdateListbookImages();
}

void CompilerSettingsDlg::UpdateListbookImages()
{
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    int sel = lb->GetSelection();
    // set page images according to their on/off status
    for (size_t i = 0; i < IMAGES_COUNT + m_PluginPanels.GetCount(); ++i)
    {
        lb->SetPageImage(i, (i * 2) + (sel == (int)i ? 0 : 1));
    }

    // the selection colour is ruining the on/off effect,
    // so make sure no item is selected ;)
    // (only if we have icons showing)
    if (GetSettingsIconsStyle(lb->GetListView()) != sisNoIcons)
        lb->GetListView()->Select(sel, false);

    // update the page title
    wxString label = lb->GetPageText(sel);
    // replace any stray & with && because label makes it an underscore
    while (label.Replace(_T(" & "), _T(" && ")))
        ;
    XRCCTRL(*this, "lblBigTitle", wxStaticText)->SetLabel(label);
}

void CompilerSettingsDlg::OnPageChanging(wxListbookEvent& event)
{
}

void CompilerSettingsDlg::OnPageChanged(wxListbookEvent& event)
{
    UpdateListbookImages();
}

void CompilerSettingsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        // tab "Batch builds"
#ifdef __WXMSW__  /* TODO: remove preprocessor when Associations::SetXXX are supported on non-Windows platforms */
        ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
        wxString bbargs = XRCCTRL(*this, "txtBatchBuildsCmdLine", wxTextCtrl)->GetValue();
        if (bbargs != cfg->Read(_T("/batch_build_args"), appglobals::DefaultBatchBuildArgs))
        {
            cfg->Write(_T("/batch_build_args"), bbargs);
//            Associations::SetBatchBuildOnly();
        }

        // batch build plugins
        ConfigManager *bbcfg = Manager::Get()->GetConfigManager(_T("plugins"));
        wxArrayString bbplugins;
        wxCheckListBox* clb = XRCCTRL(*this, "chkBBPlugins", wxCheckListBox);
        for (int i = 0; i < (int)clb->GetCount(); ++i)
        {
            if (clb->IsChecked(i))
            {
                const PluginElementsArray& plugins = Manager::Get()->GetPluginManager()->GetPlugins();
                for (size_t n = 0; n < plugins.GetCount(); ++n)
                {
                    PluginElement* elem = plugins[n];
                    if (!elem)
                        continue;
                    if (elem->info.title == clb->GetString(i))
                    {
                        bbplugins.Add(wxFileName(elem->fileName).GetFullName());
                        break;
                    }
                }
            }
        }

        const wxString compiler(platform::windows ? _T("compiler.dll") : _T("libcompiler.so"));

        if (bbplugins.Index(compiler) == wxNOT_FOUND)
        {
            bbplugins.Add(compiler);
            cbMessageBox(_("The compiler plugin must always be loaded for batch builds!\n"
                        "Automatically re-enabled."),
                        _("Warning"), wxICON_WARNING);
        }
        bbcfg->Write(_T("/batch_build_plugins"), bbplugins);
#endif

        // finally, apply settings in all plugins' panels
        for (size_t i = 0; i < m_PluginPanels.GetCount(); ++i)
        {
            caConfigurationPanel* panel = m_PluginPanels[i];
            panel->OnApply();
        }
    }
    else
    {
        // finally, cancel settings in all plugins' panels
        for (size_t i = 0; i < m_PluginPanels.GetCount(); ++i)
        {
            caConfigurationPanel* panel = m_PluginPanels[i];
            panel->OnCancel();
        }
    }

    wxDialog::EndModal(retCode);
}
