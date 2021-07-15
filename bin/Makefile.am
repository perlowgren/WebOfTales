
CXXFLAGS = -Wall -Werror -O2 -g -I../src/

bin_PROGRAMS = weboftales wotclient

weboftales_SOURCES = \
	../src/command.h \
	../src/Main.h \
	../src/game/Character.h \
	../src/game/City.h \
	../src/game/Cloud.h \
	../src/game/Coordinate.h \
	../src/game/Effect.h \
	../src/game/Enum.h \
	../src/game/Item.h \
	../src/game/Location.h \
	../src/game/Map.h \
	../src/game/NPC.h \
	../src/game/Player.h \
	../src/game/Race.h \
	../src/game/Skill.h \
	../src/game/Team.h \
	../src/server/CreateAccountDlg.h \
	../src/server/CreateCharDlg.h \
	../src/server/CreateWorldDlg.h \
	../src/server/InstallDlg.h \
	../src/server/LoginDlg.h \
	../src/server/ServerWorld.h \
	../src/server/WebOfTales.h \
	../src/server/WoTFrame.h \
	../src/command.cpp \
	../src/config.h \
	../src/config.cpp \
	../src/game/Character.cpp \
	../src/game/City.cpp \
	../src/game/Cloud.cpp \
	../src/game/Coordinate.cpp \
	../src/game/Effect.cpp \
	../src/game/Item.cpp \
	../src/game/Location.cpp \
	../src/game/Map.cpp \
	../src/game/NPC.cpp \
	../src/game/Player.cpp \
	../src/game/Race.cpp \
	../src/game/Skill.cpp \
	../src/game/Team.cpp \
	../src/server/CreateAccountDlg.cpp \
	../src/server/CreateCharDlg.cpp \
	../src/server/CreateWorldDlg.cpp \
	../src/server/InstallDlg.cpp \
	../src/server/LoginDlg.cpp \
	../src/server/ServerWorld.cpp \
	../src/server/WebOfTales2.cpp \
	../src/server/WebOfTales.cpp \
	../src/server/WoTFrame.cpp

weboftales_CPPFLAGS = -DWOTSERVER -DWOT_GTK
weboftales_CXXFLAGS = `pkg-config --cflags gtk+-2.0`
weboftales_LDADD = -lamanita -lSDL -lSDL_net
weboftales_LDFLAGS = `pkg-config --libs gtk+-2.0`


wotclient_SOURCES = \
	../src/command.h \
	../src/Main.h \
	../src/client/BattleDlg.h \
	../src/client/CharacterDlg.h \
	../src/client/CityDisplay.h \
	../src/client/IncarnateDlg.h \
	../src/client/ItemToolTip.h \
	../src/client/MapDisplay.h \
	../src/client/MapDlg.h \
	../src/client/Message.h \
	../src/client/Panel.h \
	../src/client/TeamDlg.h \
	../src/client/WorldDisplay.h \
	../src/client/WoTClient.h \
	../src/game/Character.h \
	../src/game/City.h \
	../src/game/Cloud.h \
	../src/game/Coordinate.h \
	../src/game/Effect.h \
	../src/game/Enum.h \
	../src/game/Item.h \
	../src/game/Location.h \
	../src/game/Map.h \
	../src/game/NPC.h \
	../src/game/Player.h \
	../src/game/Race.h \
	../src/game/Skill.h \
	../src/game/Team.h \
	../src/command.cpp \
	../src/config.cpp \
	../src/config.h \
	../src/client/BattleDlg.cpp \
	../src/client/CharacterDlg.cpp \
	../src/client/CityDisplay.cpp \
	../src/client/IncarnateDlg.cpp \
	../src/client/ItemToolTip.cpp \
	../src/client/MapDisplay.cpp \
	../src/client/MapDlg.cpp \
	../src/client/Message.cpp \
	../src/client/Panel.cpp \
	../src/client/TeamDlg.cpp \
	../src/client/WorldDisplay.cpp \
	../src/client/WoTClient2.cpp \
	../src/client/WoTClient.cpp \
	../src/game/Character.cpp \
	../src/game/City.cpp \
	../src/game/Cloud.cpp \
	../src/game/Coordinate.cpp \
	../src/game/Effect.cpp \
	../src/game/Item.cpp \
	../src/game/Location.cpp \
	../src/game/Map.cpp \
	../src/game/NPC.cpp \
	../src/game/Player.cpp \
	../src/game/Race.cpp \
	../src/game/Skill.cpp \
	../src/game/Team.cpp

wotclient_CPPFLAGS = -DWOTCLIENT -DLIBAMANITA_SDL
wotclient_CXXFLAGS = `pkg-config --cflags gtk+-2.0`
wotclient_LDADD = -lamanita-sdl -lSDL -lSDL_net -lSDL_image -lSDL_ttf
wotclient_LDFLAGS = `pkg-config --libs gtk+-2.0`

