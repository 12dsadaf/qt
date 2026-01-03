#ifndef MINIEXCELQT_H
#define MINIEXCELQT_H

#include <QString>
#include <QVariant>
#include <QList>
#include <QMap>
#include <QDateTime>

class MiniExcelQtPrivate;

class MiniExcelQt
{
public:
    MiniExcelQt();
    ~MiniExcelQt();

    bool write(int row, int col, const QVariant &value);
    bool write(const QString &cell, const QVariant &value);

    QVariant read(int row, int col) const;
    QVariant read(const QString &cell) const;

    bool renameSheet(int index, const QString &name);
    bool renameSheet(const QString &oldName, const QString &newName);

    bool saveAs(const QString &filePath);

private:
    MiniExcelQtPrivate *d;
};

#endif // MINIEXCELQT_H
