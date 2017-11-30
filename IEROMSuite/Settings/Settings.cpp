#include "Settings.h"
#include <QProcessEnvironment>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>

IEROM_NAMESPACE_START

SETTINGS_NAMESPACE_START

void SystemInfo::read(const QJsonObject &json)
{
    author = json["Author"].toString();
    QJsonObject jsonVersion = json["Version"].toObject();
    version.major =  jsonVersion["Major"].toInt();
    version.minor =  jsonVersion["Minor"].toInt();
}

void RawImageProp::read(const QJsonObject &json)
{
    //sourceImageFileExtension = json["SourceImageFileExtension"].toString();
    backgroundColor = json["BackgroundColor"].toInt();
    minPixelIntensity = json["MinPixelIntensity"].toInt();
    maxPixelIntensity = json["MaxPixelIntensity"].toInt();
}

void DirectoryName::read(const QJsonObject &json)
{
    detected = json["Detected"].toString();
    cropped = json["Cropped"].toString();
    normalized = json["Normalized"].toString();
    sticherInfo = json["SticherInfo"].toString();
    stiched = json["Stiched"].toString();
    titled = json["Tiled"].toString();
    scaled = json["Scaled"].toString();
    rotated = json["Rotated"].toString();
    attenuated = json["Attenuated"].toString();
}

void ProcessInfo::read(const QJsonObject &json)
{
    maxProcessWaitTime = json["MaxProcessWaitTime"].toInt();
    globalSharedMemoryKey = json["GlobalSharedMemoryKey"].toString();
    processExt = json["ProcessExt"].toString();
    QJsonObject jsonDebugRelease;

    jsonDebugRelease = (kDebug) ? json["Debug"].toObject() : json["Release"].toObject();
    partialPath = jsonDebugRelease["PartialPath"].toString();
    pathPrefix = jsonDebugRelease["PathPrefix"].toString();

    modelMaker.read(json["Model3DMaker"].toObject());
    volumemaker.read(json["VolumeMaker"].toObject());
    detector.read(json["SliceAreaDetector"].toObject());
    corrector.read(json["SliceAreaCorrector"].toObject());
    cropper.read(json["SliceAreaCropper"].toObject());
    normalizer.read(json["IntensityNormalizer"].toObject());
    sticherInfo.read(json["SliceSticherInfo"].toObject());
    sticher.read(json["SliceSticher"].toObject());
    composer.read(json["MultiLayerComposer"].toObject());
}

void Model3DMaker::read(const QJsonObject &json)
{
    QJsonObject versionJson = json["Version"].toObject();
    version.major = versionJson["Major"].toInt();
    version.minor = versionJson["Minor"].toInt();
    name = json["Name"].toString();

    unitVolsDirectoryName = json["UnitVolumesDirectoryName"].toString();
    stlsDirectoryName     = json["STLMeshesDirectoryName"].toString();
    tiffsDirectoryName    = json["MultiPageTIFFsDirectoryName"].toString();
    vtksDirectoryName     = json["VTKFilesDirectoryName"].toString();
}

void VolumeMaker::read(const QJsonObject &json)
{
    QJsonObject versionJson = json["Version"].toObject();
    version.major = versionJson["Major"].toInt();
    version.minor = versionJson["Minor"].toInt();
    name = json["Name"].toString();

    imageDirectoryName = json["ImageDirectoryName"].toString();
    unitVolsDirectoryName = json["UnitVolumeDirectoryName"].toString();
}

void SliceAreaDetector::read(const QJsonObject &json)
{
    QJsonObject versionJson = json["Version"].toObject();
    version.major = versionJson["Major"].toInt();
    version.minor = versionJson["Minor"].toInt();
    name = json["Name"].toString();

    metadataJsonFileName = json["MetadataJsonFileName"].toString();
    templateFileName = json["TemplateFileName"].toString();
    columNumberDigits = json["ColumnNumberDigits"].toInt();
    rightEdgeWindowSize = json["RightEdgeWindowSize"].toInt();
}

void SliceAreaCorrector::read(const QJsonObject &json)
{
    QJsonObject versionJson = json["Version"].toObject();
    version.major = versionJson["Major"].toInt();
    version.minor = versionJson["Minor"].toInt();
    name = json["Name"].toString();

    imageChuckThreshold = json["ImageChuckThreshold"].toInt();
    maxRightEdgeDifference = json["MaxRightEdgeDifference"].toInt();
    maxConsecutiveWrongImages = json["MaxConsecutiveWrongImages"].toInt();
    defaultFactorForWindowSize = json["DefaultFactorForWindowSize"].toInt();
    maxOutlierImages = json["MaxOutlierImages"].toInt();
}

void SliceAreaCropper::read(const QJsonObject &json)
{
    QJsonObject versionJson = json["Version"].toObject();
    version.major = versionJson["Major"].toInt();
    version.minor = versionJson["Minor"].toInt();
    name = json["Name"].toString();
}

void IntensityNormalizer::read(const QJsonObject &json)
{
    QJsonObject versionJson = json["Version"].toObject();
    version.major = versionJson["Major"].toInt();
    version.minor = versionJson["Minor"].toInt();
    name = json["Name"].toString();

    normalizedMinIntensity = json["NormalizedMinIntensity"].toInt();
    normalizedMaxIntensity = json["NormalizedMaxIntensity"].toInt();
}

void SliceSticherInfo::read(const QJsonObject &json)
{
    QJsonObject versionJson = json["Version"].toObject();
    version.major = versionJson["Major"].toInt();
    version.minor = versionJson["Minor"].toInt();
    name = json["Name"].toString();

}

void SliceSticher::read(const QJsonObject &json)
{
    QJsonObject versionJson = json["Version"].toObject();
    version.major = versionJson["Major"].toInt();
    version.minor = versionJson["Minor"].toInt();
    name = json["Name"].toString();

    infoFileName = json["InfoFileName"].toString();
    infoFormat = json["InfoFormat"].toString();
    invertImageFlag = json["InvertImageFlag"].toString();
}

void MultiLayerComposer::read(const QJsonObject &json)
{
    QJsonObject versionJson = json["Version"].toObject();
    version.major = versionJson["Major"].toInt();
    version.minor = versionJson["Minor"].toInt();
    name = json["Name"].toString();

    indexedImageFileNameFormat = json["IndexedImageFileNameFormat"].toString();
    zPosFileNameFormat = json["ZPosFileNameFormat"].toString();
    composerUnitDepth = json["ComposerUnitDepth"].toInt();
}

Settings::Settings()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    this->suiteDir = QDir::cleanPath(env.value(kIEROMSuitePathEnv));

    this->isLoaded = loadSettings();
}

bool Settings::loadSettings()
{
//    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
//    QString suiteDir = env.value(kIEROMSuitePathEnv);
    QString jsonFilePath = QDir::cleanPath(this->suiteDir + kSettingsJsonFileRelPath);
    //qDebug() << jsonFilePath;

    QFile settingsFile(jsonFilePath);
    if(!settingsFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open the json file.");
        return false;
    }
    QByteArray settingsByteArray = settingsFile.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(settingsByteArray);
    QJsonObject json = jsonDoc.object();

    read(json);

    return true;
}

void Settings::read(const QJsonObject &json)
{
    QJsonObject ierom = json["IEROM"].toObject();
    systemInfo.read(ierom["SystemInfo"].toObject());
    rawImageProp.read(ierom["RawImageProp"].toObject());
    directoryName.read(ierom["DirectoryName"].toObject());
    processInfo.read(ierom["ProcessInfo"].toObject());
}


SETTINGS_NAMESPACE_END

IEROM_NAMESPACE_END
