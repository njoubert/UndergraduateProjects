#include "cv.h"
#include "highgui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

#include <iostream>

#ifdef _EiC
#define WIN32
#endif

#define LINE_COLOR CV_RGB(192,192,192)
#define CONTROL_POINT_FILE_NAME "tmp_controlPoints.yaml"

using namespace std;

void draw( IplImage* image );
void mouseCallback(int event, int x, int y, int flags, void* param);

bool pointInRect(CvPoint pPoint, CvRect pRect) { // includes the rect boundary
    // cout << "Point: " << pPoint.x << " " << pPoint.y << endl;
    // cout << "Rect: " << pRect.x << " " << pRect.y << " " << pRect.width << " " << pRect.height << endl;
    
    if (pPoint.x >= pRect.x && pPoint.x <= pRect.x + pRect.width && pPoint.y >= pRect.y && pPoint.y <= pRect.y + pRect.height) {
        // cout << "YES" << endl;
        return true;
    } else {
        // cout << "NO" << endl;
        return false;
    }
}

#define CPC_NUM_CONTROL_POINTS 4
#define CPC_NO_CONTROL_POINT -1

class ControlPointController {
private:
    CvPoint _controlPoints[CPC_NUM_CONTROL_POINTS];
    int _nextControlPointIndex;
    
    CvMat *_perspectiveTransform;
    
    int _controlPointRadius;
    
    bool _controlPointMoving;
    int _movingControlPointIndex;
    
    CvMemStorage *_storage;
    
    static ControlPointController *_sharedControlPointController;
    
    ControlPointController() {
        _nextControlPointIndex = 0;
        _perspectiveTransform = NULL;

        _controlPointRadius = 4;

        _controlPointMoving = false;
        _movingControlPointIndex = -1;
        
        _storage = cvCreateMemStorage(0);
        
        readControlPoints();
    }
public:
    
    static ControlPointController *sharedControlPointController() {
        if (NULL == _sharedControlPointController) {
            _sharedControlPointController = new ControlPointController();
        }
        
        return _sharedControlPointController;
    }
    
    ~ControlPointController() {
        cvReleaseMemStorage(&_storage);
    }
    
    int addControlPoint(int pX, int pY) { // This function is also called to move a control point. Returns the control point added/moved.
        // cout << "Add control point" << endl;
        int lIndex = CPC_NO_CONTROL_POINT;
        
        if (!_controlPointMoving) {
            if (_nextControlPointIndex < CPC_NUM_CONTROL_POINTS) {
                _controlPoints[_nextControlPointIndex].x = pX;
                _controlPoints[_nextControlPointIndex].y = pY;
                lIndex = _nextControlPointIndex;
                _nextControlPointIndex++;
            }
        } else {
            _controlPoints[_movingControlPointIndex].x = pX;
            _controlPoints[_movingControlPointIndex].y = pY;
            
            invalidatePerspectiveTransform();
            
            lIndex = _movingControlPointIndex;
        }
        
        writeControlPoints();
        
        return lIndex;
    }
    
    void clearControlPoints() {
        endMovingControlPoint();
        invalidatePerspectiveTransform();
        _nextControlPointIndex = 0;
        writeControlPoints();
    }
    
    void writeControlPoints() {
        CvFileStorage *lFile = cvOpenFileStorage(CONTROL_POINT_FILE_NAME, _storage, CV_STORAGE_WRITE);
        if (NULL != lFile) {
            for (int i = 0; i < _nextControlPointIndex; i++) {
                char lName[3];
                
                sprintf(lName, "x%d", i);
                cvWriteInt(lFile, lName, _controlPoints[i].x);
                
                sprintf(lName, "y%d", i);
                cvWriteInt(lFile, lName, _controlPoints[i].y);
            }
        } else {
            printf("Could not write control point file: %s. Ignoring.\n", CONTROL_POINT_FILE_NAME);
        }
        
        cvReleaseFileStorage(&lFile); 
    }
    
    void readControlPoints() {
        CvFileStorage* lFile = cvOpenFileStorage( CONTROL_POINT_FILE_NAME, _storage, CV_STORAGE_READ );
        if (NULL != lFile) {
            CvFileNode *lRoot = cvGetRootFileNode(lFile);
            
            for (int i = 0; i < CPC_NUM_CONTROL_POINTS; i++ ) {
                char lName[3];

                sprintf(lName, "x%d", i);
                int x = cvReadIntByName(lFile, lRoot, lName, -1 /* default value */ );
                
                sprintf(lName, "y%d", i);
                int y = cvReadIntByName(lFile, lRoot, lName, -1 /* default value */ );
                    
                if (x >=0 && y >= 0) {
                    _controlPoints[i].x = x;
                    _controlPoints[i].y = y;
                    _nextControlPointIndex = i+1;
                    
                    printf("Read saved control point %d: (%d, %d)\n", i, x, y );
                } else {
                    break; // no more, or malformatted, control points
                }
            }    
        }
            
        cvReleaseFileStorage(&lFile);
    }
    
    void startMovingControlPointWithIndex(int pControlPointIndex) {
        if (pControlPointIndex >= _nextControlPointIndex) {
            printf("Control point index out of bounds. Ignoring.\n");
            return;
        }
        
        // cout << "Moving: " << pControlPointIndex << endl;
        
        _controlPointMoving = true;
        _movingControlPointIndex = pControlPointIndex;
    }
    
    void endMovingControlPoint() {
        _controlPointMoving = false;
        _movingControlPointIndex = -1;
    }
    
    bool isMovingControlPoint() {
        return _controlPointMoving;
    }
    
    int controlPointIndexForCoordinates(int pX, int pY) {        
        for (int i = 0; i < _nextControlPointIndex; i++) {
            CvRect lControlPointBounds = cvRect(_controlPoints[i].x - _controlPointRadius, _controlPoints[i].y - _controlPointRadius, _controlPointRadius*2, _controlPointRadius*2);
            if (pointInRect(cvPoint(pX, pY), lControlPointBounds)) {
                return i;
            }
        }
        
        return CPC_NO_CONTROL_POINT;
    }
    
    void drawControlPoints(IplImage *pImage) {
        for (int i = 0; i < _nextControlPointIndex - 1; i++) {
            cvLine(pImage, _controlPoints[i], _controlPoints[i+1], LINE_COLOR);
        }
        
        if (_nextControlPointIndex == CPC_NUM_CONTROL_POINTS) {
            cvLine(pImage, _controlPoints[_nextControlPointIndex-1], _controlPoints[0], LINE_COLOR);
        }
        
        for (int i = 0; i < _nextControlPointIndex; i++) {
            if (_controlPointMoving && _movingControlPointIndex == i) {
                cvCircle(pImage, _controlPoints[i], 4, CV_RGB(255,0,0), -1);
            } else {
                cvCircle(pImage, _controlPoints[i], 4, CV_RGB(0,0,255), -1);
            }
        }
    }
    
    CvMat *getPerspectiveTransformForSize(CvSize pSize) {
        if (4 == _nextControlPointIndex) {
            if (NULL == _perspectiveTransform) {
                sortControlPoints();
                
                CvPoint2D32f lSourcePoints[CPC_NUM_CONTROL_POINTS];
                CvPoint2D32f lDestPoints[CPC_NUM_CONTROL_POINTS] = { {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0} };
                
                lDestPoints[1].x = pSize.width;
                lDestPoints[2].x = pSize.width; lDestPoints[2].y = pSize.height;
                lDestPoints[3].y = pSize.height;

                for (int i= 0; i < CPC_NUM_CONTROL_POINTS; i++) {
                    lSourcePoints[i] = cvPointTo32f(_controlPoints[i]);
                }

                _perspectiveTransform = cvCreateMat(3, 3, CV_32FC1);
                cvGetPerspectiveTransform(lSourcePoints, lDestPoints, _perspectiveTransform);
            }
        }

        return _perspectiveTransform;
    }
    
    void invalidatePerspectiveTransform() {
        cvReleaseMat(&_perspectiveTransform);
        _perspectiveTransform = NULL;
    }
    
    void sortControlPoints() {
        // Sort the control points to that the order is UL, UR, LR, LL.
        CvPoint swapPoint;
        
        for (int i = 1; i < _nextControlPointIndex; i++) {
            for (int j = i; j > 0; j--) {
                if (_controlPoints[j-1].y > _controlPoints[j].y) {
                    swapPoint = _controlPoints[j-1];
                    _controlPoints[j-1] = _controlPoints[j];
                    _controlPoints[j] = swapPoint;
                } else {
                    break;
                }
            }
        }
        
        if (_controlPoints[0].x > _controlPoints[1].x) {
            swapPoint = _controlPoints[0];
            _controlPoints[0] = _controlPoints[1];
            _controlPoints[1] = swapPoint;
        }
        
        if (_controlPoints[3].x > _controlPoints[2].x) {
            swapPoint = _controlPoints[2];
            _controlPoints[2] = _controlPoints[3];
            _controlPoints[3] = swapPoint;
        }
    }
};

ControlPointController *ControlPointController::_sharedControlPointController = 0;

#define NUM_STRINGS 5
#define INVERSE_PERSPECTIVE_IMAGE_WIDTH 200
#define PLOT_WIDTH 100

int gSelectedString = 3;
int gHighlightedString = 3;
int gStringWidth = INVERSE_PERSPECTIVE_IMAGE_WIDTH / NUM_STRINGS;

void stringSelectorMouseCallback(int event, int x, int y, int flags, void *param) { 
    if (CV_EVENT_LBUTTONDOWN == event) {
        gSelectedString = x / gStringWidth;
    } else if (CV_EVENT_MOUSEMOVE == event) {
        gHighlightedString = x / gStringWidth;
    }   
}

CvRect insetRect(CvRect pRect, int pDx, int pDy) {
    return cvRect(pRect.x + pDx, pRect.y + pDy, pRect.width - 2*pDx, pRect.height - 2*pDy);
}

CvRect rectForStringAndHeight(int pString, int pHeight) {
    return cvRect(gStringWidth*pString, 0, gStringWidth-1, pHeight-1);
}

void drawStringHighlightAndSelection(IplImage *pImage) {
    // Draw selection rectangle
    CvRect lSelectedStringRect = rectForStringAndHeight(gSelectedString, pImage->height);
    cvRectangle(pImage, cvPoint(lSelectedStringRect.x, lSelectedStringRect.y), cvPoint(lSelectedStringRect.x+lSelectedStringRect.width, lSelectedStringRect.y+lSelectedStringRect.height), CV_RGB(255,0,0));
    if (gHighlightedString != gSelectedString) {
        CvRect lHighlightedStringRect = rectForStringAndHeight(gHighlightedString, pImage->height);
        cvRectangle(pImage, cvPoint(lHighlightedStringRect.x, lHighlightedStringRect.y), cvPoint(lHighlightedStringRect.x+lHighlightedStringRect.width, lHighlightedStringRect.y+lHighlightedStringRect.height), LINE_COLOR);
    }
}

int main( int argc, char** argv )
{
    CvCapture* capture = 0;
    IplImage *frame, *frame_copy = 0;
    const char* input_name;
    
    input_name = argc > 1 ? argv[1] : 0;
    
    // Set up mouse handler for specifying the perspective points
    
    if( !input_name || (isdigit(input_name[0]) && input_name[1] == '\0') )
        capture = cvCaptureFromCAM( !input_name ? 0 : input_name[0] - '0' );
    else
        capture = cvCaptureFromAVI( input_name ); 

    cvNamedWindow( "Source", 1 );
    cvNamedWindow("Inverse Perspective", 1);
    cvNamedWindow("Single String", 1);
    cvNamedWindow("Plot", 1);
    cvSetMouseCallback("Source", mouseCallback);
    cvSetMouseCallback("Inverse Perspective", stringSelectorMouseCallback);

    if( capture )
    {
        for(;;)
        {
            if( !cvGrabFrame( capture ))
                break;
            frame = cvRetrieveFrame( capture );
            if( !frame )
                break;
            if( !frame_copy )
                frame_copy = cvCreateImage( cvSize(frame->width,frame->height),
                                            IPL_DEPTH_8U, frame->nChannels );
            if( frame->origin == IPL_ORIGIN_TL )
                cvCopy( frame, frame_copy, 0 );
            else
                cvFlip( frame, frame_copy, 0 );
            
            draw( frame_copy );

            int lKey = cvWaitKey( 10 );
            
            if( 'q' == lKey )
                break;
            else if ('x' == lKey)
                ControlPointController::sharedControlPointController()->clearControlPoints();
            
        }

        cvReleaseImage( &frame_copy );
        cvReleaseCapture( &capture );
    }
    else
    {
        const char* filename = input_name ? input_name : (char*)"lena.jpg";
        IplImage* image = cvLoadImage( filename, 1 );

        if( image )
        {
            draw( image );
            cvWaitKey(0);
            cvReleaseImage( &image );
        }
        else
        {
            /* assume it is a text file containing the
               list of the image filenames to be processed - one per line */
            FILE* f = fopen( filename, "rt" );
            if( f )
            {
                char buf[1000+1];
                while( fgets( buf, 1000, f ) )
                {
                    int len = (int)strlen(buf);
                    while( len > 0 && isspace(buf[len-1]) )
                        len--;
                    buf[len] = '\0';
                    image = cvLoadImage( buf, 1 );
                    if( image )
                    {
                        draw( image );
                        cvWaitKey(0);
                        cvReleaseImage( &image );
                    }
                }
                fclose(f);
            }
        }

    }
    
    cvDestroyWindow("Source");
    cvDestroyWindow("Inverse Perspective");
    cvDestroyWindow("Single String");
    cvDestroyWindow("Plot");

    return 0;
}

void mouseCallback(int event, int x, int y, int flags, void *param) {
    if (CV_EVENT_LBUTTONDOWN == event) {
        ControlPointController *lCPController = ControlPointController::sharedControlPointController();
        
        if (lCPController->isMovingControlPoint()) {
            lCPController->addControlPoint(x,y);
            lCPController->endMovingControlPoint();
        } else {   
            int lIndex = lCPController->controlPointIndexForCoordinates(x, y);

            if (CPC_NO_CONTROL_POINT == lIndex) {
                lIndex = lCPController->addControlPoint(x,y);
            } else {
                lCPController->startMovingControlPointWithIndex(lIndex);
            }
        }  
    }
}

void draw( IplImage* img )
{
    // static CvScalar colors[] = 
    // {
    //     {{0,0,255}},
    //     {{0,128,255}},
    //     {{0,255,255}},
    //     {{0,255,0}},
    //     {{255,128,0}},
    //     {{255,255,0}},
    //     {{255,0,0}},
    //     {{255,0,255}}
    // };
    // 
    // double scale = 1.3;
    // IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    // IplImage* small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
    //                      cvRound (img->height/scale)),
    //                  8, 1 );
    // int i;
    // 
    // cvCvtColor( img, gray, CV_BGR2GRAY );
    // cvResize( gray, small_img, CV_INTER_LINEAR );
    // cvEqualizeHist( small_img, small_img );
    // cvClearMemStorage( storage );
    // 
    // if( cascade )
    // {
    //     double t = (double)cvGetTickCount();
    //     CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
    //                                         1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/,
    //                                         cvSize(30, 30) );
    //     t = (double)cvGetTickCount() - t;
    //     printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
    //     for( i = 0; i < (faces ? faces->total : 0); i++ )
    //     {
    //         CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
    //         CvPoint center;
    //         int radius;
    //         center.x = cvRound((r->x + r->width*0.5)*scale);
    //         center.y = cvRound((r->y + r->height*0.5)*scale);
    //         radius = cvRound((r->width + r->height)*0.25*scale);
    //         cvCircle( img, center, radius, colors[i%8], 3, 8, 0 );
    //     }
    // }
    
    ControlPointController *lCPController = ControlPointController::sharedControlPointController();
    
    CvSize lTransformedImageSize;
    lTransformedImageSize.width = INVERSE_PERSPECTIVE_IMAGE_WIDTH;
    lTransformedImageSize.height = img->height;
    
    CvMat *lPerspectiveTransform = lCPController->getPerspectiveTransformForSize(lTransformedImageSize);
    
    IplImage* lTransformedImage = cvCreateImage( lTransformedImageSize, IPL_DEPTH_8U, img->nChannels); 
    
    if (NULL != lPerspectiveTransform) {
        cvWarpPerspective(img, lTransformedImage, lPerspectiveTransform);
        drawStringHighlightAndSelection(lTransformedImage);      
    } else {
        // draw an X in the window
        // cvLine(lTransformedImage, cvPoint(0.0,0.0), cvPoint(lTransformedImageSize.width,lTransformedImageSize.height), LINE_COLOR);
        // cvLine(lTransformedImage, cvPoint(0.0,lTransformedImageSize.height), cvPoint(lTransformedImageSize.width,0.0), LINE_COLOR);
    }
    
    cvShowImage( "Inverse Perspective", lTransformedImage); 
    
    if (NULL != lPerspectiveTransform) {
        CvRect lClippedStringImageRect = insetRect(rectForStringAndHeight(gSelectedString, lTransformedImage->height), 9, 0);
        IplImage* lClippedStringImage = cvCreateImage(cvSize(lClippedStringImageRect.width, lClippedStringImageRect.height), IPL_DEPTH_8U, img->nChannels);
        cvSetImageROI(lTransformedImage, lClippedStringImageRect);
        //cout << "ROI: " << lClippedStringImageRect.x << " " << lClippedStringImageRect.y << " " << lClippedStringImageRect.width << " " << lClippedStringImageRect.height << endl, 
        cvCopy(lTransformedImage, lClippedStringImage);
        cvShowImage("Single String", lClippedStringImage);
        
        CvMat *lRed = cvCreateMat(lClippedStringImage->height, lClippedStringImage->width, CV_8UC1);
        CvMat *lGreen = cvCreateMat(lClippedStringImage->height, lClippedStringImage->width, CV_8UC1);
        CvMat *lBlueAndGray = cvCreateMat(lClippedStringImage->height, lClippedStringImage->width, CV_8UC1);
        CvMat *lTemp = cvCreateMat(lClippedStringImage->height, lClippedStringImage->width, CV_8UC1);
        
        cvSplit(lClippedStringImage, lRed, lGreen, lBlueAndGray, NULL);
        cvMax(lBlueAndGray, lGreen, lTemp);
        cvMax(lRed, lTemp, lBlueAndGray);
        
        CvMat *lPseudoRowLuminance = cvCreateMat(lClippedStringImage->height, 1, CV_8UC1);
        CvMat *lPseudoRowLuminance2 = cvCreateMat(lClippedStringImage->height, 1, CV_8UC1);
        CvMat *lTempHeader = cvCreateMatHeader(1, lClippedStringImage->width, CV_32FC1);
        
        for (int i = 0; i < lClippedStringImage->height; i++) {
            cvSet2D(lPseudoRowLuminance, i, 0, cvScalar(cvAvg(cvGetRow(lBlueAndGray, lTempHeader, i)).val[0]));
        }

        CvMat *lKernel = cvCreateMat(5, 5, CV_8UC1);
        cvSet(lKernel, cvRealScalar(1));
        
        cvNormalize(lPseudoRowLuminance, lPseudoRowLuminance, PLOT_WIDTH, 0, CV_MINMAX);
        //cvThreshold(lPseudoRowLuminance, lPseudoRowLuminance2, 60.0, 90.0, CV_THRESH_BINARY);
        cvSmooth(lPseudoRowLuminance, lPseudoRowLuminance2, CV_GAUSSIAN, 7);
        
        IplImage* lPlot = cvCreateImage(cvSize(PLOT_WIDTH, lClippedStringImage->height), IPL_DEPTH_8U, img->nChannels);
        
        for (int i = 0; i < cvGetSize(lPseudoRowLuminance2).height - 1; i++) {
            cvLine(lPlot, cvPoint(cvGet2D(lPseudoRowLuminance2, i, 0).val[0], i), cvPoint(cvGet2D(lPseudoRowLuminance2, i+1, 0).val[0], i+1), LINE_COLOR);
        }
        
        cvReleaseMat(&lPseudoRowLuminance);
        cvReleaseMat(&lPseudoRowLuminance2);
        cvReleaseMat(&lTempHeader);
        cvReleaseMat(&lRed);
        cvReleaseMat(&lGreen);
        cvReleaseMat(&lBlueAndGray);
        cvReleaseMat(&lTemp);
        
        cvShowImage("Plot", lPlot );
        cvReleaseImage(&lPlot);
        
        cvReleaseImage( &lClippedStringImage );
    }
    
    lCPController->drawControlPoints(img);
    cvShowImage( "Source", img );
    
    cvReleaseImage( &lTransformedImage );
}
