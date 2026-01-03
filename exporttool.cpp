#include "ExportTool.h"
// 仅引入Qt自带公共头文件（无任何外部/私有头文件）
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QPdfWriter>
#include <QPainter>
#include <QFont>
#include <QDir>
#include <QPageSize>

ExportTool::ExportTool(QObject *parent) : QObject(parent)
{
}

// ---------------------- 纯Qt公共API生成Excel兼容文件（解决setCodec报错） ----------------------
bool ExportTool::exportToExcel(const QList<Task>& tasks, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug() << "文件打开失败：" << file.errorString();
        return false;
    }

    QTextStream out(&file);
    // UTF-8 BOM头（解决乱码）
    out << "\xEF\xBB\xBF";

    // 生成HTML表格（Excel原生识别为结构化表格）
    out << "<html>\n";
    out << "<head><meta charset=\"UTF-8\"></head>\n";
    out << "<body>\n";
    out << "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\">\n"; // 带边框的表格

    // 表头行（<th>标签，Excel识别为表头）
    out << "<tr>\n";
    out << "<th>ID</th>\n";
    out << "<th>任务名称</th>\n";
    out << "<th>分类</th>\n";
    out << "<th>优先级</th>\n";
    out << "<th>截止时间</th>\n";
    out << "<th>完成状态</th>\n";
    out << "</tr>\n";

    // 数据行（<td>标签，Excel识别为单元格）
    foreach (const Task& task, tasks) {
        QString priorityStr = task.priority == 3 ? "高" : (task.priority == 2 ? "中" : "低");
        QString completedStr = task.isCompleted ? "已完成" : "未完成";

        out << "<tr>\n";
        out << QString("<td>%1</td>\n").arg(task.id);
        out << QString("<td>%1</td>\n").arg(task.name);
        out << QString("<td>%1</td>\n").arg(task.category);
        out << QString("<td>%1</td>\n").arg(priorityStr);
        out << QString("<td>%1</td>\n").arg(task.deadline.toString("yyyy-MM-dd HH:mm:ss"));
        out << QString("<td>%1</td>\n").arg(completedStr);
        out << "</tr>\n";
    }

    out << "</table>\n";
    out << "</body>\n";
    out << "</html>\n";

    file.close();
    return true;
}

// ---------------------- PDF导出（纯Qt公共API，兼容Qt5/Qt6） ----------------------
bool ExportTool::exportToPdf(const QList<Task>& tasks, const QString &filePath)
{
    QPdfWriter pdfWriter(filePath);
    // Qt5/Qt6兼容的A4设置
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setResolution(150);
    pdfWriter.setTitle("任务列表");

    QPainter painter(&pdfWriter);
    if (!painter.isActive()) {
        qDebug() << "PDF创建失败：" << filePath;
        return false;
    }

    // 中文字体（兼容Windows）
    QFont font("SimSun", 10);
    painter.setFont(font);

    // 列宽优化：无重叠
    int xStart = 50, yStart = 50, rowHeight = 25;
    int colWidths[] = {50, 200, 100, 80, 250, 120};

    // 绘制表头
    int x = xStart;
    painter.setPen(Qt::black);
    QString headers[] = {"ID", "任务名称", "分类", "优先级", "截止时间", "完成状态"};
    for (int i = 0; i < 6; i++) {
        painter.drawText(x + 5, yStart + rowHeight/2, headers[i]);
        x += colWidths[i];
    }
    painter.drawLine(xStart, yStart + rowHeight, xStart + 750, yStart + rowHeight);

    // 绘制数据
    int y = yStart + rowHeight + 10, pageRowCount = 0;
    const int maxRowPerPage = 25;
    foreach (const Task& task, tasks) {
        if (pageRowCount >= maxRowPerPage) {
            pdfWriter.newPage();
            y = yStart; x = xStart;
            painter.drawLine(xStart, y + rowHeight, xStart + 750, y + rowHeight);
            for (int i = 0; i < 6; i++) {
                painter.drawText(x + 5, y + rowHeight/2, headers[i]);
                x += colWidths[i];
            }
            y = yStart + rowHeight + 10;
            pageRowCount = 0;
        }

        x = xStart;
        QString priorityStr = task.priority == 3 ? "高" : (task.priority == 2 ? "中" : "低");
        QString completedStr = task.isCompleted ? "已完成" : "未完成";
        painter.drawText(x + 5, y + rowHeight/2, QString::number(task.id)); x += colWidths[0];
        painter.drawText(x + 5, y + rowHeight/2, task.name); x += colWidths[1];
        painter.drawText(x + 5, y + rowHeight/2, task.category); x += colWidths[2];
        painter.drawText(x + 5, y + rowHeight/2, priorityStr); x += colWidths[3];
        painter.drawText(x + 5, y + rowHeight/2, task.deadline.toString("yyyy-MM-dd HH:mm:ss")); x += colWidths[4];
        painter.drawText(x + 5, y + rowHeight/2, completedStr);

        y += rowHeight;
        pageRowCount++;
    }

    painter.end();
    return true;
}
