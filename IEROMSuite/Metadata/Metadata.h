#ifndef METADATA_H
#define METADATA_H

#include <QDebug>
#include <QString>
#include <QDate>
#include <QJsonObject>

#include "Settings.h"

IEROM_NAMESPACE_START

struct StartEndDate
{
    QString start;
    QString end;
};

struct Size
{
    int width;
    int depth;
    int height;
};

class TissueSample
{
public:
    void read(const QJsonObject &json);

public:
    QString id;
    QString specimen;
    QString organ;
    QString stain;
    QString object;
    QString sectioningPlane;
    QString scope;
    StartEndDate startEndDate;
    Size size;
};

struct VoxelResol
{
    double width;
    double depth;
    double height;
};

struct SliceSize
{
    int width;
    int height;
};

struct Columns
{
    int start;
    int end;
};

struct RangeZ
{
    double minZ;
    double maxZ;
};

class Sectioning
{
public:
    void read(const QJsonObject &json);

public:
    QString knifeEdge;
    VoxelResol voxelResol;
    SliceSize sliceSize;
    Columns columns;
    RangeZ rangeZ;
};

class Metadata
{
public:
    Metadata();

private:
    bool loadMetadata();
    void read(const QJsonObject &json);

public:
    bool isLoaded;

    QString basePath; // where the Metadata.json is
    QString metadataJsonFileName;

    // From json file
    QString imageFileExtension;
    TissueSample tissueSample;
    Sectioning sectioning;
};

IEROM_NAMESPACE_END

#endif // METADATA_H
