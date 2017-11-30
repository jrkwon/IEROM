#-------------------------------------------------
#
# Project created by QtCreator 2015-11-05T11:31:17
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = TissueExtraction
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    main.cpp

unix|win32: LIBS += -L$$PWD/../../../../../../../../BI2STools/ITK/ITKVTKLib/lib\
        -llibITKBiasCorrection-4.8\
        -llibITKBioCell-4.8\
        -llibITKCommon-4.8\
        -llibITKDICOMParser-4.8\
        -llibitkdouble-conversion-4.8\
        -llibITKEXPAT-4.8\
        -llibITKFEM-4.8\
        -llibitkgdcmcharls-4.8\
        -llibitkgdcmCommon-4.8\
        -llibitkgdcmDICT-4.8\
        -llibitkgdcmDSED-4.8\
        -llibitkgdcmIOD-4.8\
        -llibitkgdcmjpeg12-4.8\
        -llibitkgdcmjpeg16-4.8\
        -llibitkgdcmjpeg8-4.8\
        -llibitkgdcmMEXD-4.8\
        -llibitkgdcmMSFF-4.8\
        -llibitkgdcmopenjpeg-4.8\
        -llibitkgdcmsocketxx-4.8\
        -llibITKgiftiio-4.8\
        -llibitkhdf5-4.8\
        -llibitkhdf5_cpp-4.8\
        -llibITKIOBioRad-4.8\
        -llibITKIOBMP-4.8\
        -llibITKIOCSV-4.8\
        -llibITKIOGDCM-4.8\
        -llibITKIOGE-4.8\
        -llibITKIOGIPL-4.8\
        -llibITKIOHDF5-4.8\
        -llibITKIOImageBase-4.8\
        -llibITKIOIPL-4.8\
        -llibITKIOJPEG-4.8\
        -llibITKIOLSM-4.8\
        -llibITKIOMesh-4.8\
        -llibITKIOMeta-4.8\
        -llibITKIOMRC-4.8\
        -llibITKIONIFTI-4.8\
        -llibITKIONRRD-4.8\
        -llibITKIOPNG-4.8\
        -llibITKIOSiemens-4.8\
        -llibITKIOSpatialObjects-4.8\
        -llibITKIOStimulate-4.8\
        -llibITKIOTIFF-4.8\
        -llibITKIOTransformBase-4.8\
        -llibITKIOTransformHDF5-4.8\
        -llibITKIOTransformInsightLegacy-4.8\
        -llibITKIOTransformMatlab-4.8\
        -llibITKIOVTK-4.8\
        -llibITKIOXML-4.8\
        -llibitkjpeg-4.8\
        -llibITKKLMRegionGrowing-4.8\
        -llibITKLabelMap-4.8\
        -llibITKMesh-4.8\
        -llibITKMetaIO-4.8\
        -llibitkNetlibSlatec-4.8\
        -llibITKniftiio-4.8\
        -llibITKNrrdIO-4.8\
        -llibITKOptimizers-4.8\
        -llibITKOptimizersv4-4.8\
        -llibITKPath-4.8\
        -llibitkpng-4.8\
        -llibITKPolynomials-4.8\
        -llibITKQuadEdgeMesh-4.8\
        -llibITKSpatialObjects-4.8\
        -llibITKStatistics-4.8\
        -llibitksys-4.8\
        -llibitktiff-4.8\
        -llibitkv3p_lsqr-4.8\
        -llibitkv3p_netlib-4.8\
        -llibitkvcl-4.8\
        -llibITKVideoCore-4.8\
        -llibITKVideoIO-4.8\
        -llibitkvnl-4.8\
        -llibITKVNLInstantiation-4.8\
        -llibitkvnl_algo-4.8\
        -llibITKVTK-4.8\
        -llibITKVtkGlue-4.8\
        -llibITKWatersheds-4.8\
        -llibitkzlib-4.8\
        -llibITKznz-4.8


INCLUDEPATH += $$PWD/../../../../../../../../BI2STools/ITK/ITKVTKLib/include/ITK-4.8
DEPENDPATH += $$PWD/../../../../../../../../BI2STools/ITK/ITKVTKLib/include/ITK-4.8

unix|win32: LIBS += -L$$PWD/../../../../../../../../BI2STools/OpenCV/Install/x86/mingw/lib\
                                 -llibopencv_calib3d300\
                                 -llibopencv_calib3d300\
                                 -llibopencv_core300\
                                 -llibopencv_features2d300\
                                 -llibopencv_flann300\
                                 -llibopencv_imgcodecs300\
                                 -llibopencv_imgproc300\
                                 -llibopencv_ml300\
                                 -llibopencv_objdetect300\
                                 -llibopencv_photo300\
                                 -llibopencv_shape300\
                                 -llibopencv_stitching300\
                                 -llibopencv_superres300\
                                 -llibopencv_video300\
                                 -llibopencv_videostab300\
                                 -llibopencv_viz300

INCLUDEPATH += $$PWD/../../../../../../../../BI2STools/OpenCV/Install/include
DEPENDPATH += $$PWD/../../../../../../../../BI2STools/OpenCV/Install/include
INCLUDEPATH += $$PWD/../../../../../../../../BI2STools/OpenCV/Install/include/opencv
DEPENDPATH += $$PWD/../../../../../../../../BI2STools/OpenCV/Install/include/opencv2
INCLUDEPATH += $$PWD/../../../../../../../../BI2STools/OpenCV/Install/include/opencv
DEPENDPATH += $$PWD/../../../../../../../../BI2STools/OpenCV/Install/include/opencv2

unix|win32: LIBS += -L$$PWD/../../../../../../../../BI2STools/ITK/ITKVTKOpenCVLib/lib/ -llibITKVideoBridgeOpenCV-4.8

INCLUDEPATH += $$PWD/../../../../../../../../BI2STools/ITK/ITKVTKOpenCVLib/include/ITK-4.8
DEPENDPATH += $$PWD/../../../../../../../../BI2STools/ITK/ITKVTKOpenCVLib/include/ITK-4.8
