# --------------------------------------------------------------------------------------
#
#	File	:	/frmwrk/make/make_repository
#	Author	:	sebastian lesse
#
#	Brief	:	This file handles repositories based operations
#			You can use it to get the actual commit name of your project or
#			the frmwrk
#
# --------------------------------------------------------------------------------------

GIT					= git
GIT_FRMWRK_PATH		= -C $(FRMWRK_PATH)
GIT_LOG				= log -1
GIT_STATUS			= status
GIT_PULL			= $(GIT) pull

#---------------------------------------------------------------------------------------

FRMWRK_REV := UNDEFINED
-include framework_revision

#---------------------------------------------------------------------------------------

repo_last_commit_frmwrk:
	$(VERBOSE) $(GIT) $(GIT_FRMWRK_PATH) $(GIT_LOG) $(GREP) commit $(AWK) '/(commit)/ {print $$2}'

repo_check_modifications_frmwrk:
	$(VERBOSE) $(GIT) $(GIT_FRMWRK_PATH) $(GIT_STATUS) $(GREP) Changes

save_frmwrk_revision: repo_check_modifications_frmwrk
	$(VERBOSE) $(ECHO_NO_N) "FRMWRK_REV := " > framework_revision
	$(VERBOSE) $(GIT) $(GIT_FRMWRK_PATH) $(GIT_LOG) $(GREP) commit $(AWK) '/(commit)/ {print $$2}' >> framework_revision

show_frmwrk_revision:
	$(VERBOSE) $(ECHO) $(FRMWRK_REV)

load_frmwrk_revision:
ifneq  ($(FRMWRK_REV), UNDEFINED)
	$(VERBOSE) $(GIT) $(GIT_FRMWRK_PATH) checkout $(FRMWRK_REV)
endif
