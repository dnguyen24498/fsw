IMPORT_SERVICES += \
    fswconnectivity.fo \
    fswengineering.fo \
    fswupdateengine.fo
    
include services/connectivity/connectivity.mk
include services/engineering/engineering.mk
include services/updateengine/updateengine.mk
