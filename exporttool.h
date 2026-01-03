#ifndef EXPORTTOOL_H
#define EXPORTTOOL_H

#include <QObject>
#include <QList>
// 仅依赖Qt公共头文件+项目自身的DataManager（无任何外部依赖）
#include "DataManager.h"

class ExportTool : public QObject
{
    Q_OBJECT
public:
    explicit ExportTool(QObject *parent = nullptr);

    // 生成Excel兼容文件（纯Qt公共API，CSV格式，双击无乱码）
    bool exportToExcel(const QList<Task>& tasks, const QString& filePath);
    // PDF导出（纯Qt公共API）
    bool exportToPdf(const QList<Task>& tasks, const QString& filePath);
};

#endif // EXPORTTOOL_H
