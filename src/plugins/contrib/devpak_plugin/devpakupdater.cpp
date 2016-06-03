/***************************************************************
 * Name:      devpakupdater.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yiannis Mandravellos<mandrav@codeblocks.org>
 * Created:   05/12/05 15:44:32
 * Copyright: (c) Yiannis Mandravellos
 * License:   GPL
 **************************************************************/

#include "devpakupdater.h"
#include "updatedlg.h"
#include "conf.h"
#include <manager.h>
#include <configmanager.h>
#include <compilerfactory.h>
#include <compiler.h>
#include <wx/msgdlg.h>
#include <wx/dirdlg.h>
#include <wx/intl.h>

// Register the plugin
namespace
{
    PluginRegistrant<DevPakUpdater> reg(wxT_2("DevPakUpdater"));
};

DevPakUpdater::DevPakUpdater()
{
	//ctor
    if(!Manager::LoadResource(wxT_2("devpakupdater.zip")))
    {
        NotifyMissingFile(wxT_2("devpakupdater.zip"));
    }
    g_MasterPath = Manager::Get()->GetConfigManager(wxT_2("devpak_plugin"))->Read(wxT_2("/master_path"));
}

DevPakUpdater::~DevPakUpdater()
{
	//dtor
}

void DevPakUpdater::OnAttach()
{
}

void DevPakUpdater::OnRelease(bool appShutDown)
{
}

bool DevPakUpdater::ConfigurationValid()
{
    // let's make sure we have a valid configuration
    if (g_MasterPath.IsEmpty() || !wxDirExists(g_MasterPath))
    {
        if (wxMessageBox(_("The Dev-C++ DevPak Plugin is not configured yet.\nDo you want to configure it now?"), _("Question"), wxICON_QUESTION | wxYES_NO) == wxNO)
            return false;
        if (Configure() != 0)
            return false;
        // ask to add in compiler paths
        if (wxMessageBox(_("Do you want to add this path to the compiler's search dirs?\n"
                        "(needed to be able to actually compile anything)"),
                        _("Question"), wxICON_QUESTION | wxYES_NO) == wxYES)
        {
            Compiler* compiler = CompilerFactory::GetCompiler(0); // GCC is always first compiler
            if (!compiler)
            {
                wxMessageBox(_("Invalid compiler!?!"), _("Error"), wxICON_ERROR);
                return true;
            }
            compiler->AddIncludeDir(g_MasterPath + wxFILE_SEP_PATH + wxT_2("include"));
            compiler->AddLibDir(g_MasterPath + wxFILE_SEP_PATH + wxT_2("lib"));
        }
    }
    return true;
}

int DevPakUpdater::Configure()
{
    if (g_MasterPath.IsEmpty())
        g_MasterPath = ConfigManager::GetConfigFolder() + wxFILE_SEP_PATH + wxT_2("DevPaks");
	wxString dir = wxDirSelector(_("Please select the path where DevPaks will be downloaded and installed:"),
                                g_MasterPath);
    if (!dir.IsEmpty())
    {
        g_MasterPath = dir;
        if (!ConfigurationValid())
        {
            g_MasterPath.Clear();
            return -1;
        }
        Manager::Get()->GetConfigManager(wxT_2("devpak_plugin"))->Write(wxT_2("/master_path"), g_MasterPath);
        return 0;
    }
    return -1;
}

int DevPakUpdater::Execute()
{
    if (!ConfigurationValid())
        return -1;
    UpdateDlg dlg(Manager::Get()->GetAppWindow());
    dlg.ShowModal();
	return 0;
}
