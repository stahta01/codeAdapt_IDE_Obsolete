#ifndef ENVVARS_H
#define ENVVARS_H

#include <wx/string.h>
#include <wx/window.h>

#include <map>

class wxMenu;
class wxMenuBar;
class wxToolBar;

class TiXmlElement;

class caProject;

#include "api/plugin.h" // the base class we 're inheriting
#include "configurationpanel.h"
#include "sdk_events.h"

typedef std::map<caProject*, wxString> ProjectEnvvarMap;

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

class EnvVars : public caPlugin
{
public:
  /// Ctor
           EnvVars();
  /// Dtor
  virtual ~EnvVars();

  wxString GetProjectEnvvarSet(caProject* project)
  { return m_ProjectSets[project]; }

  void     SetProjectEnvvarSet(caProject* project, const wxString& envvar_set);

protected:
  /// fires when a project is being loaded / saved
  void     OnProjectLoadingHook(caProject* project, TiXmlElement* elem,
                                bool loading);

  /// fires when a project is being activated
  void     OnProjectActivated(CodeBlocksEvent& event);

  /// fires when a project is being closed
  void     OnProjectClosed(CodeBlocksEvent& event);

private:
  /// fires when the plugin is attached to the application:
  void     OnAttach();

  /// fires when the plugin is released from the application:
  void     OnRelease(bool appShutDown);

  /// configures the plugin
  int      Configure();

  /// returns the configuration priority (when to setup the plugin)
  int      GetConfigurationPriority() const
  { return 50; }

  /// returns the configuration group this plugin belongs to
  int      GetConfigurationGroup() const
  { return  cgContribPlugin; }

  /// returns the configuration panel for the plugin (if any)
  virtual  caConfigurationPanel* GetConfigurationPanel(wxWindow* parent);

  /// returns the project configuration panel for the plugin (if any)
  virtual  caConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent,
                                                              caProject* project);

  /// hooks into the menu build process to allow the plugin to add menu entries
  void     BuildMenu(wxMenuBar* menuBar)
  { return; }

  /// hooks into the module menu build process to allow the plugin to add menu entries
  void     BuildModuleMenu(const ModuleType type, wxMenu* menu,
                           const caFileTreeData* data = 0)
  { return; }

  /// hooks into the toolbar build process to allow the plugin to add an own toolbar
  bool     BuildToolBar(wxToolBar* toolBar)
  { return false; }

  /// issues a warning if an activated project has a reference to an envvar set that does not exist
  void     EnvvarSetWarning(const wxString& envvar_set);

  int              m_EnvVarHookID; //!< project loader hook ID
  ProjectEnvvarMap m_ProjectSets;  //!< preferred envvar sets for all projects

  DECLARE_EVENT_TABLE()
};

#endif // ENVVARS_H
