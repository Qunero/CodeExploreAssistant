#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDate>
#include <QStandardItemModel>
#include <QtDebug>

#define APP_NAME "CodeExploreAssistant"
#define APP_REVISION "1.0.0"

namespace Ui {
class MainWindow;
}

enum{
    EM_COLUMN_GROUP_NAME     = 0,
    EM_COLUMN_AUTOLOAD       = 1,
    EM_COLUMN_CFG_FILE_PATH  = 2,
    EM_COLOUM_COUNT          = 3
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_checkBox_autoConvert_toggled(bool checked);

    void on_pushButtonConvertDec2Hex_clicked();

    void on_pushButtonConvertHex2Dec_clicked();

    void on_lineEdit_dec_textChanged(const QString &arg1);

    void on_lineEdit_hex_textChanged(const QString &arg1);

    void on_action_About_triggered();

    void on_checkBox_showDetail_toggled(bool checked);

    void on_pushButton_addToList_clicked();

    void on_pushButton_modifyCfgFiel_clicked();

    void on_pushButton_openDetail_clicked();

    void on_pushButton_deleteCfgFile_clicked();

    void on_pushButton_clear_clicked();

private:
    Ui::MainWindow *ui;
    bool isCfgFileLoaded;
    qlonglong sharedNumberValue;
    QString aboutSoftwareMsg;
    QStandardItemModel cfgFileListModel;

    void updateUiFromDec2Hex();
    void updateUiFromHex2Dec();
    void clearChoosenFileInfo();
    void updateChoosenFileInfo(QStringList rowInfo);
    bool hasValidSelectedCfgFile();
};

#endif // MAINWINDOW_H
