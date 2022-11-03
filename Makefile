#
# A Makefile that compiles all .c and .s files in "src" and "res" 
# subdirectories and places the output in a "obj" subdirectory
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = ${GBDKDIR}

LCC = $(GBDK_HOME)bin/lcc
# RUNNER = $(GBDK_HOME)bin/visualboyadvance-m.app/Contents/MacOS/visualboyadvance-m
# RUNNER = $(GBDK_HOME)bin/SameBoy.app/Contents/MacOS/SameBoy
# RUNNER = $(GBDK_HOME)bin/mGBA.app/Contents/MacOS/mGBA
RUNNER = $(GBDK_HOME)bgb/bgb.exe


# You can set flags for LCC here
# For example, you can uncomment the line below to turn on debug output
# LCCFLAGS = -debug

# You can set the name of the .gb ROM file here
PROJECTNAME    = gbDium

SRCDIR      = src
OBJDIR      = obj
RESDIR      = res
BINS	    = ./$(PROJECTNAME).gb
CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES  = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s)))
OBJS		= $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.s=$(OBJDIR)/%.o)

all: clean default

test: default
# $(RUNNER) $(BINS) &
# open $(BINS) -a $(RUNNER)
	/usr/local/bin/wine64 $(RUNNER) $(BINS) &

default: ${OBJS} $(BINS)

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .c files in "res/" to .o object files
$(OBJDIR)/%.o:	$(RESDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .s assembly files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.s
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# If needed, compile .c files in "src/" to .s assembly files
# (not required if .c is compiled directly to .o)
$(OBJDIR)/%.s:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -S -o $@ $<

# Link the compiled object files into a .gb ROM file
$(BINS): $(OBJS)
	$(LCC) $(LCCFLAGS) -o $(BINS) $(OBJS)

sprite:
	wine64 $(GBDK_HOME)gbtd22/GBTD.EXE

map:
	wine64 $(GBDK_HOME)gbmb18/GBMB.EXE

clean:
#	rm -f  *.gb *.ihx *.cdb *.adb *.noi *.map
	rm -f  $(OBJDIR)/*.*
	rm -f *.gb *.ihx 

