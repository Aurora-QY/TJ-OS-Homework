/********************************************************************************
** Form generated from reading UI file 'filesystem.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILESYSTEM_H
#define UI_FILESYSTEM_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_fileSystem
{
public:
    QWidget *centralWidget;
    QPushButton *BackBtn;
    QPushButton *AddDirBtn;
    QPushButton *AddFileBtn;
    QPushButton *FormatBtn;
    QLabel *PathLabel;
    QLabel *BackLabel;
    QLabel *AddDirLabel;
    QLabel *AddFileLabel;
    QLabel *AddFileLabel_2;
    QTableWidget *tableWidget;
    QTableWidget *BitMap;
    QLabel *BackLabel_2;

    void setupUi(QMainWindow *fileSystem)
    {
        if (fileSystem->objectName().isEmpty())
            fileSystem->setObjectName(QString::fromUtf8("fileSystem"));
        fileSystem->resize(1000, 600);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(fileSystem->sizePolicy().hasHeightForWidth());
        fileSystem->setSizePolicy(sizePolicy);
        fileSystem->setStyleSheet(QString::fromUtf8(""));
        centralWidget = new QWidget(fileSystem);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setStyleSheet(QString::fromUtf8("background:rgb(255, 255, 255);"));
        BackBtn = new QPushButton(centralWidget);
        BackBtn->setObjectName(QString::fromUtf8("BackBtn"));
        BackBtn->setGeometry(QRect(20, 20, 61, 61));
        BackBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    border-image: url(:/icons/Back1.png);\n"
"}\n"
"QPushButton:hover {\n"
"    border-image: url(:/icons/Back2.png);\n"
"}\n"
"QPushButton:pressed {\n"
"    border-image: url(:/icons/Back3.png);\n"
"}"));
        AddDirBtn = new QPushButton(centralWidget);
        AddDirBtn->setObjectName(QString::fromUtf8("AddDirBtn"));
        AddDirBtn->setGeometry(QRect(500, 20, 61, 61));
        AddDirBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    border-image: url(:/icons/AddDir1.png);\n"
"}\n"
"QPushButton:hover {\n"
"    border-image: url(:/icons/AddDir2.png);\n"
"}\n"
"QPushButton:pressed {\n"
"    border-image: url(:/icons/AddDir3.png);\n"
"}"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/AddDir.png"), QSize(), QIcon::Normal, QIcon::Off);
        AddDirBtn->setIcon(icon);
        AddDirBtn->setIconSize(QSize(12, 12));
        AddFileBtn = new QPushButton(centralWidget);
        AddFileBtn->setObjectName(QString::fromUtf8("AddFileBtn"));
        AddFileBtn->setGeometry(QRect(600, 20, 61, 61));
        AddFileBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    border-image: url(:/icons/AddFile1.png);\n"
"}\n"
"QPushButton:hover {\n"
"    border-image: url(:/icons/AddFile2.png);\n"
"}\n"
"QPushButton:pressed {\n"
"    border-image: url(:/icons/AddFile3.png);\n"
"}"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/AddFile.png"), QSize(), QIcon::Normal, QIcon::Off);
        AddFileBtn->setIcon(icon1);
        AddFileBtn->setIconSize(QSize(12, 12));
        FormatBtn = new QPushButton(centralWidget);
        FormatBtn->setObjectName(QString::fromUtf8("FormatBtn"));
        FormatBtn->setGeometry(QRect(690, 20, 61, 61));
        FormatBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    border-image: url(:/icons/Format1.png);\n"
"}\n"
"QPushButton:hover {\n"
"    border-image: url(:/icons/Format2.png);\n"
"}\n"
"QPushButton:pressed {\n"
"    border-image: url(:/icons/Format3.png);\n"
"}"));
        PathLabel = new QLabel(centralWidget);
        PathLabel->setObjectName(QString::fromUtf8("PathLabel"));
        PathLabel->setGeometry(QRect(100, 10, 381, 31));
        PathLabel->setStyleSheet(QString::fromUtf8("background:rgb(255, 255, 255)rgb(241, 241, 241)rgb(241, 241, 241);\n"
"border:1px solid black;\n"
"border-radius: 10px;\n"
"font-weight: bold;"));
        BackLabel = new QLabel(centralWidget);
        BackLabel->setObjectName(QString::fromUtf8("BackLabel"));
        BackLabel->setGeometry(QRect(0, 0, 101, 21));
        AddDirLabel = new QLabel(centralWidget);
        AddDirLabel->setObjectName(QString::fromUtf8("AddDirLabel"));
        AddDirLabel->setGeometry(QRect(480, 0, 101, 21));
        AddFileLabel = new QLabel(centralWidget);
        AddFileLabel->setObjectName(QString::fromUtf8("AddFileLabel"));
        AddFileLabel->setGeometry(QRect(580, 0, 101, 21));
        AddFileLabel_2 = new QLabel(centralWidget);
        AddFileLabel_2->setObjectName(QString::fromUtf8("AddFileLabel_2"));
        AddFileLabel_2->setGeometry(QRect(670, 0, 101, 21));
        tableWidget = new QTableWidget(centralWidget);
        if (tableWidget->columnCount() < 4)
            tableWidget->setColumnCount(4);
        if (tableWidget->rowCount() < 4)
            tableWidget->setRowCount(4);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(20, 80, 500, 500));
        tableWidget->setContextMenuPolicy(Qt::NoContextMenu);
        tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setShowGrid(false);
        tableWidget->setRowCount(4);
        tableWidget->setColumnCount(4);
        tableWidget->horizontalHeader()->setVisible(false);
        tableWidget->horizontalHeader()->setDefaultSectionSize(125);
        tableWidget->verticalHeader()->setVisible(false);
        tableWidget->verticalHeader()->setDefaultSectionSize(125);
        BitMap = new QTableWidget(centralWidget);
        if (BitMap->columnCount() < 8)
            BitMap->setColumnCount(8);
        if (BitMap->rowCount() < 16)
            BitMap->setRowCount(16);
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::NoBrush);
        QBrush brush1(QColor(170, 255, 255, 255));
        brush1.setStyle(Qt::NoBrush);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setBackground(brush1);
        __qtablewidgetitem->setForeground(brush);
        BitMap->setItem(0, 0, __qtablewidgetitem);
        QBrush brush2(QColor(255, 255, 127, 255));
        brush2.setStyle(Qt::NoBrush);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        __qtablewidgetitem1->setBackground(brush2);
        BitMap->setItem(0, 1, __qtablewidgetitem1);
        BitMap->setObjectName(QString::fromUtf8("BitMap"));
        BitMap->setGeometry(QRect(540, 80, 300, 480));
        QFont font;
        font.setPointSize(5);
        BitMap->setFont(font);
        BitMap->setContextMenuPolicy(Qt::NoContextMenu);
        BitMap->setStyleSheet(QString::fromUtf8("QTableWidget::item[row=\"0\"][column=\"0\"]::item {\n"
"    background: blue;\n"
"}\n"
"\n"
"QTableWidget::item[row=\"0\"][column=\"1\"]::item {\n"
"    background: red;\n"
"}"));
        BitMap->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        BitMap->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        BitMap->setEditTriggers(QAbstractItemView::NoEditTriggers);
        BitMap->setSelectionMode(QAbstractItemView::NoSelection);
        BitMap->setShowGrid(true);
        BitMap->setRowCount(16);
        BitMap->setColumnCount(8);
        BitMap->horizontalHeader()->setVisible(false);
        BitMap->horizontalHeader()->setDefaultSectionSize(30);
        BitMap->verticalHeader()->setVisible(false);
        BitMap->verticalHeader()->setDefaultSectionSize(30);
        BackLabel_2 = new QLabel(centralWidget);
        BackLabel_2->setObjectName(QString::fromUtf8("BackLabel_2"));
        BackLabel_2->setGeometry(QRect(840, 20, 151, 561));
        BackLabel_2->setStyleSheet(QString::fromUtf8("border:1px solid black;"));
        fileSystem->setCentralWidget(centralWidget);
        AddFileLabel_2->raise();
        AddFileLabel->raise();
        AddDirLabel->raise();
        BackLabel->raise();
        BackBtn->raise();
        AddDirBtn->raise();
        AddFileBtn->raise();
        FormatBtn->raise();
        PathLabel->raise();
        tableWidget->raise();
        BitMap->raise();
        BackLabel_2->raise();

        retranslateUi(fileSystem);

        QMetaObject::connectSlotsByName(fileSystem);
    } // setupUi

    void retranslateUi(QMainWindow *fileSystem)
    {
        fileSystem->setWindowTitle(QCoreApplication::translate("fileSystem", "fileSystem", nullptr));
        BackBtn->setText(QString());
        AddDirBtn->setText(QString());
        FormatBtn->setText(QString());
        PathLabel->setText(QCoreApplication::translate("fileSystem", "<html><head/><body><p><span style=\" font-size:10pt;\">\\root</span></p></body></html>", nullptr));
        BackLabel->setText(QCoreApplication::translate("fileSystem", "<html><head/><body><p align=\"center\"><span style=\" font-size:10pt; font-weight:700;\">\350\277\224\345\233\236\344\270\212\347\272\247</span></p></body></html>", nullptr));
        AddDirLabel->setText(QCoreApplication::translate("fileSystem", "<html><head/><body><p align=\"center\"><span style=\" font-size:10pt; font-weight:700;\">\346\226\260\345\273\272\347\233\256\345\275\225</span></p></body></html>", nullptr));
        AddFileLabel->setText(QCoreApplication::translate("fileSystem", "<html><head/><body><p align=\"center\"><span style=\" font-size:10pt; font-weight:700;\">\346\226\260\345\273\272\346\226\207\344\273\266</span></p></body></html>", nullptr));
        AddFileLabel_2->setText(QCoreApplication::translate("fileSystem", "<html><head/><body><p align=\"center\"><span style=\" font-size:10pt; font-weight:700; color:#ff0000;\">\346\240\274\345\274\217\345\214\226</span></p></body></html>", nullptr));

        const bool __sortingEnabled = BitMap->isSortingEnabled();
        BitMap->setSortingEnabled(false);
        QTableWidgetItem *___qtablewidgetitem = BitMap->item(0, 0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("fileSystem", "FAT\350\241\250", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = BitMap->item(0, 1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("fileSystem", "\346\240\271\347\233\256\345\275\225", nullptr));
        BitMap->setSortingEnabled(__sortingEnabled);

        BackLabel_2->setText(QCoreApplication::translate("fileSystem", "<html><head/><body><p><span style=\" font-size:8pt;\">\345\267\246\351\224\256</span></p><p><span style=\" font-size:8pt;\">\346\211\223\345\274\200\347\233\256\345\275\225/\346\237\245\347\234\213\346\226\207\344\273\266</span></p><p><br/></p><p><span style=\" font-size:8pt;\">\345\217\263\351\224\256\347\233\256\345\275\225</span></p><p><span style=\" font-size:8pt;\">\351\200\211\346\213\251\350\277\233\345\205\245/\345\210\240\351\231\244</span></p><p><br/></p><p><span style=\" font-size:8pt;\">\345\217\263\351\224\256\346\226\207\344\273\266</span></p><p><span style=\" font-size:8pt;\">\351\200\211\346\213\251\346\237\245\347\234\213/\344\277\256\346\224\271/\345\210\240\351\231\244</span></p><p><span style=\" font-size:8pt;\"><br/></span></p><p><span style=\" font-size:8pt;\">\345\217\263\344\276\247\346\230\276\347\244\272\347\251\272\351\227\264\345\215\240\347\224\250\346\203\205\345\206\265</span></p><p><span style=\" font-size:8pt;\">\351\242\234\350\211\262\345\246\202\344\270\213:</span></p><p><sp"
                        "an style=\" font-size:8pt;\">\347\233\256\345\275\225:\344\272\256\350\223\235\350\211\262</span></p><p><span style=\" font-size:8pt;\">\346\226\207\344\273\266\350\265\267\345\247\213\345\235\227:\351\273\204\350\211\262</span></p><p><span style=\" font-size:8pt;\">\345\220\216\347\273\255\346\226\207\344\273\266:\347\201\260\350\211\262</span></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class fileSystem: public Ui_fileSystem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILESYSTEM_H
