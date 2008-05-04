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

using namespace std;

static CvMemStorage* storage = 0;

void draw( IplImage* image );
void mouseCallback(int event, int x, int y, int flags, void* param);

int main( int argc, char** argv )
{
    CvCapture* capture = 0;
    IplImage *frame, *frame_copy = 0;
    const char* input_name;
    
    input_name = argc > 1 ? argv[1] : 0;
    
    storage = cvCreateMemStorage(0);
    
    // Set up mouse handler for specifying the perspective points
    
    if( !input_name || (isdigit(input_name[0]) && input_name[1] == '\0') )
        capture = cvCaptureFromCAM( !input_name ? 0 : input_name[0] - '0' );
    else
        capture = cvCaptureFromAVI( input_name ); 

    cvNamedWindow( "Source", 1 );
    cvNamedWindow("Inverse Perspective", 1);
    cvSetMouseCallback("Source", mouseCallback);

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

            if( cvWaitKey( 10 ) >= 0 )
                break;
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
    
    cvDestroyWindow("result");

    return 0;
}

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
    
    static ControlPointController *_sharedControlPointController;
    
    ControlPointController() {
        _nextControlPointIndex = 0;
        _perspectiveTransform = NULL;

        _controlPointRadius = 4;

        _controlPointMoving = false;
        _movingControlPointIndex = -1;
    }
public:
    
    static ControlPointController *sharedControlPointController() {
        if (NULL == _sharedControlPointController) {
            _sharedControlPointController = new ControlPointController();
        }
        
        return _sharedControlPointController;
    }
    
    int addControlPoint(int pX, int pY) { // This function is also called to move a control point. Returns the control point added/moved.
        // cout << "Add control point" << endl;
        if (!_controlPointMoving) {
            if (_nextControlPointIndex < CPC_NUM_CONTROL_POINTS) {
                _controlPoints[_nextControlPointIndex].x = pX;
                _controlPoints[_nextControlPointIndex].y = pY;
                int lAddedControlPointIndex = _nextControlPointIndex;
                _nextControlPointIndex++;
                return lAddedControlPointIndex;
            }
        } else {
            _controlPoints[_movingControlPointIndex].x = pX;
            _controlPoints[_movingControlPointIndex].y = pY;
            
            _perspectiveTransform = NULL;
            
            return _movingControlPointIndex;
        }
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
            cvLine(pImage, _controlPoints[i], _controlPoints[i+1], CV_RGB(192,192,192));
        }
        
        if (_nextControlPointIndex == CPC_NUM_CONTROL_POINTS) {
            cvLine(pImage, _controlPoints[_nextControlPointIndex-1], _controlPoints[0], CV_RGB(192,192,192));
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
    lTransformedImageSize.width = 200;
    lTransformedImageSize.height = img->height;
    
    CvMat *lPerspectiveTransform = lCPController->getPerspectiveTransformForSize(lTransformedImageSize);
    
    if (NULL != lPerspectiveTransform) {
        IplImage* lTransformedImage = cvCreateImage( lTransformedImageSize, IPL_DEPTH_8U, img->nChannels); 
        cvWarpPerspective(img, lTransformedImage, lPerspectiveTransform);
        cvShowImage( "Inverse Perspective", lTransformedImage);
        cvReleaseImage( &lTransformedImage );
    }
    
    lCPController->drawControlPoints(img);
    cvShowImage( "Source", img );
    
    // cvReleaseImage( &small_img );
}
