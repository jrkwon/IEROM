#include <QProcessEnvironment>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>

#include "Mission.h"

IEROM_NAMESPACE_START

Mission::Mission()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString suiteDir = env.value(kIEROMSuitePathEnv);
    QString jsonFilePath = QDir::cleanPath(suiteDir + kMissionJsonFileRelPath);
    //qDebug() << jsonFilePath;

    QFile missionFile(jsonFilePath);
    if(!missionFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open the json file.");
        return;
    }
    QByteArray missionByteArray = missionFile.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(missionByteArray);
    QJsonObject json = jsonDoc.object();

    read(json);
}

void Mission::read(const QJsonObject &json)
{
    QJsonObject missionJsonObj = json["Mission"].toObject();

    this->rawImageDirectory = missionJsonObj["Raw Image Directory"].toString();
    this->processedImageDirectory = missionJsonObj["Processed Image Direcotry"].toString();
}

IEROM_NAMESPACE_END
