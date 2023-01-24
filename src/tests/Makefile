
# Emplacement des executables de test
B_OBJDIR	:= $(shell $(GSCC_PATH)/gscc_getpath.py --wspace=$(WSPACE) --variant="$(LIST_VARIANT)" --target_dir=exe)
SUFFIX		:= $(shell $(GSCC_PATH)/gscc_getsuffix.py -c ../tests $(VARIANT) $(VERSION))

SCRIPT_GEN		:= $(B_OBJDIR)/script_gen$(SUFFIX)
PYTHON_RUNNER	:= $(B_OBJDIR)/python_runner$(SUFFIX)

.PHONY:test_dyn
test_dyn:
	mkdir -p /tmp/$(USER)
	\rm -f /tmp/$(USER)/toto.py
	\rm -f /tmp/$(USER)/out.txt
	$(SCRIPT_GEN) /tmp/$(USER)/toto.py >> /tmp/$(USER)/out.txt
	$(PYTHON_RUNNER) /tmp/$(USER)/toto.py >> /tmp/$(USER)/out.txt
