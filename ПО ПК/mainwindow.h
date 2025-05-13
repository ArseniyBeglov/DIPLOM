#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QStandardPaths>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    bool checkReceiving();

    ~MainWindow();

signals:
    bool asciiChosen();
    bool binChosen();
    bool hexChosen();


private slots:
    void on_pushButton_connect_clicked();

    void on_pushButton_disconnect_clicked();

    void on_send_clicked();

    void on_clear_input_clicked();

    void on_clear_output_clicked();

    void populatePorts();

    void handleReadyRead();

    void onFormatChanged(const QString &format);

private:
    Ui::MainWindow *ui;
    QSerialPort serial;
    QByteArray  input;
    QByteArray  output;
    // Объявление функций
    QByteArray prepareDataToSend(const QString &data, const QString &format);
    QString convertData(const QByteArray &data, const QString &format);
    QString logFilePath;
    QByteArray sessionData;
};
#endif // MAINWINDOW_H
