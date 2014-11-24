#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDate>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QtDebug>

#define APP_NAME "CodeExploreAssistant"
#define APP_REVISION "1.0.0"

namespace Ui {
class MainWindow;
}

const QString APP_INIT_FILE_PATH = "config/cea.ini";
const QString APP_CFG_FILE_MAGIC_HEADER = "/*CEA_CFG_FILE*/";
const QString CfgFileColumnSep = " // ";

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
    void modifyCfgFileGroupInfo();

    void on_checkBox_autoConvert_toggled(bool checked);

    void on_pushButtonConvertDec2Hex_clicked();

    void on_pushButtonConvertHex2Dec_clicked();

    void on_lineEdit_dec_textChanged(const QString &arg1);

    void on_lineEdit_hex_textChanged(const QString &arg1);

    void on_action_About_triggered();

    void on_checkBox_showDetail_toggled(bool checked);

    void on_pushButton_saveToList_clicked();

    void on_pushButton_modifyCfgFile_clicked();

    void on_pushButton_openDetail_clicked();

    void on_pushButton_deleteCfgFile_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_chooseCfgFile_clicked();

    void on_lineEdit_cfgFilePath_textChanged(const QString &arg1);

    void on_pushButton_loadAllCfgFile_clicked();

private:
    Ui::MainWindow *ui;
    bool isCfgFileLoaded;
    bool isInModifyCfgFileGroupInfoState;
    int currentRowOfCfgFileList;
    qlonglong sharedNumberValue;
    QString aboutSoftwareMsg;
    QString currentFileName;
    QStandardItemModel cfgFileListModel;

    void updateUiFromDec2Hex();
    void updateUiFromHex2Dec();
    void clearChoosenFileInfo();
    void updateChoosenFileInfo(QStringList rowInfo);
    bool hasValidSelectedCfgFile();
    bool isCurrentFileExists();
    void saveRowToCfgFileListModel(QStringList rowInfo);
};

#endif // MAINWINDOW_H
