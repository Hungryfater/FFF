#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qfiledialog.h>
#include "cv.h"
#include "QGraphicsView"
#include "QGraphicsScene"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public:
    QImage f_IplImage2QImage(const IplImage *iplImage, double mini, double maxi);
    IplImage* f_findROI(IplImage *p_workImg);
    IplImage* f_makeROi(IplImage *work,CvRect rect);
    int BasicGlobalThreshold(int *pg,int start,int end);
    void f_check_roi(IplImage *ROI);
    void f_roi_get();
    double f_getmax(double R,double G,double B);
    double f_getmin(double R,double G,double B);

private slots:
    void on_b1_clicked();    
    void on_b2_clicked();
    void on_b3_clicked();

private:
    int b_null_photo;
    QPixmap p_pix1;
    QPixmap p_pix2;
    IplImage* p_srcImg;
    IplImage* p_workImg;
    IplImage* p_hsv;
    IplImage* p_oROI;
    IplImage* p_ROI;
    QGraphicsScene* p_scene1;
    QGraphicsScene* p_res1;
    QGraphicsScene* p_res2;
    QGraphicsScene* p_res3;
    QGraphicsScene* p_res4;
    QGraphicsPixmapItem* p_item1;
    QGraphicsPixmapItem* p_resitem1;
    QGraphicsPixmapItem* p_resitem2;
    QGraphicsPixmapItem* p_resitem3;
    QGraphicsPixmapItem* p_resitem4;
    Ui::MainWindow *ui;
    cv::Mat t_image;
    cv::Mat t_roi;
    QImage t_img;
    QImage t_img1;
    QImage t_img2;
    QImage t_resim1;
    QImage t_resim2;
    QImage t_resim3;
    QImage t_resim4;



};

#endif // MAINWINDOW_H
