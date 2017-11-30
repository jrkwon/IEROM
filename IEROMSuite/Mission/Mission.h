#ifndef MISSION_H
#define MISSION_H

#include <QJsonObject>
#include "Settings.h"

IEROM_NAMESPACE_START

#if kDebug
#define kMissionJsonFileRelPath     "/Mission/Mission.json"
#else
#define kMissionJsonFileRelPath     "/bin/Mission.json"
#endif

class Mission
{
public:
    Mission();

private:
    void read(const QJsonObject &json);

public:
    QString rawImageDirectory;
    QString processedImageDirectory;
};

IEROM_NAMESPACE_END

#endif // MISSION_H
