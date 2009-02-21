# ============================================================================
#  Name	 : build_help.mk
#  Part of  : LBSSample
#
#  Description: This make file will build the application help file (.hlp)
# 
# ============================================================================

do_nothing :
	@rem do_nothing

MAKMAKE :
	cshlpcmp LBSSample.cshlp
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
	copy LBSSample_0xE1038E0E.hlp $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
endif

BLD : do_nothing

CLEAN :
	del LBSSample_0xE1038E0E.hlp
	del LBSSample_0xE1038E0E.hlp.hrh

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo LBSSample_0xE1038E0E.hlp

FINAL : do_nothing
