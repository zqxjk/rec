#ifndef CONTOURS_H
#define CONTOURS_H

#include <cassert>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat RectangleHandContours(Mat input)//нахождение ограничивающего руку прямоугольника
{
    assert(!input.empty());

    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    Mat new_input;
    input.convertTo(new_input, CV_8UC1);
    findContours(new_input, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_NONE);//ищем контуры на изображении

    //поиск наибольшего контура(предположительно это рука)
    int index_area = -1;

    Mat cnt = new_input.clone();
    drawContours(cnt, contours, index_area, Scalar(255, 255, 255), FILLED, LINE_8, hierachy, 1);

    double biggest_area = 0;
    for (unsigned i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if(area > biggest_area)
        {
            biggest_area = area;
            index_area = i;
        }

    }

    if(index_area < 0)
        return input;

    //вырезаем наибольший контур
    Rect hand_rect = boundingRect(Mat(contours[index_area]));

    Mat output = Mat::zeros(input.size(), CV_8UC1);
    drawContours(output, contours, index_area, Scalar(255, 255, 255), FILLED, LINE_8, hierachy, 1);

    Mat output_rect = output(hand_rect);

    assert(!output.empty());
    return output_rect;
}

Mat InsertRectangleHand(Mat input_old)//преобразование прямоугольника в квадрат
{
    Mat input;
    input_old.convertTo(input, CV_8UC1);


    assert(!input.empty());

    //размеры выходного изображения
    int h = 100, w = 100;
    if (input.rows > input.cols)
    {
        h = input.rows;
        w = input.rows;
    }
    else
    {
        h = input.cols;
        w = input.cols;
    }

    Mat output(w, h, CV_8UC1, Scalar(0, 0, 0));//создание пустого изображения

    //попиксельное копирование
    if (h > 100)
    {
        for (int i = 0; i < input.rows; i++)
            for (int j = 0; j < input.cols; j++)
                  output.at<uchar>(i, j) = input.at<uchar>(i, j);
    }

    assert(!output.empty());
    return output;
}

bool FindHand(Mat input)//есть ли рука в кадре?
{
    bool find_hand = 0;
    for (int i = 0; i < input.rows; i++)
        for (int j = 0; j < input.cols; j++)
        {
            if (input.at<uchar>(i,j) != 0)
                find_hand = 1;
        }
    return find_hand;
}

Mat getRectangle(Mat input, Point p1, Point p2)//область ROI
{
    Rect rect = Rect(p1, p2);
    Mat image = input(rect);
    return image;
};

#endif
