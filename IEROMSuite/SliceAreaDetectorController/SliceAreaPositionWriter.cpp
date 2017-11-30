#include "SliceAreaPositionWriter.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QVectorIterator>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

IEROM_NAMESPACE_START

void SliceAreaInfo::write(QJsonObject &json) const
{
    json["Name"] = this->fileName;
    json["StartX"] = this->sliceArea.x();
    json["StartY"] = this->sliceArea.y();
}

SliceAreaPositionWriter::SliceAreaPositionWriter(QString textFileName, QString jsonFileName)
{
    this->textFileName = textFileName;
    this->jsonFileName = jsonFileName;
//    this->numberOfSlicesFromExistingFile = 0;

    SliceAreaInfo sliceAreaInfo;
    QFile file(this->textFileName);
    if(file.exists()) {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        in.readLine(); // first line - header
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList items = line.split('\t');
            if(items.size() < 3)
                break;
            sliceAreaInfo.fileName = items[0];
            sliceAreaInfo.sliceArea.setX(items[1].toInt());
            sliceAreaInfo.sliceArea.setY(items[2].toInt());
            this->sliceAreas.append(sliceAreaInfo);
//            this->numberOfSlicesFromExistingFile++;
        }
        file.close();
    }
}

bool SliceAreaPositionWriter::write()
{
    QFile file(this->textFileName);
    QTextStream out(&file);

    if(!file.exists()) {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error: Cannot write file "
                     << qPrintable(textFileName) << ": "
                     << qPrintable(file.errorString()) << "\n";
            return false;
        }
        out << "name" << "\t" << "startx" << "\t" << "starty" << "\n";
    }
    else {
        if(!file.open(QIODevice::Append | QIODevice::Text)) {
            qDebug() << "Error: Cannot open file for appending "
                     << qPrintable(textFileName) << ": "
                     << qPrintable(file.errorString()) << "\n";
            return false;
        }
    }

    SliceAreaInfo lastItem = sliceAreas.last();

    out << lastItem.fileName << "\t"
        << lastItem.sliceArea.x() << "\t"
        << lastItem.sliceArea.y() << "\n";

//    for (int i = 0; i < sliceAreas.size(); i++) {
//        out << sliceAreas[i].fileName << "\t"
//            << sliceAreas[i].sliceArea.x() << "\t"
//            << sliceAreas[i].sliceArea.y() << "\n";
//    }

    file.close();
    return true;
}

bool SliceAreaPositionWriter::writeJson()
{
    QFile file(this->jsonFileName);

    // Always rewritten since wirteJson executes only once
    // when the slice detection task completes
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: Cannot open file for appending "
                 << qPrintable(jsonFileName) << ": "
                 << qPrintable(file.errorString()) << "\n";
        return false;
    }

    QJsonObject json;
    QJsonArray jsonArray;

    foreach(const SliceAreaInfo area, sliceAreas) {
        QJsonObject obj;
        area.write(obj);
        jsonArray.append(obj);
    }
    json["Slice Areas"] = jsonArray;

    QJsonDocument doc(json);
    file.write(doc.toJson());

//    file.close();
    return true;
}

IEROM_NAMESPACE_END
