#ifndef SC_PLUGIN_H
#define SC_PLUGIN_H

#include "sc_base_types.h"
#include <wx/dynarray.h>

class caFileTreeData;
class wxMenu;

namespace ScriptBindings
{
    namespace ScriptPluginWrapper
    {
        wxArrayInt CreateMenu(wxMenuBar* mbar);
        wxArrayInt CreateModuleMenu(const ModuleType type, wxMenu* menu, const caFileTreeData* data);

        void OnScriptMenu(int id);
        void OnScriptModuleMenu(int id);
    }; // namespace ScriptPluginWrapper
};

#endif // SC_PLUGIN_H
