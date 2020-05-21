#ifndef BACGGROUNDREMOVER_H
#define BACGGROUNDREMOVER_H

#include <cassert>

#include <opencv2/opencv.hpp>
#include "SmoothFunction.h"

using namespace std;
using namespace cv;

class BackGroundRemover//класс вырезание фона
{
private:
    void removeBackGround(Mat input, Mat background)//удалям фон
    {
        assert(!input.empty());

        int thresholdOffset = 10;//порог

        //сравнение текущего кадра с записанным фоном(перевод в бинарный вид)
        for (int i = 0; i < input.rows; i++)
            for (int j = 0; j < input.cols; j++)
            {
                uchar framePixel = input.at<uchar>(i, j);
                uchar bgPixel = background.at<uchar>(i, j);

                if (framePixel >= bgPixel - thresholdOffset && framePixel <= bgPixel + thresholdOffset)
                    input.at<uchar>(i, j) = 0;
                else
                    input.at<uchar>(i, j) = 255;
            }

        assert(!background.empty());
    };

    Mat getForegroundMask(Mat input)//вырезанный задний фон
    {
        assert(!input.empty());

        Mat foregroundMask;

        //возращаем пустое изображение, если нет сохраненного background
        if(!calibrated)
        {
            foregroundMask = Mat::zeros(input.size(), CV_32F);
            return foregroundMask;
        }

        cvtColor(input, foregroundMask, COLOR_RGB2GRAY);//перевод в оттенки серого
        removeBackGround(foregroundMask, background);//вырезание фона

        return foregroundMask;
    };

public:
    Mat background;//фон
    bool calibrated = false;//калибровка
    BackGroundRemover()
    {
        calibrated = false;
    };

    Mat calibrate(Mat input)//калибровка
    {
        assert(!input.empty());
        cvtColor(input, background, COLOR_BGR2GRAY);
        calibrated = true;
        return background;
    };

    Mat getForeground(Mat input)//получение изображения
    {
        assert(!input.empty());
        Mat foregroundMask = getForegroundMask(input);
        Mat foreground;
        Mat fg, fgm;
        foreground.convertTo(fg, CV_8U);
        foregroundMask.convertTo(fgm, CV_8U);
        input.copyTo(fg, fgm);

        return fg;
    };
};

#endif
