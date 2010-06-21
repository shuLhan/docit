SRC_D		=.
BLD_D		=$(SRC_D)/build
LIBVOS_D	=$(SRC_D)/libvos

include $(LIBVOS_D)/Makefile

PRE_TARGET	+= $(BLD_D)
TARGET		=$(BLD_D)/docit

TARGET_OBJS	=	$(BLD_D)/main.oo		\
			$(BLD_D)/Docit.oo		\
			$(BLD_D)/Docit2HTML.oo		\
			$(BLD_D)/Content.oo		\
			$(LIBVOS_BLD_D)/Buffer.oo	\
			$(LIBVOS_BLD_D)/File.oo		\
			$(LIBVOS_BLD_D)/Record.oo	\
			$(LIBVOS_BLD_D)/RecordMD.oo	\
			$(LIBVOS_BLD_D)/Reader.oo	\
			$(LIBVOS_BLD_D)/libvos.oo


.PHONY: all debug clean

all: libvos-all

debug: libvos-debug

$(TARGET): $(TARGET_OBJS)
	@$(do_build)

$(BLD_D):
	@echo "[M] $@"
	@mkdir -p $@

$(BLD_D)/%.oo: $(SRC_D)/%.cpp $(SRC_D)/%.hpp
	@$(do_compile)

clean:
	@$(call do_rmdir,$(BLD_D))

distclean: clean libvos-clean
