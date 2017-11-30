#ifndef SLICEAREAPOSITIONWRITER_H
#define SLICEAREAPOSITIONWRITER_H

#include "Settings.h"
#include <QString>
#include <QPoint>
#include <QJsonObject>

IEROM_NAMESPACE_START

class SliceAreaInfo
{
public:
    QString fileName;
    QPoint sliceArea;
    //bool valid;

public:
    void write(QJsonObject &json) const;
};

class SliceAreaPositionWriter
{
public:
    SliceAreaPositionWriter(QString textFileName, QString jsonFileName);

    bool write();
    bool writeJson();

public:
    QVector<SliceAreaInfo> sliceAreas;
    QString textFileName;
    QString jsonFileName;
//    int numberOfSlicesFromExistingFile;
};

IEROM_NAMESPACE_END

#endif // SLICEAREAPOSITIONWRITER_H
