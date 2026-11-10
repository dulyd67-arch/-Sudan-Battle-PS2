EE_BIN = SUDAN_BATTLE.ELF
EE_OBJS = src/main.o src/engine/memory.o src/engine/camera.o src/engine/input.o src/engine/world_stream.o src/render/renderer.o src/physics/collision.o src/gameplay/match.o src/gameplay/combat.o src/gameplay/vehicles.o src/ai/bot_ai.o src/network/lan_net.o src/audio/sound.o src/ui/hud.o
EE_INCS = -I./src
EE_CFLAGS = -O2 -Wall -Wno-unused-variable -DPS2_TARGET
EE_CXXFLAGS = -O2 -Wall -fno-exceptions -fno-rtti -DPS2_TARGET
EE_LDFLAGS = -L$(PS2SDK)/ee/lib
EE_LIBS = -ldraw -lgraph -lmath3d -lpacket -ldma -lpad -lmc -laudsrv -lnetman -lps2ip -lc -lm
all: $(EE_BIN)
clean:
	rm -f $(EE_BIN) $(EE_OBJS)
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
