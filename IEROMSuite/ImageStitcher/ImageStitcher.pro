QT += core
QT -= gui

TARGET = ImageStitcher
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    imagestitcher.cpp

HEADERS += \
    imagestitcher.h

win32: LIBS += -L$$PWD/../../Tools/itk/lib/ \
              -llibITKBiasCorrection-4.9 \
              -llibITKBioCell-4.9 \
              -llibITKCommon-4.9 \
              -llibITKDICOMParser-4.9 \
              -llibitkdouble-conversion-4.9 \
              -llibITKEXPAT-4.9 \
              -llibITKFEM-4.9 \
              -llibitkgdcmcharls-4.9 \
              -llibitkgdcmCommon-4.9 \
              -llibitkgdcmDICT-4.9 \
              -llibitkgdcmDSED-4.9 \
              -llibitkgdcmIOD-4.9 \
              -llibitkgdcmjpeg12-4.9 \
              -llibitkgdcmjpeg16-4.9 \
              -llibitkgdcmjpeg8-4.9 \
              -llibitkgdcmMEXD-4.9 \
              -llibitkgdcmMSFF-4.9 \
              -llibitkgdcmopenjpeg-4.9 \
              -llibitkgdcmsocketxx-4.9 \
              -llibITKgiftiio-4.9 \
              -llibitkhdf5-4.9 \
              -llibitkhdf5_cpp-4.9 \
              -llibITKIOBioRad-4.9 \
              -llibITKIOBMP-4.9 \
              -llibITKIOCSV-4.9 \
              -llibITKIOGDCM-4.9 \
              -llibITKIOGE-4.9 \
              -llibITKIOGIPL-4.9 \
              -llibITKIOHDF5-4.9 \
              -llibITKIOImageBase-4.9 \
              -llibITKIOIPL-4.9 \
              -llibITKIOJPEG-4.9 \
              -llibITKIOLSM-4.9 \
              -llibITKIOMesh-4.9 \
              -llibITKIOMeta-4.9 \
              -llibITKIOMRC-4.9 \
              -llibITKIONIFTI-4.9 \
              -llibITKIONRRD-4.9 \
              -llibITKIOPNG-4.9 \
              -llibITKIOSiemens-4.9 \
              -llibITKIOSpatialObjects-4.9 \
              -llibITKIOStimulate-4.9 \
              -llibITKIOTIFF-4.9 \
              -llibITKIOTransformBase-4.9 \
              -llibITKIOTransformHDF5-4.9 \
              -llibITKIOTransformInsightLegacy-4.9 \
              -llibITKIOTransformMatlab-4.9 \
              -llibITKIOVTK-4.9 \
              -llibITKIOXML-4.9 \
              -llibitkjpeg-4.9 \
              -llibITKKLMRegionGrowing-4.9 \
              -llibITKLabelMap-4.9 \
              -llibITKMesh-4.9 \
              -llibITKMetaIO-4.9 \
              -llibitkNetlibSlatec-4.9 \
              -llibITKniftiio-4.9 \
              -llibITKNrrdIO-4.9 \
              -llibITKOptimizers-4.9 \
              -llibITKOptimizersv4-4.9 \
              -llibITKPath-4.9 \
              -llibitkpng-4.9 \
              -llibITKPolynomials-4.9 \
              -llibITKQuadEdgeMesh-4.9 \
              -llibITKSpatialObjects-4.9 \
              -llibITKStatistics-4.9 \
              -llibitksys-4.9 \
              -llibitktiff-4.9 \
              -llibitkv3p_lsqr-4.9 \
              -llibitkv3p_netlib-4.9 \
              -llibitkvcl-4.9 \
              -llibITKVideoCore-4.9 \
              -llibITKVideoIO-4.9 \
              -llibitkvnl-4.9 \
              -llibITKVNLInstantiation-4.9 \
              -llibitkvnl_algo-4.9 \
              -llibITKVTK-4.9 \
              -llibITKVtkGlue-4.9 \
              -llibITKWatersheds-4.9 \
              -llibitkzlib-4.9 \
              -llibITKznz-4.9


INCLUDEPATH += $$PWD/../../Tools/itk/include/ITK-4.9 \
               $$PWD/../Settings
DEPENDPATH += $$PWD/../../Tools/itk/include/ITK-4.9
