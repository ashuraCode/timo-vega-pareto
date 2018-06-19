#-------------------------------------------------
#
# Project created by QtCreator 2017-05-06T12:16:21
#
#-------------------------------------------------

QT       += core gui widgets datavisualization charts

LIBS    += -lgsl -lgslcblas #-lgw32c

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VEGAsolver
TEMPLATE = app


SOURCES += main.cpp\
        ashurawin.cpp \
    Widgets/inequalitiespanel.cpp \
    ParseAndEval/compilator.cpp \
    ParseAndEval/formula.cpp \
    ParseAndEval/printer.cpp \
    Solver/vega.cpp \
    ParseAndEval/evaluator.cpp \
    Widgets/plot3d.cpp

HEADERS  += ashurawin.h \
    Widgets/inequalitiespanel.h \
    ParseAndEval/ast.h \
    ParseAndEval/boost_qi.h \
    ParseAndEval/compilator.h \
    ParseAndEval/error_handler.h \
    ParseAndEval/formula.h \
    ParseAndEval/grammar.h \
    ParseAndEval/printer.h \
    Solver/vega.h \
    ParseAndEval/evaluator.h \
    Widgets/plot3d.h

FORMS    += ashurawin.ui \
    Widgets/inequalitiespanel.ui \
    Widgets/plot3d.ui

RESOURCES += \
    resources.qrc

QMAKE_LFLAGS +=  -static -static-libgcc -static-libstdc++
