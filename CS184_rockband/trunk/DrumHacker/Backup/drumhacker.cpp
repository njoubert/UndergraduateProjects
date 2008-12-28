#include "global.h"

#include "ControlPointController.h"

#define INVERSE_PERSPECTIVE_IMAGE_WIDTH 200


ControlPointController *ControlPointController::sharedControlPointController = 0;


/*
int gSelectedString = 3;
int gHighlightedString = 3;
int gStringWidth = INVERSE_PERSPECTIVE_IMAGE_WIDTH / NUM_STRINGS;

CvRect rectForStringAndHeight(int pString, int pHeight) {
    return cvRect(gStringWidth*pString, 0, gStringWidth-1, pHeight-1);
}
*/
void mouseCallback(int event, int x, int y, int flags, void *param) {
    if (CV_EVENT_LBUTTONDOWN == event) {
        //cout << "LBUTTON" << endl;
        ControlPointController *CPController = ControlPointController::getSharedControlPointController();
        
        if (CPController->isMovingControlPoint()) {
            CPController->addControlPoint(x,y);
            CPController->endMovingControlPoint();
        } else {   
            int index = CPController->controlPointIndexForCoordinates(x, y);

            if (CPC_NO_CONTROL_POINT == index) {
                index = CPController->addControlPoint(x,y);
            } else {
                CPController->startMovingControlPointWithIndex(index);
            }
        }  
    }
}
/*
void stringSelectorMouseCallback(int event, int x, int y, int flags, void *param) { 
    if (CV_EVENT_LBUTTONDOWN == event) {
        gSelectedString = x / gStringWidth;
        //StringAnalyzer::debugSetStringToDisplay(gSelectedString);
    } else if (CV_EVENT_MOUSEMOVE == event) {
        gHighlightedString = x / gStringWidth;
    }   
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
*/

int main() {
  
  IplImage *frame;//, *frame_copy;
  CvMat *perspectiveTransform;
  IplImage* transformedImage;
  int key = 0;

  CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
  if( !capture ) {
    fprintf( stderr, "ERROR: capture is NULL \n" );
    getchar();
    return -1;
  }

  // Create a window in which the captured images will be presented
  cvNamedWindow( "Source", CV_WINDOW_AUTOSIZE );
  cvNamedWindow( "Inverse Perspective", CV_WINDOW_AUTOSIZE );

  cvSetMouseCallback("Source", mouseCallback);

  // Show the image captured from the camera in the window and repeat
  while( 1 ) {
    // Get one frame
    frame = cvQueryFrame( capture );
    if( !frame ) {
      fprintf( stderr, "ERROR: frame is null...\n" );
      getchar();
      break;
    }

	//frame_copy = frame;

	ControlPointController *CPController = ControlPointController::getSharedControlPointController();

	CvSize transformedImageSize;
	transformedImageSize.width = INVERSE_PERSPECTIVE_IMAGE_WIDTH;
	transformedImageSize.height = frame->height;
	perspectiveTransform = CPController->getPerspectiveTransformForSize(transformedImageSize);
	transformedImage = cvCreateImage( transformedImageSize, IPL_DEPTH_8U, frame->nChannels); 

	if (NULL != perspectiveTransform) {
	   	cvWarpPerspective(frame, transformedImage, perspectiveTransform);
	}
	//drawStringHighlightAndSelection(transformedImage);
    cvShowImage( "Inverse Perspective", transformedImage);
    cvReleaseImage( &transformedImage );
    //cvShowImage( "Source", frame );

	CPController->drawControlPoints(frame);
    cvShowImage( "Source", frame );
    //cvReleaseImage ( &frame_copy );

    //If ESC key pressed
	key = cvWaitKey(2);
	if( key == 27 ) {
		break;
	} else if ( key == 'x' ) {
	    ControlPointController::getSharedControlPointController()->clearControlPoints();
	} else if ( key == 13 ) {
		cout << "HEAVEN OR HELL LET'S ROCK!" << endl;
	}
  }

  // Release the capture device, housekeeping
  cvReleaseCapture( &capture );
  cvDestroyWindow( "Source" );
  cvDestroyWindow( "Inverse Perspective" );
  return 0;
}