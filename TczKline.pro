######################################################################
# Automatically generated by qmake (2.01a) ?? ?? 11 22:53:03 2015
######################################################################

TEMPLATE = app
TARGET = TczKline
CONFIG(x64){
TARGET = $$TARGET"64"
}

include($$(SOUIPATH)/demo_com.pri)

CONFIG(debug,debug|release){
	LIBS += utilitiesd.lib souid.lib
}
else{
	LIBS += utilities.lib soui.lib
}

PRECOMPILED_HEADER = stdafx.h

HEADERS += MainDlg.h resource.h SKlinePic.h res/resource.h
SOURCES += MainDlg.cpp SKlinePic.cpp stdafx.cpp TczKline.cpp


RC_FILE += TczKline.rc