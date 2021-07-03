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

repo_last_commit_frmwrk:
	$(VERBOSE) $(GIT) $(GIT_FRMWRK_PATH) $(GIT_LOG) | grep commit
