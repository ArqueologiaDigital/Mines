TOPTARGETS := all clean
SUBDIRS := $(wildcard platforms/*/.)

docs:
	$(MAKE) -C docs

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: all docs $(SUBDIRS)
