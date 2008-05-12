#include "global.h"

#include "Guitar.h"
#include "StringAnalyzer.h"
#include "GuitarTimer.h"
#include "utils.h"

#ifdef _EiC
#define WIN32
#endif

using namespace std;

void analyzeRawImage( CvCapture* );
void mouseCallback(int event, int x, int y, int flags, void* param);

Guitar guitar = Guitar();

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
    
    double getParallelogramRatio() {
    	sortControlPoints();
    	double width = ((double)(_controlPoints[0].x + _controlPoints[3].x))/2 - ((double)(_controlPoints[1].x + _controlPoints[2].x))/2;
    	double height = ((double)(_controlPoints[0].y + _controlPoints[1].y))/2 - ((double)(_controlPoints[2].y + _controlPoints[3].y))/2;
    	return width/height;
    }
};

ControlPointController *ControlPointController::_sharedControlPointController = 0;

#define INVERSE_PERSPECTIVE_IMAGE_WIDTH 200

int gSelectedString = 3;
int gHighlightedString = 3;
int gStringWidth = INVERSE_PERSPECTIVE_IMAGE_WIDTH / NUM_STRINGS;

void stringSelectorMouseCallback(int event, int x, int y, int flags, void *param) { 
    if (CV_EVENT_LBUTTONDOWN == event) {
        gSelectedString = x / gStringWidth;
        StringAnalyzer::debugSetStringToDisplay(gSelectedString);
    } else if (CV_EVENT_MOUSEMOVE == event) {
        gHighlightedString = x / gStringWidth;
    }   
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
    
    guitar.connect("/dev/ttyUSB0");
    
    input_name = argc > 1 ? argv[1] : 0;
    
    if( !input_name || (isdigit(input_name[0]) && input_name[1] == '\0') )
        capture = cvCaptureFromCAM( !input_name ? 0 : input_name[0] - '0' );
    else
        capture = cvCaptureFromAVI( input_name ); 

    cvNamedWindow( "Source", 1 );
    cvNamedWindow("Inverse Perspective", 1);
    cvNamedWindow("String", 1);
    cvNamedWindow("Plot 1", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Plot 2", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Plot 3", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Plot 4", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Plot 5", CV_WINDOW_AUTOSIZE);
    
    cvNamedWindow("Buffer 1", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Buffer 2", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Buffer 3", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Buffer 4", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Buffer 5", CV_WINDOW_AUTOSIZE);
    
    cvMoveWindow("Source", 0, 600);
    cvMoveWindow("Inverse Perspective", 650, 600);
    cvMoveWindow("String", 850, 600);
    cvMoveWindow("Plot 1", 0, 0);
    cvMoveWindow("Plot 2", PLOT_WIDTH, 0);
    cvMoveWindow("Plot 3", 2*PLOT_WIDTH, 0);
    cvMoveWindow("Plot 4", 3*PLOT_WIDTH, 0);
    cvMoveWindow("Plot 5", 4*PLOT_WIDTH, 0);
    cvMoveWindow("Buffer 1", 0, 600);
    cvMoveWindow("Buffer 2", PLOT_WIDTH, 600);
    cvMoveWindow("Buffer 3", 2*PLOT_WIDTH, 600);
    cvMoveWindow("Buffer 4", 3*PLOT_WIDTH, 600);
    cvMoveWindow("Buffer 5", 4*PLOT_WIDTH, 600);
    
    // Set up mouse handler for specifying the perspective points
    cvSetMouseCallback("Source", mouseCallback);
    cvSetMouseCallback("Inverse Perspective", stringSelectorMouseCallback);
	
    if (capture) {
        analyzeRawImage(capture);
    } else {
    	cout << "Could not start capturing! No capture device found or no input file specified!" << endl;	
    }
    
    guitar.disconnect();
    
    /*
     * Clean up
     */
    cvReleaseCapture( &capture );
    cvDestroyWindow("Source");
    cvDestroyWindow("Inverse Perspective");
    cvDestroyWindow("String");
    cvDestroyWindow("Plot 1");
    cvDestroyWindow("Plot 2");
    cvDestroyWindow("Plot 3");
    cvDestroyWindow("Plot 4");
    cvDestroyWindow("Plot 5");
    cvDestroyWindow("Buffer 1");
    cvDestroyWindow("Buffer 2");
    cvDestroyWindow("Buffer 3");
    cvDestroyWindow("Buffer 4");
    cvDestroyWindow("Buffer 5");

    return 0;
}

void mouseCallback(int event, int x, int y, int flags, void *param) {
    if (CV_EVENT_LBUTTONDOWN == event) {
        cout << "LBUTTON" << endl;
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

/**
 * Capture raw frames from the capture input (set up in main) and
 * apply the necessary perspective transforms and clipping to hand
 * images to StringAnalyzers.
 */
void analyzeRawImage( CvCapture* captureSource ) {
	
	IplImage *frame, *frame_copy = 0;
	
	//This is the global guitar timer:
	GuitarTimer* guitarTimer = GuitarTimer::getInstance();
	
	//These are the circular buffers for each string.
	NoteTracker noteTracker0(0);
	NoteTracker noteTracker1(1);
	NoteTracker noteTracker2(2);
	NoteTracker noteTracker3(3);
	NoteTracker noteTracker4(4);
	
	//These are the stringAnalyzers for each string.
	StringAnalyzer stringAnalyzer0(0, &noteTracker0);
	StringAnalyzer stringAnalyzer1(1, &noteTracker1);
	StringAnalyzer stringAnalyzer2(2, &noteTracker2);
	StringAnalyzer stringAnalyzer3(3, &noteTracker3);
	StringAnalyzer stringAnalyzer4(4, &noteTracker4);
	
	for(;;) {
		
        int lKey = 0;
        //while ('n' != lKey) { 
	        lKey = cvWaitKey( 2 );  
	        if( 'q' == lKey ) {
	            exit(0);
	        } else if ('=' == lKey) {
	        	NoteTracker::incCursor();
	        	//PeakDetector::demandedHalfWidthFactor = PeakDetector::demandedHalfWidthFactor + 0.1;
	        } else if ('-' == lKey) {
	        	NoteTracker::decCursor();
	        	//PeakDetector::demandedHalfWidthFactor = PeakDetector::demandedHalfWidthFactor - 0.1;
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
    	//}
        
        if( !cvGrabFrame( captureSource ))
            break;
        frame = cvRetrieveFrame( captureSource );
        if( !frame )
            break;
        
        guitarTimer->frameArrived();  
            
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
		        //drawStringHighlightAndSelection(lTransformedImage);  
		    } else {
		        // draw an X in the window
		        // cvLine(lTransformedImage, cvPoint(0.0,0.0), cvPoint(lTransformedImageSize.width,lTransformedImageSize.height), LINE_COLOR);
		        // cvLine(lTransformedImage, cvPoint(0.0,lTransformedImageSize.height), cvPoint(lTransformedImageSize.width,0.0), LINE_COLOR);
		    }
		    
		    /*
		     * At this point we want to have a perspective-transformed frame, out of which we can pick the strings one by one.
		     */
		    if (NULL != lPerspectiveTransform) {
		    	
		        // Create the 3 channels of the images
                CvMat *lRed = cvCreateMat(lTransformedImage->height, lTransformedImage->width, CV_8UC1);
                CvMat *lGreen = cvCreateMat(lTransformedImage->height, lTransformedImage->width, CV_8UC1);
                CvMat *lBlue = cvCreateMat(lTransformedImage->height, lTransformedImage->width, CV_8UC1);
                CvMat *lTemp = cvCreateMat(lTransformedImage->height, lTransformedImage->width, CV_8UC1);
                cvSplit(lTransformedImage, lRed, lGreen, lBlue, NULL);
                
                IplImage *lMaxRGB = cvCreateImage(cvSize(lTransformedImage->width, lTransformedImage->height), IPL_DEPTH_8U, 1);
				
                // collapse the 3 channels into one matrix of luminance values
                cvMax(lBlue, lGreen, lTemp);
                cvMax(lRed, lTemp, lMaxRGB);
		        
		        // Calculate length of note from trapezoid input...
		        double ratio = lCPController->getParallelogramRatio();
		        int estimatedNoteLength = (int) (ratio * 40);
		        if (estimatedNoteLength > 99) //Ugly hack - it crashes if its bigger... too tired to figure out why right now
		        	estimatedNoteLength = 99;
		        
		        stringAnalyzer0.analyzeFrame(lMaxRGB, estimatedNoteLength);
		        stringAnalyzer1.analyzeFrame(lMaxRGB, estimatedNoteLength);
		        stringAnalyzer2.analyzeFrame(lMaxRGB, estimatedNoteLength);
		        stringAnalyzer3.analyzeFrame(lMaxRGB, estimatedNoteLength);
		        stringAnalyzer4.analyzeFrame(lMaxRGB, estimatedNoteLength);
		        
		        guitar.invalidate();
		        guitar.keyG = noteTracker0.hit();
				guitar.keyR = noteTracker1.hit();
				guitar.keyY = noteTracker2.hit();
				guitar.keyB = noteTracker3.hit();
				guitar.keyO = noteTracker4.hit();
				if (guitar.keyG || guitar.keyR || guitar.keyY || guitar.keyB || guitar.keyO)
					guitar.keyPickDown = true;
				else
					guitar.keyPickDown = false;
				guitar.writeState();
				
                cvReleaseMat(&lRed);
                cvReleaseMat(&lGreen);
                cvReleaseMat(&lBlue);
                cvReleaseMat(&lTemp);
                cvReleaseImage(&lMaxRGB);
		    }
		    
		    drawStringHighlightAndSelection(lTransformedImage);
		    cvShowImage( "Inverse Perspective", lTransformedImage);
		    
		    guitarTimer->frameDone();
		    
		    lCPController->drawControlPoints(frame_copy);
		    cvShowImage( "Source", frame_copy );
		    
		    cvReleaseImage( &lTransformedImage );
            	
	}

    cvReleaseImage( &frame_copy );
        
       
}
