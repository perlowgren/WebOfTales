#ifndef _CLIENT_TEAMDLG_H
#define _CLIENT_TEAMDLG_H

#include <libamanita/sdl/aWindow.h>
#include "../game/Player.h"

class aIconButton;
class aButton;
class ItemToolTip;

class TeamDlg : public aWindow {
aObject_Instance(TeamDlg)

private:
	Team *team;
	ItemToolTip *itt;

public:
	static const int Width;
	static const int Height;

	TeamDlg();
	virtual ~TeamDlg();

	void setTeam(Team *t);
	Team *getTeam() { return team; }

	void paint(time_t time);
	bool actionPerformed(aActionEvent &ae);
};



#endif
