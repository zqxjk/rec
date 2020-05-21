#ifndef SKINDETECTOR_H
#define SKINDETECTOR_H
#include <cassert>
#include <QDebug>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class skinDetector
{

private:
    //HSV руки


    Rect skinColorSamplerRectangle1, skinColorSamplerRectangle2;

    void calculateThresholds(Mat sample1, Mat sample2)//подсчёт HSV руки
    {
        int offsetLowThreshold = 35;
        int offsetHighThreshold = 30;

        Scalar hsvMeansSample1 = mean(sample1);//среднее значение с первого квадрата
        Scalar hsvMeansSample2 = mean(sample2);//среднее значение со второго квадрата

        //получаем HSV руки
        hLowThreshold = min(hsvMeansSample1[0], hsvMeansSample2[0]) - offsetLowThreshold;
        hHighThreshold = max(hsvMeansSample1[0], hsvMeansSample2[0]) + offsetHighThreshold;

        sLowThreshold = min(hsvMeansSample1[1], hsvMeansSample2[1]) - offsetLowThreshold;
        sHighThreshold = max(hsvMeansSample1[1], hsvMeansSample2[1]) + offsetHighThreshold;

        vLowThreshold = min(hsvMeansSample1[2], hsvMeansSample2[2]) - offsetLowThreshold;
        vHighThreshold = max(hsvMeansSample1[2], hsvMeansSample2[2]) + offsetHighThreshold;
    };

    void performOpening(Mat binaryImage, int kernelShape, Point kernelSize)//удаление шума
    {
        Mat structuringElement = getStructuringElement(kernelShape, kernelSize);//ядро
        morphologyEx(binaryImage, binaryImage, MORPH_OPEN, structuringElement);//морфологическое преобразование "открытие"
    };

public:

    int hLowThreshold = 0;
    int hHighThreshold = 0;
    int sLowThreshold = 0;
    int sHighThreshold = 0;
    int vLowThreshold = 0;
    int vHighThreshold = 0;

    bool calibrated = false;


    skinDetector()
    {
        hLowThreshold = 0;
        hHighThreshold = 0;
        sLowThreshold = 0;
        sHighThreshold = 0;
        vLowThreshold = 0;
        vHighThreshold = 0;

        calibrated = false;
    };



    void drawSkinColorSampler(Mat input)//рисуем квадраты
    {
        assert(!input.empty());

        int frameWidth = input.size().width;
        int frameHeight = input.size().height;

        int rectangleSize = 20;
        Scalar rectangleColor = Scalar(255, 0, 255);

        skinColorSamplerRectangle1 = Rect(4 * frameWidth / 5, frameHeight / 2, rectangleSize, rectangleSize);
        skinColorSamplerRectangle2 = Rect(4 * frameWidth / 5, frameHeight / 3, rectangleSize, rectangleSize);

        rectangle(input, skinColorSamplerRectangle1, rectangleColor);

        rectangle(input, skinColorSamplerRectangle2, rectangleColor);
    };

    vector<int> calibrate(Mat input)//калибровка
    {
        assert(!input.empty());

        Mat hsvInput = input;
        cvtColor(input, hsvInput,COLOR_RGB2HSV);

        Mat sample1 = Mat(hsvInput, skinColorSamplerRectangle1);
        Mat sample2 = Mat(hsvInput, skinColorSamplerRectangle2);

        calculateThresholds(sample1, sample2);

        vector<int> a = {        hLowThreshold,
                                 hHighThreshold,
                                 sLowThreshold,
                                 sHighThreshold,
                                 vLowThreshold,
                                 vHighThreshold,
                        };
        calibrated = true;
        return a;
    };

    Mat getSkinMask(Mat input)//получаем маску руки
    {
        assert(!input.empty());

        Mat skinMask;
        if(!calibrated)
        {
            skinMask = Mat::zeros(input.size(), CV_32F);
            return skinMask;
        }

        Mat hsvInput;
        cvtColor(input, hsvInput, COLOR_RGB2HSV);
        inRange(hsvInput, Scalar(hLowThreshold, sLowThreshold, vLowThreshold), Scalar(hHighThreshold, sHighThreshold, vHighThreshold), skinMask);
        performOpening(skinMask, MORPH_ELLIPSE, {3, 3});
        dilate(skinMask, skinMask, Mat(), Point(-1,-1), 3);

        return skinMask;
    };
};

#endif
