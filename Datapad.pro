# To fix the qmake crash:
# /Users/build/Development/Qt-5.5/5.5/clang_64/bin/qmake -set QMAKEFEATURES ""

TEMPLATE = app

ios {
	QMAKE_MAC_SDK = iphoneos
}
android {
}
#macx {
#	QMAKE_MAC_SDK = macosx10.11
#}

QT += qml quick xml

ios {
	LIBS += -lz
}
android {
	LIBS += -lz
}
macx {
	LIBS += /usr/local/lib/libz.a
}

ios {
	QMAKE_INFO_PLIST = Datapad-Info.plist

	ios_bundle.files = $$files($$PWD/bundle/AppIcon*.png)
	ios_bundle.files += $$PWD/bundle/iTunesArtwork
	ios_bundle.files += $$PWD/bundle/iTunesArtwork@2x
	ios_bundle.files += $$PWD/bundle/IOSLaunchImage-568h@2x.png
	ios_bundle.files += $$PWD/bundle/IOSLaunchImage@2x.png
	ios_bundle.files += $$PWD/bundle/IOSLaunchImage.png
	ios_bundle.files += $$PWD/bundle/IOSLaunchImage-Landscape@2x~ipad.png
	ios_bundle.files += $$PWD/bundle/IOSLaunchImage-Landscape~ipad.png
	QMAKE_BUNDLE_DATA += ios_bundle
}

ICON = datapad.icns

SOURCES += \
	src/ListModel.cpp \
	src/DataList.cpp \
	src/Downloader.cpp \
	src/CharacterXML.cpp \
	zlib/ioapi.c \
	zlib/mztools.c \
	zlib/unzip.c \
	zlib/zip.c \
	src/DataAccess.cpp \
	src/Species.cpp \
	src/StrUtil.cc \
    src/ImageProvider.cpp \
    src/Talents.cpp \
    src/Armor.cpp \
    src/Gear.cpp \
    src/Weapons.cpp \
    src/ItemAttachments.cpp \
    src/ItemDescriptors.cpp \
    src/Item.cpp \
    src/CurrentData.cpp \
    src/Syncer.cpp \
    src/md5.cc \
    src/Motivations.cpp \
    src/Moralities.cpp \
    src/Character.cpp \
    src/Skills.cpp \
    src/datapad.cpp \
    src/DatFile.cpp \
    src/DatUtil.cpp \
    src/DatXML.cpp \
    src/DatZip.cpp \
    src/Shop.cpp \
    src/DatXmlReader.cc \
    src/pxml.cc \
    src/DataSet.cpp \
    src/CharacterList.cpp \
    src/CharCache.cpp

RESOURCES += qml.qrc javascript.qrc data.qrc images.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
#QML_IMPORT_PATH =

QMAKE_TARGET_BUNDLE_PREFIX="space.fringer"

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
	src/ListModel.h \
	src/DataList.h \
	src/Downloader.h \
	src/CharacterXML.h \
	zlib/crypt.h \
	zlib/ioapi.h \
	zlib/mztools.h \
	zlib/unzip.h \
	zlib/zip.h \
	src/DataAccess.h \
	src/Species.h \
	src/StrUtil.h \
    src/ImageProvider.h \
    src/Talents.h \
    src/Armor.h \
    src/Gear.h \
    src/Weapons.h \
    src/Item.h \
    src/ItemAttachments.h \
    src/ItemDescriptors.h \
    src/CurrentData.h \
    src/Syncer.h \
    src/md5.h \
    src/Motivations.h \
    src/Moralities.h \
    src/Character.h \
    src/Skills.h \
    src/datapad.h \
    src/DatFile.h \
    src/DatUtil.h \
    src/DatXML.h \
    src/DatZip.h \
    src/Shop.h \
    src/DatXmlReader.h \
    src/pxml.h \
    src/DataSet.h \
    src/CharacterList.h \
    src/CharCache.h
