QT += core
QT -= gui

TARGET = 3DModelMaker
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    model3dmaker.cpp

HEADERS += \
    model3dmaker.h

#------
# vtk
#------
win32: LIBS += -L$$PWD/../../Tools/vtk/lib/ -llibvtkalglib-6.3 \
                -llibvtkalglib-6.3 \
                -llibvtkChartsCore-6.3 \
                -llibvtkCommonColor-6.3 \
                -llibvtkCommonComputationalGeometry-6.3 \
                -llibvtkCommonCore-6.3 \
                -llibvtkCommonDataModel-6.3 \
                -llibvtkCommonExecutionModel-6.3 \
                -llibvtkCommonMath-6.3 \
                -llibvtkCommonMisc-6.3 \
                -llibvtkCommonSystem-6.3 \
                -llibvtkCommonTransforms-6.3 \
                -llibvtkDICOMParser-6.3 \
                -llibvtkDomainsChemistry-6.3 \
                -llibvtkexoIIc-6.3 \
                -llibvtkexpat-6.3 \
                -llibvtkFiltersAMR-6.3 \
                -llibvtkFiltersCore-6.3 \
                -llibvtkFiltersExtraction-6.3 \
                -llibvtkFiltersFlowPaths-6.3 \
                -llibvtkFiltersGeneral-6.3 \
                -llibvtkFiltersGeneric-6.3 \
                -llibvtkFiltersGeometry-6.3 \
                -llibvtkFiltersHybrid-6.3 \
                -llibvtkFiltersHyperTree-6.3 \
                -llibvtkFiltersImaging-6.3 \
                -llibvtkFiltersModeling-6.3 \
                -llibvtkFiltersParallel-6.3 \
                -llibvtkFiltersParallelImaging-6.3 \
                -llibvtkFiltersProgrammable-6.3 \
                -llibvtkFiltersSelection-6.3 \
                -llibvtkFiltersSMP-6.3 \
                -llibvtkFiltersSources-6.3 \
                -llibvtkFiltersStatistics-6.3 \
                -llibvtkFiltersTexture-6.3 \
                -llibvtkFiltersVerdict-6.3 \
                -llibvtkfreetype-6.3 \
                -llibvtkftgl-6.3 \
                -llibvtkGeovisCore-6.3 \
                -llibvtkgl2ps-6.3 \
                -llibvtkImagingColor-6.3 \
                -llibvtkImagingCore-6.3 \
                -llibvtkImagingFourier-6.3 \
                -llibvtkImagingGeneral-6.3 \
                -llibvtkImagingHybrid-6.3 \
                -llibvtkImagingMath-6.3 \
                -llibvtkImagingMorphological-6.3 \
                -llibvtkImagingSources-6.3 \
                -llibvtkImagingStatistics-6.3 \
                -llibvtkImagingStencil-6.3 \
                -llibvtkInfovisCore-6.3 \
                -llibvtkInfovisLayout-6.3 \
                -llibvtkInteractionImage-6.3 \
                -llibvtkInteractionStyle-6.3 \
                -llibvtkInteractionWidgets-6.3 \
                -llibvtkIOAMR-6.3 \
                -llibvtkIOCore-6.3 \
                -llibvtkIOEnSight-6.3 \
                -llibvtkIOExodus-6.3 \
                -llibvtkIOExport-6.3 \
                -llibvtkIOGeometry-6.3 \
                -llibvtkIOImage-6.3 \
                -llibvtkIOImport-6.3 \
                -llibvtkIOInfovis-6.3 \
                -llibvtkIOLegacy-6.3 \
                -llibvtkIOLSDyna-6.3 \
                -llibvtkIOMINC-6.3 \
                -llibvtkIOMovie-6.3 \
                -llibvtkIONetCDF-6.3 \
                -llibvtkIOParallel-6.3 \
                -llibvtkIOParallelXML-6.3 \
                -llibvtkIOPLY-6.3 \
                -llibvtkIOSQL-6.3 \
                -llibvtkIOVideo-6.3 \
                -llibvtkIOXML-6.3 \
                -llibvtkIOXMLParser-6.3 \
                -llibvtkjpeg-6.3 \
                -llibvtkjsoncpp-6.3 \
                -llibvtklibxml2-6.3 \
                -llibvtkmetaio-6.3 \
                -llibvtkNetCDF-6.3 \
                -llibvtkNetCDF_cxx-6.3 \
                -llibvtkoggtheora-6.3 \
                -llibvtkParallelCore-6.3 \
                -llibvtkpng-6.3 \
                -llibvtkproj4-6.3 \
                -llibvtkRenderingAnnotation-6.3 \
                -llibvtkRenderingContext2D-6.3 \
                -llibvtkRenderingContextOpenGL-6.3 \
                -llibvtkRenderingCore-6.3 \
                -llibvtkRenderingFreeType-6.3 \
                -llibvtkRenderingGL2PS-6.3 \
                -llibvtkRenderingImage-6.3 \
                -llibvtkRenderingLabel-6.3 \
                -llibvtkRenderingLIC-6.3 \
                -llibvtkRenderingLOD-6.3 \
                -llibvtkRenderingOpenGL-6.3 \
                -llibvtkRenderingVolume-6.3 \
                -llibvtkRenderingVolumeOpenGL-6.3 \
                -llibvtksys-6.3 \
                -llibvtktiff-6.3 \
                -llibvtkverdict-6.3 \
                -llibvtkViewsContext2D-6.3 \
                -llibvtkViewsCore-6.3 \
                -llibvtkViewsInfovis-6.3 \
                -llibvtkzlib-6.3 \

INCLUDEPATH += $$PWD/../../Tools/vtk/include/vtk-6.3
DEPENDPATH += $$PWD/../../Tools/vtk/include/vtk-6.3



#-----
# itk
#-----
win32: LIBS += -L$$PWD/../../Tools/itk/lib/ \
              -llibITKBiasCorrection-4.8 \
              -llibITKBioCell-4.8 \
              -llibITKCommon-4.8 \
              -llibITKDICOMParser-4.8 \
              -llibitkdouble-conversion-4.8 \
              -llibITKEXPAT-4.8 \
              -llibITKFEM-4.8 \
              -llibitkgdcmcharls-4.8 \
              -llibitkgdcmCommon-4.8 \
              -llibitkgdcmDICT-4.8 \
              -llibitkgdcmDSED-4.8 \
              -llibitkgdcmIOD-4.8 \
              -llibitkgdcmjpeg12-4.8 \
              -llibitkgdcmjpeg16-4.8 \
              -llibitkgdcmjpeg8-4.8 \
              -llibitkgdcmMEXD-4.8 \
              -llibitkgdcmMSFF-4.8 \
              -llibitkgdcmopenjpeg-4.8 \
              -llibitkgdcmsocketxx-4.8 \
              -llibITKgiftiio-4.8 \
              -llibitkhdf5-4.8 \
              -llibitkhdf5_cpp-4.8 \
              -llibITKIOBioRad-4.8 \
              -llibITKIOBMP-4.8 \
              -llibITKIOCSV-4.8 \
              -llibITKIOGDCM-4.8 \
              -llibITKIOGE-4.8 \
              -llibITKIOGIPL-4.8 \
              -llibITKIOHDF5-4.8 \
              -llibITKIOImageBase-4.8 \
              -llibITKIOIPL-4.8 \
              -llibITKIOJPEG-4.8 \
              -llibITKIOLSM-4.8 \
              -llibITKIOMesh-4.8 \
              -llibITKIOMeta-4.8 \
              -llibITKIOMRC-4.8 \
              -llibITKIONIFTI-4.8 \
              -llibITKIONRRD-4.8 \
              -llibITKIOPNG-4.8 \
              -llibITKIOSiemens-4.8 \
              -llibITKIOSpatialObjects-4.8 \
              -llibITKIOStimulate-4.8 \
              -llibITKIOTIFF-4.8 \
              -llibITKIOTransformBase-4.8 \
              -llibITKIOTransformHDF5-4.8 \
              -llibITKIOTransformInsightLegacy-4.8 \
              -llibITKIOTransformMatlab-4.8 \
              -llibITKIOVTK-4.8 \
              -llibITKIOXML-4.8 \
              -llibitkjpeg-4.8 \
              -llibITKKLMRegionGrowing-4.8 \
              -llibITKLabelMap-4.8 \
              -llibITKMesh-4.8 \
              -llibITKMetaIO-4.8 \
              -llibitkNetlibSlatec-4.8 \
              -llibITKniftiio-4.8 \
              -llibITKNrrdIO-4.8 \
              -llibITKOptimizers-4.8 \
              -llibITKOptimizersv4-4.8 \
              -llibITKPath-4.8 \
              -llibitkpng-4.8 \
              -llibITKPolynomials-4.8 \
              -llibITKQuadEdgeMesh-4.8 \
              -llibITKSpatialObjects-4.8 \
              -llibITKStatistics-4.8 \
              -llibitksys-4.8 \
              -llibitktiff-4.8 \
              -llibitkv3p_lsqr-4.8 \
              -llibitkv3p_netlib-4.8 \
              -llibitkvcl-4.8 \
              -llibITKVideoCore-4.8 \
              -llibITKVideoIO-4.8 \
              -llibitkvnl-4.8 \
              -llibITKVNLInstantiation-4.8 \
              -llibitkvnl_algo-4.8 \
              -llibITKVTK-4.8 \
              -llibITKVtkGlue-4.8 \
              -llibITKWatersheds-4.8 \
              -llibitkzlib-4.8 \
              -llibITKznz-4.8


INCLUDEPATH += $$PWD/../../Tools/itk/include/ITK-4.8 \
               $$PWD/../Settings
DEPENDPATH += $$PWD/../../Tools/itk/include/ITK-4.8



