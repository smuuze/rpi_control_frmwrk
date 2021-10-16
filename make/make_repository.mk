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

GIT			= git
GIT_FRMWRK_PATH		= -C $(FRMWRK_PATH)
GIT_LOG			= log -1
GIT_STATUS		= status

repo_last_commit_frmwrk:
	$(VERBOSE) $(GIT) $(GIT_FRMWRK_PATH) $(GIT_LOG) $(GREP) commit $(AWK) '/(commit)/ {print $$2}'

repo_check_modifications_frmwrk:
	$(VERBOSE) $(GIT) $(GIT_FRMWRK_PATH) $(GIT_STATUS) $(GREP) Changes

repo_remember_frmwrk_revision: repo_check_modifications_frmwrk
	$(VERBOSE) $(GIT) $(GIT_FRMWRK_PATH) $(GIT_LOG) $(GREP) commit $(AWK) '/(commit)/ {print $$2}' > framework_revision
