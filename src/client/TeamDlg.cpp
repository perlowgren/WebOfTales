
#include "../config.h"
#include <stdlib.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aFont.h>
#include <libamanita/sdl/aIconButton.h>
#include <libamanita/sdl/aButton.h>
#include "../Main.h"
#include "../command.h"
#include "WoTClient.h"
#include "TeamDlg.h"
#include "ItemToolTip.h"
#include "../game/Team.h"
#include "../game/Player.h"



aObject_Inheritance(TeamDlg,aWindow);


const int TeamDlg::Width = 50;
const int TeamDlg::Height = 217;


TeamDlg::TeamDlg() : aWindow(10,70,TeamDlg::Width,TeamDlg::Height) {
app.printf("TeamDlg::TeamDlg(1)");
	team = 0;
	itt = 0;
}

TeamDlg::~TeamDlg() {
}

void TeamDlg::setTeam(Team *t) {
	if(!t) setVisible(false);
	else if(team!=t) setVisible(true);
	else setVisible(!isVisible());
	team = t;
}

void TeamDlg::paint(time_t time) {
	aWindow::paint(time);
	if(team) {
		int px = getX()+fr.l->w,py = getY()+fr.t->h;
		for(size_t i=0; i<team->sz; i++) {
			app.faces[0]->draw(px,py+i*40,team->team[i]->getIcon());
		}
	}
}

bool TeamDlg::actionPerformed(aActionEvent &ae) {
	if(aWindow::actionPerformed(ae)) return true;
	//unsigned long id = ae.source->getID();
	return false;
}


