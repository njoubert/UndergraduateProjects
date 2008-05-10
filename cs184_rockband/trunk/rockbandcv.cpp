#include "global.h"

#include "StringAnalyzer.h"

#ifdef _EiC
#define WIN32
#endif

using namespace std;

void analyzeRawImage( CvCapture* );
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

/**
 * We set up the GUI windows, register event handlers, find the input source,
 * and pass this to analyzeRawImage to start image processing.
 */
int main( int argc, char** argv ) {
    CvCapture* capture = 0;
    const char* input_name;
    
    input_name = argc > 1 ? argv[1] : 0;
    
    if( !input_name || (isdigit(input_name[0]) && input_name[1] == '\0') )
        capture = cvCaptureFromCAM( !input_name ? 0 : input_name[0] - '0' );
    else
        capture = cvCaptureFromAVI( input_name ); 

    cvNamedWindow( "Source", 1 );
    cvNamedWindow("Inverse Perspective", 1);
    cvNamedWindow("String 1", 1);
//    cvNamedWindow("String 2", 1);
//    cvNamedWindow("String 3", 1);
//    cvNamedWindow("String 4", 1);
//    cvNamedWindow("String 5", 1);
    cvNamedWindow("Plot", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("DFT", CV_WINDOW_AUTOSIZE);
    
    // Set up mouse handler for specifying the perspective points
    cvSetMouseCallback("Source", mouseCallback);
    cvSetMouseCallback("Inverse Perspective", stringSelectorMouseCallback);
	
    if (capture) {
        analyzeRawImage(capture);
    } else {
    	cout << "Could not start capturing! No capture device found or no input file specified!" << endl;	
    }
    
    /*
     * Clean up
     */
    cvReleaseCapture( &capture );
    cvDestroyWindow("Source");
    cvDestroyWindow("Inverse Perspective");
    cvDestroyWindow("String 1");
//    cvDestroyWindow("String 2");
//    cvDestroyWindow("String 3");
//    cvDestroyWindow("String 4");
//    cvDestroyWindow("String 5");
    cvDestroyWindow("Plot");
    cvDestroyWindow("DFT");

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

void analyzeRawImage( CvCapture* captureSource ) {
	
	IplImage *frame, *frame_copy = 0;
	
	StringAnalyzer stringAnalyzer1(1);
	StringAnalyzer stringAnalyzer2(2);
	StringAnalyzer stringAnalyzer3(3);
	StringAnalyzer stringAnalyzer4(4);
	StringAnalyzer stringAnalyzer5(5);
	
	for(;;) {
		
        if( !cvGrabFrame( captureSource ))
            break;
        frame = cvRetrieveFrame( captureSource );
        if( !frame )
            break;
        if( !frame_copy )
            frame_copy = cvCreateImage( cvSize(frame->width,frame->height),
                                        IPL_DEPTH_8U, frame->nChannels );
        if( frame->origin == IPL_ORIGIN_TL )
            cvCopy( frame, frame_copy, 0 );
        else
            cvFlip( frame, frame_copy, 0 );
        
        	ControlPointController *lCPController = ControlPointController::sharedControlPointController();
    
		    CvSize lTransformedImageSize;
		    lTransformedImageSize.width = INVERSE_PERSPECTIVE_IMAGE_WIDTH;
		    lTransformedImageSize.height = frame_copy->height;
		    CvMat *lPerspectiveTransform = lCPController->getPerspectiveTransformForSize(lTransformedImageSize);
		    IplImage* lTransformedImage = cvCreateImage( lTransformedImageSize, IPL_DEPTH_8U, frame_copy->nChannels); 
		    
		    if (NULL != lPerspectiveTransform) {
		        cvWarpPerspective(frame_copy, lTransformedImage, lPerspectiveTransform);
		        drawStringHighlightAndSelection(lTransformedImage);      
		    } else {
		        // draw an X in the window
		        // cvLine(lTransformedImage, cvPoint(0.0,0.0), cvPoint(lTransformedImageSize.width,lTransformedImageSize.height), LINE_COLOR);
		        // cvLine(lTransformedImage, cvPoint(0.0,lTransformedImageSize.height), cvPoint(lTransformedImageSize.width,0.0), LINE_COLOR);
		    }
		    
		    cvShowImage( "Inverse Perspective", lTransformedImage); 
		    
		    /*
		     * At this point we want to have a perspective-transformed frame, out of which we can pick the strings one by one.
		     */
		    if (NULL != lPerspectiveTransform) {
		    	
		        CvRect lClippedStringImageRect = insetRect(rectForStringAndHeight(gSelectedString, lTransformedImage->height), 9, 0);
		        IplImage* lClippedStringImage1 = cvCreateImage(cvSize(lClippedStringImageRect.width, lClippedStringImageRect.height), IPL_DEPTH_8U, frame_copy->nChannels);
		        cvSetImageROI(lTransformedImage, lClippedStringImageRect);
		        //cout << "ROI: " << lClippedStringImageRect.x << " " << lClippedStringImageRect.y << " " << lClippedStringImageRect.width << " " << lClippedStringImageRect.height << endl, 
		        cvCopy(lTransformedImage, lClippedStringImage1);
		        cvShowImage("String 1", lClippedStringImage1);
		        
		        //Calculate length of note from trapezoid input...
		        int estimatedNoteLength = 15;
		        
		        stringAnalyzer1.analyzeFrame(lClippedStringImage1, estimatedNoteLength);
//		        stringAnalyzer2.analyzeFrame(lClippedStringImage2);
//		        stringAnalyzer3.analyzeFrame(lClippedStringImage3);
//		        stringAnalyzer4.analyzeFrame(lClippedStringImage4);
//		        stringAnalyzer5.analyzeFrame(lClippedStringImage5);

				int lKey = 0;
		        while ('n' != lKey) { 
			        lKey = cvWaitKey( 2 );  
			        if( 'q' == lKey ) {
			            exit(0);
			        } else if ('x' == lKey) {
			            ControlPointController::sharedControlPointController()->clearControlPoints();
			        } 
			        /** 
			        else if ('s' == lKey) {
			        	
			        	        CvFileStorage *lFile = cvOpenFileStorage(FRAME_LUMINANCE_FILE_NAME, NULL, CV_STORAGE_WRITE);
						        if (NULL != lFile) {
						        	
						        	for (int i = 0; i < cvGetSize(lPseudoRowLuminance2).height - 1; i++) {
						        		cvWriteReal(lFile, NULL, cvGet2D(lPseudoRowLuminance2, i, 0).val[0]);
			       					}
			 						printf("Wrote Luminance values to output file.\n");
						        } else {
						            printf("Could not write control point file: %s. Ignoring.\n", CONTROL_POINT_FILE_NAME);
						        }
						        
						        cvReleaseFileStorage(&lFile); 
		        	
		        	} //Save frame
		        	// */
		    	}
		
		        
		        cvReleaseImage( &lClippedStringImage1 );
//		        cvReleaseImage( &lClippedStringImage2 );
//		        cvReleaseImage( &lClippedStringImage3 );
//		        cvReleaseImage( &lClippedStringImage4 );
//		        cvReleaseImage( &lClippedStringImage5 );
		    
		    }
		    
		    lCPController->drawControlPoints(frame_copy);
		    cvShowImage( "Source", frame_copy );
		    
		    cvReleaseImage( &lTransformedImage );
            	
	}

    cvReleaseImage( &frame_copy );
        
       
}
