#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QApplication>
#include <QShortcut>
#include <QApplication>
#include <QPixmap>
#include <QDir>
#include <QProcess>
#include <QStringList>

#include "BackGroundRemover.h"
#include "SkinDetector.h"
#include "FaceDetector.h"
#include "SmoothFunction.h"
#include "Contours.h"


#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(timeSlot()));



    QPixmap bg(qApp->applicationDirPath() + "/res/bg.png");

    ui->label->setPixmap(bg);

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));              //Exit action
    connect(ui->bgButton, SIGNAL(released()), this, SLOT(bgNotch()));               //Background calibration
    connect(ui->cntButton, SIGNAL(released()), this, SLOT(cntNotch()));             //Skin Detector calibration
    connect(timer, SIGNAL(timeout()), this, SLOT(timeSlot()));
    connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(start_activated()));   //Start action
    connect(ui->actionBackground, SIGNAL(triggered()), this, SLOT(bgNotch()));      //same as bgButton
    connect(ui->actionSkin_samples, SIGNAL(triggered()), this, SLOT(cntNotch()));   //same as cntButton
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(stop()));               //Stop action


}

MainWindow::~MainWindow()
{
    QStringList arg;
    arg<<QString::number(python_pid);
    QProcess::startDetached("kill", arg);
    delete ui;
}

void MainWindow::bgNotch(){
    phase = 'b';
}

void MainWindow::cntNotch(){
    phase = 's';
}

#define MS_PER_FRAME 17
#define DEBUG 0
void MainWindow::frameProcessing(Mat frame){//processes individual frames and produces B/W file with hand

    proc_params = {
        ui->spinBox->value(),   //gaussian blur arguments
        ui->spinBox_2->value(),
        ui->spinBox_3->value(),
        ui->spinBox_4->value(),
        ui->spinBox_5->value(), //bilateral filter arguments
        ui->spinBox_6->value(),
        ui->spinBox_7->value(),
        ui->spinBox_8->value(), //skin detector thresholds
        ui->spinBox_9->value(),
        ui->spinBox_10->value(),//face detector thresholds
        ui->spinBox_11->value()
    };

    if(!ui->checkBox_4->isChecked())//option to mirror input frames
        flip(frame, frame, 1);
    Mat frameOut = frame.clone();


    BackGroundRemover backGroundRemover;
    skinDetector      skinDetector;

    skinDetector.drawSkinColorSampler(frame);

    if(phase == 'b'){
        bg = backGroundRemover.calibrate(frameOut);
        isCalibrated = true;
    }

    if(phase == 's'){
        skin_params = skinDetector.calibrate(frameOut);
        isMeasured = true;
    }

    if(isCalibrated == true){
        backGroundRemover.calibrated = true;
        backGroundRemover.background = bg;
    }

    if(isMeasured == true){
        skinDetector.calibrated     = true;
        skinDetector.hLowThreshold  = skin_params[0];
        skinDetector.hHighThreshold = skin_params[1];
        skinDetector.sLowThreshold  = skin_params[2];
        skinDetector.sHighThreshold = skin_params[3];
        skinDetector.vLowThreshold  = skin_params[4];
        skinDetector.vHighThreshold = skin_params[5];
    }

    if(ui->checkBox->isChecked()){
        rectangle(frame, Point(frame.cols*0.6, 0), Point(frame.cols, frameOut.rows*0.6), Scalar(255, 255, 255));
    }
    Mat image, handMask;

    Mat foreground = backGroundRemover.getForeground(frame);
    RemoveFaces(frame, foreground);

    if(ui->checkBox->isChecked()){
        image = getRectangle(foreground, Point(frame.cols*0.6, 0), Point(frame.cols, frame.rows*0.6));
        handMask = skinDetector.getSkinMask(image);
    }else{
        handMask = skinDetector.getSkinMask(foreground);//ñîçäà¸ì ìàñêó ðóêè
    }


    Gauss(handMask, handMask, proc_params[3], 9, 0, proc_params[4], proc_params[5], proc_params[6]);

    Mat matrix;

    matrix = RectangleHandContours(handMask);
    Mat hand   = InsertRectangleHand(matrix);
    imwrite(qApp->applicationDirPath().toStdString() + "/cache/hand.jpg", hand);//writes hand image to file for python script
    cvtColor(hand, hand, COLOR_GRAY2BGR, 3);
    ui->label_8->setPixmap(QPixmap::fromImage(QImage((unsigned char *) hand.data, hand.cols, hand.rows, QImage::Format_BGR888)));//displays B/W image of hand
    phase = 0x0;
}

void MainWindow::stop(){//pauses video stream but not python script
    ui->label->setPixmap(QPixmap(qApp->applicationDirPath() + "/res/bg.png"));
    cap.release();
    timer->stop();
}

void MainWindow::do_smth(char act){//function to pass strings (i. e. commands) to VLC player's unix socket via netcat

    QChar data[1] = {act};
    ui->label_2->setText(QString(data, 1));

    if(ui->checkBox_5->isChecked()){
        QString str = "nc -q 1  -U ~/rec/vlc.sock <<<" ;

        switch (act) {
            case '0':
                str += "loop";
            break;

            case '1':
                str += "pause";
            break;

            case '2':
                str += "volup";
            break;

            case '3':
                str += "voldown";
            break;

            case '4':
                str += "seek 10";
            break;

            case '5':
                str += "seek 20";
            break;

            default:
                str += "help";
            break;
        }
        QStringList ar;
        ar<<"-c"<<str;
        double start_delay = static_cast<double>(ui->spinBox_14->value())/ 1000.0;
        QString s = "sleep " + QString::number(start_delay);
        QProcess::startDetached("bash", ar);
        qDebug()<<s<<endl;
    }


}

void MainWindow::start_activated(){
    if(python_pid != 0){
        qDebug()<<"Performing soft restart (python script and camera re-opening)!"<<endl;
        QProcess::startDetached("kill", QStringList(QString::number(python_pid)));
        python_pid = 0;
        stop();
    }
    QStringList arg_py;
    arg_py<< qApp->applicationDirPath() + "/src/cnn.py";

    QProcess::startDetached("python", arg_py, qApp->applicationDirPath(), &python_pid);
    QProcess::startDetached("rm prediction.txt");

    if(ui->checkBox_3->isChecked()){
        if(!cap.isOpened()){
            std::string src = ui->textEdit_12->toPlainText().toStdString();//source link
            cap.open(src);
            cap.set(CAP_PROP_FORMAT, CV_32F);
            newFrame();
            timer->start(MS_PER_FRAME);
        }else{
            newFrame();
        }
    }else{

        ui->spinBox_13->setEnabled(1);
        int src_raw = ui->spinBox_13->value();
        ui->textEdit_12->setEnabled(0);

        cap.open(src_raw);
        cap.set(CAP_PROP_FORMAT, CV_32F);
        newFrame();
        timer->start(MS_PER_FRAME);
    }

}

#define FRAMES_PER_PREDICT 30

Mat MainWindow::newFrame(){
    Mat frame;
    cap.set(CAP_PROP_FORMAT, CV_32F);
    cap>>frame;
    counter++;

    frameProcessing(frame);
    ui->label->setPixmap(QPixmap::fromImage(QImage((unsigned char *) frame.data, frame.cols, frame.rows, QImage::Format_BGR888)));

    if(counter % FRAMES_PER_PREDICT == 0){
        std::fstream file("prediction.txt", std::ios::in);
        char temp;
        file>>temp;
        qDebug()<<temp<<endl;
        file.close();
        do_smth(temp);
    }

    return frame;


}

void MainWindow::timeSlot(){
    newFrame();
}


void MainWindow::on_actionStart_VLC_player_triggered(){//launches VLC player with unix-socket
    if(vlc_pid != 0){
        QProcess::startDetached("kill", QStringList(QString::number(vlc_pid)));
    }


    QStringList args;
    args<<"--rc-unix"<<"./rec/vlc.sock";
    QProcess::startDetached("vlc", args, "~/", &vlc_pid);

}
