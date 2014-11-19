#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDate>

#define APP_NAME "CodeExploreAssistant"
#define APP_REVISION "1.0.0"

namespace Ui {
class MainWindow;
}

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

private:
    Ui::MainWindow *ui;
    bool isCfgFileLoaded;
    qlonglong sharedNumberValue;
    QString aboutSoftwareMsg;

    void updateUiFromDec2Hex();
    void updateUiFromHex2Dec();
};

#endif // MAINWINDOW_H
