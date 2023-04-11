local dir = pathJoin(os.getenv("PWD"),"/..")
setenv("HOST_OS","WCOSS")


setenv("BASE_DIR",dir)
setenv("INCLUDEepoch",pathJoin(dir,"include"))
setenv("HOMEepoch",dir)
setenv("EXECepoch",pathJoin(dir,"exec"))
setenv("LIBepoch",pathJoin(dir,"lib"))
setenv("PARMepoch",pathJoin(dir,"parm"))
setenv("RAP_DIR",pathJoin(dir,"buildOutput"))
setenv("RAP_SHARED_DIR",pathJoin(dir,"buildOutput"))

setenv("EPOCH_SRC_DIR",pathJoin(dir,"sorc"))
setenv("RAP_MAKE_INC_DIR",pathJoin(dir,"sorc/make_include"))

setenv("RAP_INC_DIR",pathJoin(dir,"include"))
setenv("RAP_SHARED_INC_DIR",pathJoin(dir,"include"))

setenv("RAP_LIB_DIR",pathJoin(dir,"lib"))
setenv("RAP_SHARED_LIB_DIR",pathJoin(dir,"lib"))

setenv("RAP_BIN_DIR",pathJoin(dir,"exec"))
setenv("RAP_SHARED_BIN_DIR",pathJoin(dir,"exec"))


