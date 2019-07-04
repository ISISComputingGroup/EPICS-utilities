#Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG
DIRS := $(DIRS) $(filter-out $(DIRS), configure)
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *App))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard iocBoot))

define DIR_template
 $(1)_DEPEND_DIRS = configure
endef
$(foreach dir, $(filter-out configure,$(DIRS)),$(eval $(call DIR_template,$(dir))))

utilitiesTestApp_DEPEND_DIRS += utilitiesApp
iocBoot_DEPEND_DIRS += $(filter %App,$(DIRS))

TEST_RUNNER = $(TOP)/utilitiesApp/src/O.$(EPICS_HOST_ARCH)/runner

include $(TOP)/configure/RULES_TOP

.PHONY: test
test:
ifneq ($(wildcard $(TEST_RUNNER)*),)
	$(TEST_RUNNER) --gtest_output=xml:$(TOP)/test-reports/TEST-Utilities.xml
endif
