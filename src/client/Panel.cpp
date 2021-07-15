
#include "../config.h"
#include <math.h>
#include <string.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aIconButton.h>
#include <libamanita/sdl/aTooltip.h>
#include "../Main.h"
#include "../command.h"
#include "WoTClient.h"
#include "Panel.h"
#include "../game/Player.h"


aObject_Inheritance(Panel,aComponent);



Panel::Panel() : aComponent(0,0,g.getScreenWidth(),34) {
app.printf("Panel::Panel(1)");
	setLocation(0,g.getScreenHeight()-getHeight());

	aFont *f = app.fonts[FONT_WHITE];
	chatTextField = new aTextfield(5,getHeight()-3+f->getDescent(),getWidth()-418,f->getLineSkip(),"");
	chatTextField->setFont(f);
	chatTextField->setVisible(true);
	chatTextField->setEnabled(true);
	chatTextField->setShowCaret(true);
	chatTextField->setMaxLength(120);
	add(chatTextField);

	aIconButton *b;
	b = new aIconButton(CMD_SETTINGS,368,1);
	b->setImage(app.gui[10],1,2,3,4);
	b->setActionListener(this);
	b->setToolTip(new aTooltip("Settings"));
	add(b);
	b = new aIconButton(CMD_SHOW_HIDE_MAP,352,1);
	b->setImage(app.gui[10],5,6,7,8);
	b->setActionListener(this);
	b->setToolTip(new aTooltip("Show/Hide Map (Ctrl-M)"));
	add(b);
	b = new aIconButton(CMD_INVENTORY,336,1);
	b->setImage(app.gui[10],9,10,11,12);
	b->setActionListener(this);
	b->setToolTip(new aTooltip("Show/Hide Inventory (Ctrl-I)"));
	add(b);
	b = new aIconButton(CMD_CHARACTER_SHEET,320,1);
	b->setImage(app.gui[10],13,14,15,16);
	b->setActionListener(this);
	b->setToolTip(new aTooltip("Show/Hide Character Sheet (Ctrl-C)"));
	add(b);
	b = new aIconButton(CMD_SHOW_HIDE_TEAM,304,1);
	b->setImage(app.gui[10],17,18,19,20);
	b->setActionListener(this);
	b->setToolTip(new aTooltip("Create/Show/Hide Team (Ctrl-T)"));
	add(b);

	b = new aIconButton(101,393,1);
	b->setImage(app.gui[0],62,63,64,62);
	b->setActionListener(this);
	b->setToolTip(new aTooltip("Next Set of Actionbuttons (Alt-Up)"));
	add(b);
	b = new aIconButton(102,393,18);
	b->setImage(app.gui[0],65,66,67,65);
	b->setActionListener(this);
	b->setToolTip(new aTooltip("Previous Set of Actionbuttons (Alt-Down)"));
	add(b);

}

Panel::~Panel() {
}


void Panel::paint(time_t time) {
	app.mainFrameImage->draw(getX(),getY()+getHeight()-app.mainFrameImage->getCell(2)->h,2);
	if(app.getPlayer()) {
		Player &pl = *app.getPlayer();
		SDL_Rect r = *app.gui[1]->getCell(6);
		r.w = pl.getAttackXP(r.w);
		app.gui[1]->draw(getX()+9,getY()+2,r);
		r = *app.gui[1]->getCell(7);
		r.w = pl.getXP(r.w);
		app.gui[1]->draw(getX()+9,getY()+10,r);
	}
}

bool Panel::mouseDown(aMouseEvent &me) {
	return false;
}

bool Panel::mouseMove(aMouseMotionEvent &mme) {
	return false;
}

bool Panel::mouseDrag(aMouseMotionEvent &mme) {
	return false;
}

bool Panel::actionPerformed(aActionEvent &ae) {
	unsigned long id = ae.source->getID();
	if(id>=CMD_SETTINGS && id<=CMD_SHOW_HIDE_TEAM) app.command(id);
	return true;
}


