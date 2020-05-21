#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QTimer>
#include <QDir>

#include <opencv2/opencv.hpp>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QTimer *timer;                  //ti
    unsigned int counter = 0;       //frame counter
    qint64 python_pid = 0;          //pid of detached python script process
    qint64 vlc_pid    = 0;          //pid of detached VLC player process
    bool isCalibrated = false;
    bool isMeasured   = false;
    cv::Mat hand, bg, skin;
    std::vector<int> skin_params;   //parameters for skin detection
    QDir app_path;                  //detects executable path, used for 'limited' portability
    std::vector<int> proc_params;   //parameters for all other processing operations

public:
    char phase;                     //determines calibration phase (background deletion or skin sampling)
    cv::VideoCapture cap;           //video/camera stream

    cv::Mat newFrame();
    void frameProcessing(cv::Mat);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void do_smth(char);
    void timeSlot();
    void bgNotch();
    void cntNotch();
    void start_activated();
    void stop();
    void on_actionStart_VLC_player_triggered();
};




#endif // MAINWINDOW_H
