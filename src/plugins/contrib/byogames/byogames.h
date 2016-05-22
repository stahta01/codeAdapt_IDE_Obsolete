/***************************************************************
 * Name:      byogames.h
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#ifndef BYOGAMES_H
#define BYOGAMES_H


#include <wx/timer.h>
#include "api/plugin.h" // the base class we 're inheriting

class BYOGames : public caToolPlugin
{
	public:

		BYOGames();
		~BYOGames();
		int Configure(){ return  0; }
		int GetConfigurationPriority() const{ return  25; }
		int GetConfigurationGroup() const { return cgContribPlugin; }
		caConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
		int Execute();
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application

	private:

        wxTimer SecondTick;
        void OnTimer(wxTimerEvent& event);

        int SelectGame();

        DECLARE_EVENT_TABLE()
};

#endif
