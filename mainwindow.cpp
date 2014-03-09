#include "mainwindow.h"
#include <iostream>
#include <qdebug.h>
#include <QTextStream>
#include <QFile>
#include "ui_mainwindow.h"

using namespace std;


const double  MIN_DIST = 0.1;
typedef struct{
    int index;  //下标，即行
    double dist_from_in;
    double vector[8];

}color_vector;


QList<color_vector> col_vec;
//计算两个向量之间的欧式距离
double calc_dist(double *vector_a, double *vector_b, int d)
{
    double ans = 0;
    for(int i = 0;i<d;i++){
        ans += pow(vector_a[i]-vector_b[i], 2);
    }
    return ans;
}

bool vector_comparator(color_vector t1, color_vector t2)
{
    return t1.dist_from_in < t2.dist_from_in;
}

QList<color_vector> search(double *in_vec, int k, int d)
{
    //初始化全部的数据到all_color_vectors中
    QList<color_vector> all_color_vectors;
    color_vector tmp_col_vec;
    QFile file("E:/result.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return all_color_vectors;


    QTextStream in(&file);
    int i = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList list = line.split(" ", QString::SkipEmptyParts);
        tmp_col_vec.index = i;
        for (int j = 0; j < list.size(); ++j)
            tmp_col_vec.vector[j] = list.at(j).toDouble();
        tmp_col_vec.dist_from_in = calc_dist(in_vec, tmp_col_vec.vector, d);
        all_color_vectors << tmp_col_vec;
        i++;
    }
    qSort(all_color_vectors.begin(), all_color_vectors.end(), vector_comparator);
    file.close();
    return all_color_vectors;
}





MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    ui->setupUi(this);
    this->b_null_photo = 0;
    this->p_scene1 = new QGraphicsScene();
    ui->graphicsView1->setScene(p_scene1);

    this->p_res1 = new QGraphicsScene();
    this->p_res2 = new QGraphicsScene();
    this->p_res3 = new QGraphicsScene();
    this->p_res4 = new QGraphicsScene();
    ui->g1->setScene(p_res1);
    ui->g2->setScene(p_res2);
    ui->g3->setScene(p_res3);
    ui->g4->setScene(p_res4);


    this->p_srcImg = NULL;
    this->p_workImg = NULL;
    this->p_hsv = NULL;
    this->p_ROI =NULL;
    this->p_oROI = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*
 *载入源图像
 */
void MainWindow::on_b1_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image Files (*.png *.jpg *.bmp)"));
    printf("0");
    if(!fileName.isEmpty())
    {
        this->b_null_photo = 1;
        QByteArray ba(fileName.toLatin1().data());
        cout<<ba.data()<<endl;
        p_srcImg = cvLoadImage(fileName.toStdString().c_str());
        t_img = f_IplImage2QImage(p_srcImg,0,0);
        p_workImg = p_srcImg;
        p_pix1 = QPixmap::fromImage(t_img);
        this->p_item1 = p_scene1->addPixmap(p_pix1);
        ui->graphicsView1->viewport()->update();
    }

}

void MainWindow::on_b2_clicked()
{
}





void MainWindow::on_b3_clicked()
{
    this->p_srcImg = this->f_findROI(this->p_workImg);
}


/*
 *将图像从IplImage形式转换为Qt能识别的QImage
 */
QImage MainWindow::f_IplImage2QImage(const IplImage *iplImage, double mini, double maxi)
{

    uchar *qImageBuffer = NULL;
    int width = iplImage->width;
    int widthStep = iplImage->widthStep;
    int height = iplImage->height;

    switch (iplImage->depth)
    {
    case IPL_DEPTH_8U:
        if(iplImage->nChannels == 1)
        {

            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const uchar *iplImagePtr = (const uchar *) iplImage->imageData;

            for(int y = 0; y < height; y++)
            {
                memcpy(QImagePtr, iplImagePtr, width);
                QImagePtr += width;
                iplImagePtr += widthStep;
            }

        }
        else if(iplImage->nChannels == 3)
        {
            qImageBuffer = (uchar *) malloc(width*height*4*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const uchar *iplImagePtr = (const uchar *) iplImage->imageData;
            for(int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {

                    QImagePtr[0] = iplImagePtr[0];
                    QImagePtr[1] = iplImagePtr[1];
                    QImagePtr[2] = iplImagePtr[2];
                    QImagePtr[3] = 0;

                    QImagePtr += 4;
                    iplImagePtr += 3;
                }
                iplImagePtr += widthStep-3*width;
            }

        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=8U and %d channels ", iplImage->nChannels);
        }
        break;
    case IPL_DEPTH_16U:
        if(iplImage->nChannels == 1)
        {
            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;

            const unsigned int *iplImagePtr = (const unsigned int *)iplImage->imageData;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    *QImagePtr++ = ((*iplImagePtr++) >> 8);
                }
                iplImagePtr += widthStep/sizeof(unsigned int)-width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=16U and %d channels ", iplImage->nChannels);

        }
        break;
    case IPL_DEPTH_32F:
        if(iplImage->nChannels == 1)
        {
            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const float *iplImagePtr = (const float *) iplImage->imageData;
            for(int y = 0; y < height; y++)
            {
                for(int x = 0; x < width; x++)
                {
                    uchar p;
                    float pf = 255 * ((*iplImagePtr++) - mini) / (maxi - mini);
                    if(pf < 0) p = 0;
                    else if(pf > 255) p = 255;
                    else p = (uchar) pf;

                    *QImagePtr++ = p;
                }
                iplImagePtr += widthStep/sizeof(float)-width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=32F and %d channels ", iplImage->nChannels);
        }
        break;
    case IPL_DEPTH_64F:
        if(iplImage->nChannels == 1)
        {
            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const double *iplImagePtr = (const double *) iplImage->imageData;
            for(int y = 0; y < height; y++)
            {
                for(int x = 0; x < width; x++)
                {
                    uchar p;
                    double pf = 255 * ((*iplImagePtr++) - mini) / (maxi - mini);

                    if(pf < 0) p = 0;
                    else if(pf > 255) p = 255;
                    else p = (uchar) pf;

                    *QImagePtr++ = p;
                }
                iplImagePtr += widthStep/sizeof(double)-width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=64F and %d channels ", iplImage->nChannels);
        }
        break;
    default:
        qDebug("IplImageToQImage: image format is not supported : depth=%d and %d channels ", iplImage->depth, iplImage->nChannels);
    }

    QImage qImage;
    QVector<QRgb> vcolorTable(256);
    if(iplImage->nChannels == 1)
    {
        QRgb *colorTable = new QRgb[256];
        for(int i = 0; i < 256; i++)
        {
            colorTable[i] = qRgb(i, i, i);
            vcolorTable[i] = colorTable[i];
        }
        qImage = QImage(qImageBuffer, width, height, QImage::Format_Indexed8).copy();
        qImage.setColorTable(vcolorTable);
    }
    else
    {
        qImage = QImage(qImageBuffer, width, height, QImage::Format_RGB32).copy();
    }
    free(qImageBuffer);
    return qImage;
}

IplImage* MainWindow::f_findROI(IplImage *p_workImg)
{
    //    IplImage *t_roi =p_workImg;
    //    int dark;
    //    int light;
    //    int t_h = 0;
    //    int t_w = 0;
    //    int  mode = CV_RETR_CCOMP;
    //    int  contours_num = 0,color;
    int red=0;
    int blue =0;
    int yellow =0;
    int green =0;
    int purple =0;
    int pink = 0;
    int black =0;
    int white =0;
    int nu = 0;
    int human =0;
    int black_human =0;
    int white_human =0;
    double h1,S,V,R,G,B,max,min,temp;
    double in[8];

    CvScalar color1;
    //    CvScalar external_color;
    //    CvScalar hole_color;
    //    CvRect ROIrect;
    //    CvMemStorage* storage = cvCreateMemStorage(0);
    //    CvSeq* contour = 0;
    //    IplImage* pImg;
    //    IplImage* pContourImg = NULL;
    //    IplImage* dst = NULL;
    //    IplImage* HSI = NULL;
    //    IplImage* final_dst = NULL;
    QImage r_img1;
    QImage r_img2;
    QImage r_img3;
    QImage r_img4;
    QPixmap r_pix1;
    QPixmap r_pix2;
    QPixmap r_pix3;
    QPixmap r_pix4;


    int  pg[256],i,thre;

    //    for (i=0;i<256;i++) pg[i]=0;
    //    for (i=0;i<p_workImg->imageSize;i++)
    //    {
    //        unsigned char temp = (unsigned char)p_workImg->imageData[i];
    //        pg[temp]++;
    //        if(temp > 128)
    //            dark++;
    //        else
    //            light++;
    //    }
    //    thre = BasicGlobalThreshold(pg,0,256);
    //    int ignore = 10;
    //    pImg = cvCreateImage(cvGetSize(p_workImg),IPL_DEPTH_8U,1);
    //    HSI = cvCreateImage(cvGetSize(p_workImg),IPL_DEPTH_8U,3);
    //    dst = cvCreateImage(cvGetSize(p_workImg),p_workImg->depth,p_workImg->nChannels);
    //    cvZero(dst);
    //    cvCvtColor(p_workImg,pImg,CV_BGR2GRAY);
    //    cvThreshold(pImg,pImg,thre,255,CV_THRESH_BINARY_INV);
    //    pContourImg = cvCreateImage(cvGetSize(pImg),IPL_DEPTH_8U,3);
    //    cvZero(pContourImg);
    //    mode = CV_RETR_LIST;
    //    contours_num=cvFindContours(pImg,storage,&contour,
    //        sizeof(CvContour),mode,CV_CHAIN_APPROX_NONE);



    //    color=2;
    //    int n;
    //    int ROIArea = 0;
    //    int imgArea = pImg->width * pImg->height;
    //    qDebug("Image->width %d",pImg->width);
    //    qDebug("Image->height %d",pImg->height);
    //    qDebug("Img area is %d",imgArea);
    //    t_h = pImg->height;
    //    t_w = pImg->width;

    //    CvSeq* ROIContour;
    //    double area[256];

    //    for (n=0; contour != 0 ; contour = contour->h_next,n++)
    //    {
    //        if(n>=255)
    //        {
    //            break;
    //        }
    //        area[n] = cvContourArea(contour);

    //        if(area[n] < ignore) {
    //            continue;
    //        }
    //        if(area[n] > ROIArea && area[n] != imgArea)
    //        {
    //            ROIArea = area[n];
    //            ROIContour = contour;
    //        }
    //    }
    //    contour = ROIContour;
    //    CvBox2D rect = cvMinAreaRect2(contour,storage);
    //    CvPoint2D32f rect_pts0[4];


    //    cvBoxPoints(rect, rect_pts0);

    //    int npts = 4;
    //    CvPoint rect_pts[4], *pt = rect_pts;
    //    CvPoint lefttop,rightbottom;
    //    lefttop = rightbottom = cvPointFrom32f(rect.center);

    //    for (int rp=0; rp<4; rp++)
    //    {
    //        rect_pts[rp]= cvPointFrom32f(rect_pts0[rp]);
    //        if(rect_pts[rp].x < lefttop.x)
    //            lefttop.x = rect_pts[rp].x;
    //        if(rect_pts[rp].y <lefttop.y)
    //            lefttop.y = rect_pts[rp].y;
    //        if(rect_pts[rp].x > rightbottom.x)
    //            rightbottom.x = rect_pts[rp].x;
    //        if(rect_pts[rp].y > rightbottom.y)
    //            rightbottom.y = rect_pts[rp].y;
    //    }
    //    if(lefttop.y<=0)
    //        lefttop.y = 1;
    //    if(lefttop.x<=0)
    //        lefttop.x = 1;
    //    if(rightbottom.x>t_w)
    //        rightbottom.x = t_w-1;
    //    if(rightbottom.y>t_h)
    //        rightbottom.y = t_h-1;
    //    cvDrawRect(pContourImg,lefttop,rightbottom,CV_RGB(120,120,120),3);



    //    ROIrect.x = lefttop.x;
    //    ROIrect.y = lefttop.y;
    //    ROIrect.height = rightbottom.y - lefttop.y-5;
    //    ROIrect.width = rightbottom.x - lefttop.x-5;
    //    if(ROIrect.height <= 0&&ROIrect.height>=-4)
    //        ROIrect.height = ROIrect.height + 5;
    //    if(ROIrect.width <= 0&&ROIrect.width>=-4)
    //        ROIrect.width = ROIrect.width + 5;

    //    final_dst = this->f_makeROi(p_workImg,ROIrect);

    //this->f_check_roi(final_dst);

    int width = p_workImg->width;
    int height =p_workImg->height;
    /*
 *根据HSI色彩区间对图像中的像素点进行分类处理
 */
    for(int w =1;w<width;w++)
    {
        for(int h= 1;h<height;h++)
        {
            color1 = cvGet2D(p_workImg,h,w);
            R = color1.val[2];
            G = color1.val[1] ;
            B = color1.val[0] ;
            //基于RGB色彩空间，处理白色的区域
            if((int)color1.val[0]>=220&&(int)color1.val[0]<250&&(int)color1.val[1]>=220&&(int)color1.val[1]<250&&(int)color1.val[2]>=220&&(int)color1.val[2]<250)
            {
                white++;
            }
            //基于RGB色彩空间，处理不可能出现在衣服的的颜色
            else if((int)color1.val[0]>=250&&(int)color1.val[1]>=250&&(int)color1.val[2]>=250)
            {
                nu++;
            }
            //基于RGB色彩空间，处理黑色的区域
            else if((int)color1.val[0]<45&&(int)color1.val[1]<45&&(int)color1.val[2]<45)
            {
                black++;
            }
            //            else if((int)color1.val[0]>=220&&(int)color1.val[0]<235&&(int)color1.val[1]>=170&&(int)color1.val[1]<190&&(int)color1.val[2]>=140&&(int)color1.val[2]<160)
            //            {
            //                human++;
            //            }
            else if((int)color1.val[2]>=220&&(int)color1.val[2]<235&&(int)color1.val[1]>=165&&(int)color1.val[1]<190&&(int)color1.val[0]>=125&&(int)color1.val[0]<175&&color1.val[1]>=color1.val[0]+12)
            {
                white_human++;
            }
            else if((int)color1.val[2]>=185&&(int)color1.val[2]<215&&(int)color1.val[1]>=115&&(int)color1.val[1]<155&&(int)color1.val[0]>=70&&(int)color1.val[0]<140&&color1.val[1]>=color1.val[0]+12&&color1.val[2]>color1.val[1]+30)
            {
                black_human++;
            }
            else if((int)color1.val[2]>=140&&(int)color1.val[2]<215&&(int)color1.val[1]>=90&&(int)color1.val[1]<130&&(int)color1.val[0]>=0&&(int)color1.val[0]<80&&color1.val[2]>color1.val[1]+30)
            {
                black_human++;
            }
            else if((int)color1.val[2]>=105&&(int)color1.val[2]<130&&(int)color1.val[1]>=60&&(int)color1.val[1]<80&&(int)color1.val[0]>=0&&(int)color1.val[0]<55&&color1.val[2]>color1.val[1]+30)
            {
                black_human++;
            }
            else
            {
                //将RGB空间转换成HSI空间
                max = f_getmax(R,G,B);
                min = f_getmin(R,G,B);
                if(R == max)
                    h1 = (G-B)/(max-min);
                if(G == max)
                    h1 = (B-R)/(max-min)+2;
                if(B == max)
                    h1 = (R-G)/(max-min)+4;
                h1 = h1 *60;
                if(h1<0)
                    h1 = h1+360;
                //根据HSI色彩空间的空间形状呈圆锥分布的特点根据扇形的弧度对像素点进行分类
                if((h1>=0&&h1<=9)||(h1>=345&&h1<=360))
                {
                    red++;
                }
                else if(h1>9&&h1<=75)
                {
                    yellow++;
                }
                else if(h1>75&&h1<=190)
                {
                    green++;
                }
                else if(h1>190&&h1<=280)
                {
                    blue++;
                }
                else if(h1>280&&h1<=315)
                {
                    purple++;
                }
                else if(h1>315&&h1<345)
                {
                    pink++;
                }


            }
        }
    }
    human = black_human+white_human;
    qDebug("human area is %d",human);
    qDebug("fuck all    %d",width*height);
    qDebug("fuck null   %d",nu);
    qDebug("fuck yellow     %d",yellow);
    in[0] = (double)100*white/(width*height-nu-human);
    in[1] = (double)100*black/(width*height-nu-human);
    in[2] = (double)100*red/(width*height-nu-human);
    in[3] = (double)100*blue/(width*height-nu-human);
    in[4] = (double)100*green/(width*height-nu-human);
    in[5] = (double)100*purple/(width*height-nu-human);
    in[6] = (double)100*pink/(width*height-nu-human);
    in[7] = (double)100*yellow/(width*height-nu-human);

    int g = 0;
    for(g=0;g<8;g++)
    {
        qDebug("%f",in[g]);
    }

    int inde[5];
    QList<color_vector> ans = search(in, 8, 8);
    QFile file("E:/index.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream ins(&file);
    QString ind[3000];
    i = 0;
    while (!ins.atEnd())
    {
        QString line = ins.readLine();
        QStringList list = line.split("@", QString::SkipEmptyParts);
        QString temp1 = "E:/"+list.at(0)+"/"+list.at(1);
        ind[i] = temp1;
        i++;

    }
    i = 0;
    int t1 =0,t2 = 0;
    IplImage*out[10];
    while(i<10)
    {
        out[i] = cvLoadImage(ind[ans.at(i).index].toStdString().c_str());
        cout<<ind[ans.at(i).index].toStdString().c_str()<<endl;
        for(int j = 0;j<8;j++)
            cout << ans.at(i).vector[j] << " ";
        cout<<endl;
        i++;
    }
    t_img = f_IplImage2QImage(p_srcImg,0,0);
    p_pix1 = QPixmap::fromImage(t_img);
    this->p_item1 = p_scene1->addPixmap(p_pix1);
    ui->graphicsView1->viewport()->update();
    r_img1 = f_IplImage2QImage(out[1],0,0);
    r_img2 = f_IplImage2QImage(out[2],0,0);
    r_img3 = f_IplImage2QImage(out[3],0,0);
    r_img4 = f_IplImage2QImage(out[4],0,0);
    r_pix1 = QPixmap::fromImage(r_img1);
    r_pix2 = QPixmap::fromImage(r_img2);
    r_pix3 = QPixmap::fromImage(r_img3);
    r_pix4 = QPixmap::fromImage(r_img4);

    this->p_resitem1 = p_res1->addPixmap(r_pix1);
    this->p_resitem2 = p_res2->addPixmap(r_pix2);
    this->p_resitem3 = p_res3->addPixmap(r_pix3);
    this->p_resitem4 = p_res4->addPixmap(r_pix4);

    ui->g1->viewport()->update();
    ui->g2->viewport()->update();
    ui->g3->viewport()->update();
    ui->g4->viewport()->update();


    //    external_color = CV_RGB(rand()&255,rand()&255,rand()&255);
    //    hole_color = CV_RGB(rand()&255,rand()&255,rand()&255);
    //    cvDrawContours(pContourImg,contour,external_color,hole_color,1,1,8);

    //    cvReleaseMemStorage(&storage);
    file.close();
    //    return pContourImg;

}

/*
 *根据矩形区域，从源图像中获取ROI
 */
IplImage *MainWindow::f_makeROi(IplImage *work, CvRect rect)
{
    cvSetImageROI(work,rect);
    IplImage *dst1 =cvCreateImage(cvSize(rect.width,rect.height),work->depth,work->nChannels);
    cvCopy(work,dst1);
    cvResetImageROI(work);
    return dst1;
}

/*
 *计算基本标准阈值
 */
int  MainWindow::BasicGlobalThreshold(int *pg,int start,int end)
{
    int  i,t,t1,t2,k1,k2;
    double u,u1,u2;

    t=0;     u=0;
    for (i=start;i<end;i++) {
        t+=pg[i];
        u+=i*pg[i];
    }
    k2=(int) (u/t);

    do {
        k1=k2;
        t1=0;     u1=0;
        for (i=start;i<=k1;i++) {
            t1+=pg[i];
            u1+=i*pg[i];
        }
        t2=t-t1;
        u2=u-u1;
        if (t1) u1=u1/t1;
        else u1=0;
        if (t2) u2=u2/t2;
        else u2=0;
        k2=(int) ((u1+u2)/2);
    }
    while(k1!=k2);
    return(k1);
}


/*
 *根据图像的HS的值构建直方图
 */
void MainWindow::f_check_roi(IplImage *ROI)
{

    IplImage *hsv = cvCreateImage( cvGetSize(ROI), 8, 3 );
    IplImage* h_plane = cvCreateImage( cvGetSize(ROI), 8, 1 );
    IplImage* s_plane = cvCreateImage( cvGetSize(ROI), 8, 1 );
    IplImage* v_plane = cvCreateImage( cvGetSize(ROI), 8, 1 );
    IplImage* planes[] = { h_plane, s_plane };
    int h_bins = 16, s_bins = 8;
    int hist_size[] = {h_bins, s_bins};
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 255 };
    float* ranges[] = { h_ranges, s_ranges };
    cvCvtColor( ROI, hsv, CV_BGR2HSV );
    cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
    CvHistogram * hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
    cvCalcHist( planes, hist, 0, 0 );
    float max_value;
    cvGetMinMaxHistValue( hist, 0, &max_value, 0, 0 );
    int height = 240;
    int width = (h_bins*s_bins*6);
    IplImage* hist_img = cvCreateImage( cvSize(width,height), 8, 3 );
    cvZero( hist_img );
    IplImage * hsv_color = cvCreateImage(cvSize(1,1),8,3);
    IplImage * rgb_color = cvCreateImage(cvSize(1,1),8,3);
    int bin_w = width / (h_bins * s_bins);
    for(int h = 0; h < h_bins; h++)
    {
        for(int s = 0; s < s_bins; s++)
        {   int i = h*s_bins + s;
            float bin_val = cvQueryHistValue_2D( hist, h, s );
            int intensity = cvRound(bin_val*height/max_value);
            cvSet2D(hsv_color,0,0,cvScalar(h*180.f / h_bins,s*255.f/s_bins,255,0));
            cvCvtColor(hsv_color,rgb_color,CV_HSV2BGR);
            CvScalar color = cvGet2D(rgb_color,0,0);
            cvRectangle( hist_img, cvPoint(i*bin_w,height), cvPoint((i+1)*bin_w,height - intensity), color, -1, 8, 0 );
        }
    }
    //this->f_getHSI(ROI);
    cvShowImage( "H-S Histogram", hist_img );
    cvWaitKey(0);


}


/*
 *获取RGB三个参数中的最大值，用于与HSI色彩空间的转换
 */
double MainWindow::f_getmax(double R, double G, double B)
{
    double temp;
    if(R>G)
        temp = R;
    else
        temp = G;
    if(temp > B)
        return temp;
    else
        return B;

}



/*
 *获取RGB三个参数中的最小值，用于与HSI色彩空间的转换
 */
double MainWindow::f_getmin(double R, double G, double B)
{
    double temp;
    if(R<G)
        temp = R;
    else
        temp = G;
    if(temp<B)
        return temp;
    else
        return B;
}
