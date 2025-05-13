#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    populatePorts();

    connect(&serial, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    connect(ui->comboBox_format, &QComboBox::currentTextChanged, this, &MainWindow::onFormatChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populatePorts()
{
    ui->portname->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->portname->addItem(info.portName());
    }
}

void MainWindow::on_pushButton_connect_clicked()
{
    serial.setPortName(ui->portname->currentText());
    qDebug() << ui->portname->currentText();
    serial.setBaudRate(ui->baudrate->currentText().toUInt());
    qDebug() << serial.baudRate();

    if (ui->stopbits->currentText() == "One Stop Bit")
        serial.setStopBits(QSerialPort::OneStop);

    qDebug() << serial.stopBits();
    if (ui->parity->currentText() == "None")
        serial.setParity(QSerialPort::NoParity);
    qDebug() << serial.parity();
    if (ui->databits->currentText() == "Eight")
        serial.setDataBits(QSerialPort::Data8);
    qDebug() << serial.dataBits();


    if (serial.open(QSerialPort::ReadWrite)) {
        // Создаем путь к файлу логирования на рабочем столе
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        logFilePath = desktopPath + "/log_" + timestamp + ".txt";
        sessionData.clear();
    }
}

void MainWindow::on_pushButton_disconnect_clicked()
{
    if (serial.isOpen()) {
        serial.close();
        QFile file(logFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(sessionData);
            file.close();
        } else {
            qDebug() << "Не удалось сохранить лог:" << file.errorString();
        }
    }
}

void MainWindow::on_send_clicked()
{
    QString text = ui->lineEdit_input->text();
    text += "\n";
    QString format = ui->comboBox_format->currentText();
    QByteArray bytesToSend = prepareDataToSend(text, format);
    serial.write(bytesToSend);
    input.append(bytesToSend);
    if (!serial.waitForBytesWritten()){
        qDebug() << bytesToSend;
    }
    QString formattedData = convertData(input, format);
    ui->textEdit_input->setPlainText(formattedData);
}

void MainWindow::on_clear_input_clicked()
{
    ui->textEdit_input->clear();
    input.clear();
}

void MainWindow::on_clear_output_clicked()
{
    ui->textEdit_output->clear();
    output.clear();
}


void MainWindow::handleReadyRead()
{
    static QByteArray buffer;

    QByteArray data = serial.readAll();
    buffer.append(data);

    while (buffer.contains('\n')) {
        int index = buffer.indexOf('\n');
        QByteArray line = buffer.left(index).trimmed();  // строка до \n
        buffer = buffer.mid(index + 1);  // остаток после \n

        QString decoded = QString::fromLatin1(line);
        sessionData.append(decoded.toUtf8() + "\n");  // добавляем строку в лог

        ui->textEdit_output->setPlainText(decoded);
    }
}



void MainWindow::onFormatChanged(const QString &format) {
    QString formattedData = convertData(output, format);
    QString formattedInput = convertData(input, format);
    ui->textEdit_output->setPlainText(formattedData);
    ui->textEdit_input->setPlainText(formattedInput);
}


QString MainWindow::convertData(const QByteArray &data, const QString &format) {
    if (format == "ASCII") {
        return QString::fromLatin1(data);
    } else if (format == "HEX") {
        return data.toHex(' ').toUpper();
    } else if (format == "BIN") {
        QString binOutput;
        for (char byte : data) {
            binOutput.append(QString("%1 ").arg(static_cast<unsigned char>(byte), 8, 2, QLatin1Char('0')));
        }
        return binOutput.trimmed();
    }
    return QString::fromLatin1(data);
}

QByteArray MainWindow::prepareDataToSend(const QString &data, const QString &format) {
    if (format == "ASCII") {
        // Отправляем данные как есть в формате ASCII
        return data.toLatin1();
    } else if (format == "HEX") {
        // Преобразуем строку HEX в байты
        QByteArray bytes;
        QStringList hexList = data.split(' ', Qt::SkipEmptyParts);
        for (const QString &hexStr : hexList) {
            bool ok;
            uint byte = hexStr.toUInt(&ok, 16);
            if (ok) {
                bytes.append(static_cast<char>(byte));
            } else {
                // Обработка ошибки при неверном формате
                qDebug() << "Invalid HEX value:" << hexStr;
            }
        }
        return bytes;
    } else if (format == "BIN") {
        // Преобразуем бинарную строку в байты
        QByteArray bytes;
        QStringList binList = data.split(' ', Qt::SkipEmptyParts);
        for (const QString &binStr : binList) {
            bool ok;
            uint byte = binStr.toUInt(&ok, 2);
            if (ok) {
                bytes.append(static_cast<char>(byte));
            } else {
                qDebug() << "Invalid BIN value:" << binStr;
            }
        }
        return bytes;
    }
    // По умолчанию отправляем как ASCII
    return data.toLatin1();
}
