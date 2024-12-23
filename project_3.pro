QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    gamemap.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    gamemap.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    material/picture/back/back.jpg \
    material/picture/back/back2.jpg \
    material/picture/back/bsmz.png \
    material/picture/back/gameover.png \
    material/picture/mine/destroy1.png \
    material/picture/mine/destroy2.png \
    material/picture/mine/destroy3.png \
    material/picture/mine/mine1.png \
    material/picture/mine/mine2.png \
    material/picture/mine/mine3.png \
    material/picture/mine/mine4.png \
    material/picture/mine/mine5.png \
    material/picture/mine/mine6.png \
    material/picture/mine/mine7.png \
    material/picture/mine/mine8.png \
    material/picture/num/d0.png \
    material/picture/num/d1.png \
    material/picture/num/d2.png \
    material/picture/num/d3.png \
    material/picture/num/d4.png \
    material/picture/num/d5.png \
    material/picture/num/d6.png \
    material/picture/num/d7.png \
    material/picture/num/d8.png \
    material/picture/num/d9.png \
    material/picture/tools/bomb.png \
    material/picture/tools/continue.png \
    material/picture/tools/hammer.png \
    material/picture/tools/pause.png \
    material/picture/tools/refresh.png \
    material/picture/tools/tip.jpg \
    material/sound/bgm.mp3 \
    material/sound/bomb.mp3 \
    material/sound/break.mp3 \
    material/sound/countdown.mp3 \
    material/sound/failToExchange.mp3 \
    material/sound/failure.mp3 \
    material/sound/gotEquipment.mp3 \
    material/sound/refresh.mp3 \
    material/sound/start.mp3 \
    material/sound/victory.mp3

RESOURCES += \
    resources.qrc
