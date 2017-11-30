#include <QCoreApplication>
#include "Settings.h"
#include "Metadata.h"
#include "Mission.h"

//////////////////////////////////////////////////////////////////////////////
/// \brief main This is only for testing Metadata class.
/// \param argc
/// \param argv
/// \return
///
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //ierom::settings::Settings settings;

    //QString metadataJsonFileName = settings.processInfo.detector.metadataJsonFileName;
    //ierom::Mission mission;
    ierom::Metadata metadata;//mission.rawImageDirectory, metadataJsonFileName);
    //bool res = metadata.loadMetadata();

    if(metadata.isLoaded) {
        qDebug("Tissue Sameple - ID: %s", qPrintable(metadata.tissueSample.id));
        qDebug("Sectioning - Knife Edge: %s", qPrintable(metadata.sectioning.knifeEdge));
    }
    else {
        qDebug("Failed to load a metadata.");
    }
    return true; //res; //a.exec();
}
