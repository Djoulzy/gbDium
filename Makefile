.DEFAULT_GOAL := all
.DELETE_ON_ERROR:

# Machine-specific overrides stay outside version control.
-include config.local.mk

GBDK_HOME ?= $(if $(GBDKDIR),$(GBDKDIR),$(CURDIR)/.tools/gbdk)
LCC ?= $(patsubst %/,%,$(GBDK_HOME))/bin/lcc
PNG2ASSET ?= $(patsubst %/,%,$(GBDK_HOME))/bin/png2asset
ROMUSAGE ?= $(patsubst %/,%,$(GBDK_HOME))/bin/romusage
PYTHON ?= python3
HOST_CC ?= cc
HOST_CFLAGS ?= -O2 -std=c11 -Wall -Wextra -Werror
LCCFLAGS ?= -debug -Wf--max-allocs-per-node50000 -Wf--opt-code-speed
LDFLAGS ?= -Wl-m -Wm-ynGBDIUM -Wm-yt0 -Wm-yo2 -Wm-ya0
PROJECTNAME := gbDium
OBJDIR := obj
ROM := $(PROJECTNAME).gb
SOURCES := $(wildcard src/*.c res/*.c src/*.s res/*.s)
OBJECTS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(basename $(SOURCES))))
HEADERS := $(wildcard src/*.h res/*.h)
SAMEBOY_APP ?=
RUNNER ?=

.PHONY: all default setup doctor check-toolchain check check-logic run test image usage clean help FORCE

all default: $(ROM)

setup:
	@sh scripts/setup-gbdk.sh

check-toolchain:
	@test -x "$(LCC)" || { printf 'GBDK introuvable : %s\nLancer make setup ou définir GBDK_HOME dans config.local.mk.\n' "$(LCC)" >&2; exit 1; }

doctor: check-toolchain
	@printf 'GBDK : %s\n' "$(LCC)"
	@$(PYTHON) --version
	@SAMEBOY_APP="$(SAMEBOY_APP)" RUNNER="$(RUNNER)" sh scripts/run-sameboy.sh --check

# Rebuild when the compiler, flags or source list changes.
$(OBJDIR)/build-config: FORCE
	@mkdir -p "$(OBJDIR)"
	@printf '%s\n' '$(LCC)' '$(LCCFLAGS)' '$(LDFLAGS)' '$(SOURCES)' > "$@.tmp"
	@cmp -s "$@.tmp" "$@" && rm "$@.tmp" || mv "$@.tmp" "$@"

$(OBJDIR)/%.o: %.c $(HEADERS) $(OBJDIR)/build-config Makefile | check-toolchain
	@mkdir -p "$(@D)"
	"$(LCC)" $(LCCFLAGS) -c -o "$@" "$<"

$(OBJDIR)/%.o: %.s $(OBJDIR)/build-config Makefile | check-toolchain
	@mkdir -p "$(@D)"
	"$(LCC)" $(LCCFLAGS) -c -o "$@" "$<"

$(ROM): $(OBJECTS) $(OBJDIR)/build-config | check-toolchain
	"$(LCC)" $(LCCFLAGS) $(LDFLAGS) -o "$@" $(OBJECTS)

check-logic:
	@mkdir -p "$(OBJDIR)/tests"
	$(HOST_CC) $(HOST_CFLAGS) -Itests/stubs -Isrc tests/test-game.c src/GraphLib.c src/player.c src/mobs.c src/boss.c $(wildcard res/*.c) -o "$(OBJDIR)/tests/check-game"
	@"$(OBJDIR)/tests/check-game"

check: $(ROM) check-logic
	@$(PYTHON) scripts/check-rom.py "$(ROM)"

run: check
	@SAMEBOY_APP="$(SAMEBOY_APP)" RUNNER="$(RUNNER)" sh scripts/run-sameboy.sh "$(abspath $(ROM))"

# Preserve the original development command.
test: run

image: check-toolchain
	"$(PNG2ASSET)" assets/title.png -map -noflip -bpp 2 -o res/title.c
	"$(PNG2ASSET)" assets/bigShip.png -map -noflip -keep_duplicate_tiles -bpp 2 -o res/bigShip.c

usage: $(ROM)
	"$(ROMUSAGE)" $(PROJECTNAME).noi -g

clean:
	rm -rf "$(OBJDIR)/src" "$(OBJDIR)/res" "$(OBJDIR)/tests"
	rm -f "$(OBJDIR)/build-config" "$(OBJDIR)/build-config.tmp" $(OBJDIR)/*.o
	rm -f $(PROJECTNAME).gb $(PROJECTNAME).ihx $(PROJECTNAME).map $(PROJECTNAME).noi $(PROJECTNAME).sym $(PROJECTNAME).cdb $(PROJECTNAME).adb

help:
	@printf '%s\n' 'make setup       : installer GBDK 4.5.0 dans .tools/gbdk' 'make doctor      : vérifier GBDK, Python et SameBoy' 'make             : compiler gbDium.gb' 'make check       : compiler, tester la logique et vérifier la ROM' 'make check-logic : tester la logique avec le compilateur C de la machine' 'make test        : compiler, tester et ouvrir dans SameBoy (alias : make run)' 'make image       : réexporter les PNG du titre et du boss vers res/' 'make usage       : afficher la mémoire ROM et RAM occupée' 'make clean       : supprimer la compilation, conserver les sauvegardes'
