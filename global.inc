.PHONY: src test clean

ifneq ($(sub-dirs),)
src-targets   += $(sub-dirs:%=%.src)
test-targets  += $(sub-dirs:%=%.test)
clean-targets += $(sub-dirs:%=%.clean)

$(src-targets):
	make -C $(@:%.src=%)   src

$(test-targets):
	make -C $(@:%.test=%)  test 

$(clean-targets):
	make -C $(@:%.clean=%) clean

else

ifneq ($(src-dirs),)
src-targets   += $(src-dirs:%=%.src)
clean-targets += $(src-dirs:%=%.clean)
endif

ifneq ($(test-dirs),)
test-targets  += $(test-dirs:%=%.test)
clean-targets += $(test-dirs:%=%.clean)
endif

$(src-targets):
	make -C $(@:%.src=%)

$(test-targets):
	make -C $(@:%.test=%)

$(clean-targets):
	make -C $(@:%.clean=%) clean

endif

src:	$(src-targets)

test:	$(test-targets)

clean:	$(clean-targets)
