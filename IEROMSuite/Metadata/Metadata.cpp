#include <QDir>
#include <QJsonDocument>

#include "Metadata.h"
#include "Mission.h"

IEROM_NAMESPACE_START

void TissueSample::read(const QJsonObject &json)
{
    this->id = json["ID"].toString();
    this->specimen = json["Specimen"].toString();
    this->organ = json["Organ"].toString();
    this->stain = json["Stain"].toString();
    this->object = json["Object"].toString();
    this->sectioningPlane = json["Sectioning Plane"].toString();
    this->scope = json["Scope"].toString();
    QJsonObject acqDateJsonObj = json["Acquisition Date"].toObject();
    this->startEndDate.start = acqDateJsonObj["Start"].toString();
    this->startEndDate.end = acqDateJsonObj["End"].toString();
    QJsonObject sizeJsonObject = json["Size"].toObject();
    this->size.width = sizeJsonObject["Width"].toInt();
    this->size.depth = sizeJsonObject["Depth"].toInt();
    this->size.height = sizeJsonObject["Height"].toInt();
}

void Sectioning::read(const QJsonObject &json)
{
    this->knifeEdge = json["Knife Edge"].toString();
    QJsonObject vrJsonObj = json["Voxel Resol"].toObject();
    this->voxelResol.width = vrJsonObj["Width"].toDouble();
    this->voxelResol.depth = vrJsonObj["Depth"].toDouble();
    this->voxelResol.height = vrJsonObj["Height"].toDouble();
    QJsonObject ssJsonObj = json["Slice Size"].toObject();
    this->sliceSize.width = ssJsonObj["Width"].toInt();
    this->sliceSize.height = ssJsonObj["Height"].toInt();
    QJsonObject coJsonObj = json["Columns"].toObject();
    this->columns.start = coJsonObj["Start"].toInt();
    this->columns.end = coJsonObj["End"].toInt();
    QJsonObject rzJsonObj = json["RangeZ"].toObject();
    this->rangeZ.minZ = rzJsonObj["MinZ"].toDouble();
    this->rangeZ.maxZ = rzJsonObj["MaxZ"].toDouble();
}

Metadata::Metadata()
{
    settings::Settings settings;
    Mission mission;

    this->basePath = mission.rawImageDirectory;
    this->metadataJsonFileName = settings.processInfo.detector.metadataJsonFileName;

    isLoaded = loadMetadata();
}

bool Metadata::loadMetadata()
{
    QString jsonFilePath = QDir::cleanPath(this->basePath + "/" + this->metadataJsonFileName);
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

void Metadata::read(const QJsonObject &json)
{
    this->imageFileExtension = json["Image File Extension"].toString();
    QJsonObject tsJson = json["Tissue Sample"].toObject();
    this->tissueSample.read(tsJson);

    QJsonObject secJson = json["Sectioning"].toObject();
    this->sectioning.read(secJson);
}

IEROM_NAMESPACE_END
