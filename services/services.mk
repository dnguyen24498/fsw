include services/connectivity/connectivity.mk
include services/engineering/engineering.mk
include services/updateengine/updateengine.mk

IMPORT_LIBS += \
	libfswconnectivity.a \
	libfswengineering.a \
	libfswupdateengine.a
