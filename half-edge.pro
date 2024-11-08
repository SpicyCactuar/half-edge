QT+=opengl
LIBS+=-lGLU
TEMPLATE = app
TARGET = ./bin/half-edge
INCLUDEPATH += ./src
OBJECTS_DIR=./build/obj
MOC_DIR=./build/moc
CONFIG += c++17

 # You can make your code fail to compile if you use deprecated APIs.
 # In order to do so, uncomment the following line.
 # Please consult the documentation of the deprecated API in order to know
 # how to port your code away from it.
 # You can also select to disable deprecated APIs only up to a certain version of Qt.
 #DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

 # Input
 HEADERS += src/ArcBall.h \
            src/ArcBallWidget.h \
            src/Cartesian3.h \
            src/TriangleMesh.h \
            src/Homogeneous4.h \
            src/Matrix4.h \
            src/Quaternion.h \
            src/RenderController.h \
            src/RenderParameters.h \
            src/RenderWidget.h \
            src/RenderWindow.h \
            src/SphereVertices.h

 SOURCES += src/ArcBall.cpp \
            src/ArcBallWidget.cpp \
            src/Cartesian3.cpp \
            src/TriangleMesh.cpp \
            src/Homogeneous4.cpp \
            src/main.cpp \
            src/Matrix4.cpp \
            src/Quaternion.cpp \
            src/RenderController.cpp \
            src/RenderWidget.cpp \
            src/RenderWindow.cpp \
            src/SphereVertices.cpp


