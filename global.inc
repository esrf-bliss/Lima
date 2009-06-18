sub-dirs-clean = $(sub-dirs:%=%.clean)

.PHONY: $(sub-dirs)

all: $(sub-dirs)

clean: $(sub-dirs-clean)

$(sub-dirs):
	make -C $(@)

$(sub-dirs-clean):
	make -C $(@:%.clean=%) clean

