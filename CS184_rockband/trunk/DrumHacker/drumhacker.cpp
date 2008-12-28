#include "global.h"

#include "ControlPointController.h"
#include "StringAnalyzer.h"
#include "DrumTimer.h"
#include "NoteDetector.h"

#define INVERSE_PERSPECTIVE_IMAGE_WIDTH 320

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

ControlPointController *ControlPointController::sharedControlPointController = 0;
int dStringWidth = INVERSE_PERSPECTIVE_IMAGE_WIDTH / NUM_STRINGS;


CvRect rectForStringAndHeight(int pString, int pHeight) {
    return cvRect(dStringWidth*pString, 0, dStringWidth-1, pHeight-1);
}

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
        gSelectedString = x / dStringWidth;
        StringAnalyzer::debugSetStringToDisplay(gSelectedString);
    } else if (CV_EVENT_MOUSEMOVE == event) {
        gHighlightedString = x / dStringWidth;
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

  bool play = false;

  int			error=0;
  TaskHandle	taskHandle=0;
  uInt32		data=0x00000000;
  char			errBuff[2048]={'\0'};
  int32			written;

  /*********************************************/
  // DAQmx Configure Code
  /*********************************************/
  DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
  DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev1/port0","",DAQmx_Val_ChanForAllLines));
  //DAQmxErrChk (DAQmxSetDOOutputDriveType(taskHandle, "Dev1/port0", DAQmx_Val_ActiveDrive));

  /*********************************************/
  // DAQmx Start Code
  /*********************************************/
  DAQmxErrChk (DAQmxStartTask(taskHandle));

  /*********************************************/
  // DAQmx Write Code
  /*********************************************/
  DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));

Error:
  if( DAQmxFailed(error) )
	DAQmxGetExtendedErrorInfo(errBuff,2048);
  if( DAQmxFailed(error) )
	printf("DAQmx Error: %s\n",errBuff);

  
  IplImage *frame;//, *frame_copy;
  CvMat *perspectiveTransform;
  IplImage* transformedImage;
  int key = 0;

  ofstream recordFile;
  recordFile.open("Notes.txt");
  recordFile << "Notes to Hit\n\n";
  bool record = false;

  int frameNum = 0;
  string filename = "frame";
  string fileformat = ".bmp";
  char strNum[10];
  _itoa(frameNum,strNum,10);
  string fullFileName = filename + strNum + fileformat;
  char output[50];
  strcpy(output,fullFileName.c_str());
  bool takepicture = false;

  CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);//cvCaptureFromCAM( 0 );

  if( !capture ) {
	fprintf( stderr, "ERROR: capture is NULL \n" );
	getchar();
	return -1;
  }
  //cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,640); //320,640,960
  //cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,480);//240,480,720

  DrumTimer* drumTimer = DrumTimer::getInstance();

  //These are the stringAnalyzers for each string.
  StringAnalyzer stringAnalyzer0(0);
  StringAnalyzer stringAnalyzer1(1);
  StringAnalyzer stringAnalyzer2(2);
  StringAnalyzer stringAnalyzer3(3);

  int detectIndex;
  noteHits_t notes;
  notes.pos = -1;
  queue<noteHits_t> noteQueue;
  noteHits_t noteArray[100];
  int noteSpeed = 0; //pixels per loop
  int numNotes = 0;
  int numReds = 0;
  int numYellows = 0;
  int numBlues = 0;
  int numGreens = 0;
  int numBass = 0;
  int loopCount = 0;
  double delay = 5.75;
  double timeToHitSum = 0.0;

  // Create a window in which the captured images will be presented
  cvNamedWindow( "Source", CV_WINDOW_AUTOSIZE );
  cvNamedWindow( "Inverse Perspective", CV_WINDOW_AUTOSIZE );

  cvNamedWindow("Buffer 1", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Buffer 2", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Buffer 3", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Buffer 4", CV_WINDOW_AUTOSIZE);

  cvSetMouseCallback("Source", mouseCallback);
  //cvSetMouseCallback("Inverse Perspective", stringSelectorMouseCallback);

  // Show the image captured from the camera in the window and repeat
  while( 1 ) {
    // Get one frame

    frame = cvQueryFrame( capture );
    if( !frame ) {
      fprintf( stderr, "ERROR: frame is null...\n" );
      getchar();
      break;
    }

	drumTimer->frameArrived();

	detectIndex = frame->height*0.75;
	//frame_copy = frame;

	ControlPointController *CPController = ControlPointController::getSharedControlPointController();

	CvSize transformedImageSize;
	transformedImageSize.width = INVERSE_PERSPECTIVE_IMAGE_WIDTH;
	transformedImageSize.height = frame->height;
	perspectiveTransform = CPController->getPerspectiveTransformForSize(transformedImageSize);
	transformedImage = cvCreateImage( transformedImageSize, IPL_DEPTH_8U, frame->nChannels); 

	if (NULL != perspectiveTransform) {
	   	cvWarpPerspective(frame, transformedImage, perspectiveTransform);

		// Create the 3 channels of the images
        CvMat *red = cvCreateMat(transformedImage->height, transformedImage->width, CV_8UC1);
        CvMat *green = cvCreateMat(transformedImage->height, transformedImage->width, CV_8UC1);
        CvMat *blue = cvCreateMat(transformedImage->height, transformedImage->width, CV_8UC1);
        CvMat *temp = cvCreateMat(transformedImage->height, transformedImage->width, CV_8UC1);
        cvSplit(transformedImage, red, green, blue, NULL);
        
        IplImage *maxRGB = cvCreateImage(cvSize(transformedImage->width, transformedImage->height), IPL_DEPTH_8U, 1);
		//IplImage *maxRGB2 = cvCreateImage(cvSize(transformedImage->width, transformedImage->height), IPL_DEPTH_8U, 1);
		
        // collapse the 3 channels into one matrix of luminance values
        cvMax(blue, green, temp);
        cvMax(red, temp, maxRGB);
		
		cvConvertScale(red, red, 0.4, 0);
		cvConvertScale(green, green, 0.4, 0);
		cvConvertScale(blue, blue, 0.2, 0);
		cvAdd(red, green, temp);
		cvAdd(blue, temp, maxRGB);

        // Calculate length of note from trapezoid input...
        double ratio = CPController->getParallelogramRatio();
		//cout << ratio << endl;
        int estimatedNoteLength = (int) (ratio * 7);
        if (estimatedNoteLength > 99) //Ugly hack - it crashes if its bigger... too tired to figure out why right now
        	estimatedNoteLength = 99;
		//cout << estimatedNoteLength << endl;

		//CvMat *lum0, *lum1, *lum2, *lum3;
		
		
		vector<peak_t> peaks0 = stringAnalyzer0.analyzeFrame(maxRGB, estimatedNoteLength, NoteDetector::noteWidth);
		vector<peak_t> peaks1 = stringAnalyzer1.analyzeFrame(maxRGB, estimatedNoteLength, NoteDetector::noteWidth);
		vector<peak_t> peaks2 = stringAnalyzer2.analyzeFrame(maxRGB, estimatedNoteLength, NoteDetector::noteWidth);
		vector<peak_t> peaks3 = stringAnalyzer3.analyzeFrame(maxRGB, estimatedNoteLength, NoteDetector::noteWidth);
		
		noteHits_t nextNotes = NoteDetector::detectNotes(peaks0, peaks1, peaks2, peaks3, detectIndex, drumTimer->getDeltaP());
		//cout << nextNotes.hitRed << " " << nextNotes.hitYellow << " " << nextNotes.hitBlue << " " << nextNotes.hitGreen << " " << nextNotes.hitBass << " -- " << nextNotes.pos << endl;
		
		if ((nextNotes.pos <= notes.pos || nextNotes.pos-notes.pos < drumTimer->getDeltaP()/3) && numNotes > 0) {
			noteHits_t notesFinal;
			notesFinal.hitRed = ((float)numReds/numNotes) >= 0.5;
			notesFinal.hitYellow = ((float)numYellows/numNotes) >= 0.5;
			notesFinal.hitBlue = ((float)numBlues/numNotes) >= 0.5;
			notesFinal.hitGreen = ((float)numGreens/numNotes) >= 0.5;
			notesFinal.hitBass = numBass > 0;
			notesFinal.pos = notes.pos;
			int timeToHit = (int)((double)loopCount+(double)(timeToHitSum/numNotes)-delay+0.5)%100;
			if (timeToHit < 0.0)
				timeToHit = 0;
			notesFinal.timeToHit = timeToHit;
			if (!StringAnalyzer::startup && play) {
				//if (noteArray[notesFinal.timeToHit].timeToHit == notesFinal.timeToHit)
				//	cout << "Overwriting\n";
				noteArray[notesFinal.timeToHit] = notesFinal;
			}
			numNotes = 0;
			numReds = 0;
			numYellows = 0;
			numBlues = 0;
			numGreens = 0;
			numBass = 0;
			timeToHitSum = 0.0;
		} else if (nextNotes.pos > 0 && nextNotes.pos > notes.pos) {
			if (nextNotes.hitRed)
				numReds++;
			if (nextNotes.hitYellow)
				numYellows++;
			if (nextNotes.hitBlue)
				numBlues++;
			if (nextNotes.hitGreen)
				numGreens++;
			if (nextNotes.hitBass)
				numBass++;
			timeToHitSum = timeToHitSum+(frame->height-nextNotes.pos)/drumTimer->getDeltaP();
			numNotes++;
		}
		notes = nextNotes;
		if (play) {
			noteHits_t front = noteArray[loopCount];
			if (front.timeToHit == loopCount) {
				if (front.hitRed)
					data+=0x00000001;
				if (front.hitYellow)
					data+=0x00000002;
				if (front.hitBlue)
					data+=0x00000004;
				if (front.hitGreen)
					data+=0x00000008;
				if (front.hitBass)
					data+=0x00000010;
				DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
				Sleep(30);
				data=0x00000000;
				DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
				noteArray[loopCount].timeToHit = -1;
				//cout  << "Actual hit: " << front.hitRed << " " << front.hitYellow << " " << front.hitBlue << " " << front.hitGreen << " " << front.hitBass << " -- " << front.pos << " | " << loopCount << endl;
			}
		}

		cvReleaseMat(&red);
        cvReleaseMat(&green);
        cvReleaseMat(&blue);
        cvReleaseMat(&temp);

        cvReleaseImage(&maxRGB);
	}
	if (takepicture) {
		if(!cvSaveImage(output,transformedImage)) printf("Could not save: %s\n",output);
		frameNum++;
		_itoa(frameNum,strNum,10);
		fullFileName = filename + strNum + fileformat;
		strcpy(output,fullFileName.c_str());
	}

	//drawStringHighlightAndSelection(transformedImage);
	cvLine(transformedImage, cvPoint(0,detectIndex), cvPoint(INVERSE_PERSPECTIVE_IMAGE_WIDTH,detectIndex), cvScalar(0,255,0), 1);
	//cvLine(transformedImage, cvPoint(0,195), cvPoint(PLOT_WIDTH,195), cvScalar(255,0,0), 1);
    cvShowImage( "Inverse Perspective", transformedImage);
    cvReleaseImage( &transformedImage );

	drumTimer->frameDone();
    //cvShowImage( "Source", frame );

	CPController->drawControlPoints(frame);
    cvShowImage( "Source", frame );
    //cvReleaseImage ( &frame_copy );


	data = 0x00000000;
    //If ESC key pressed
	key = cvWaitKey(10);
	if( key == 27 ) {
		break;
	} else if ( key == 'x' ) {
	    ControlPointController::getSharedControlPointController()->clearControlPoints();
	} else if ( key == 13 ) {
		if (!play) {
			StringAnalyzer::startup = true;
			loopCount = 0;
			cout << "LET'S DO THIS!!" << endl;
		}
		else {
			cout << "SHIT yeah!" << endl;
		}
		play = !play;
		//StringAnalyzer::startupDone();
	} else if (key == '=') {
		delay+=0.1;
		cout << "delay set to: " << delay << endl;
	} else if (key == '-') {
		delay-=0.1;
		cout << "delay set to: " << delay << endl;
	} else if (key == 'r') {
		cout << "Recording started\n";
		record = true;
	} else if (key == 't') {
		record = false;
		cout << "Recording stopped\n";
	} else if (key == 'p') {
		takepicture = !takepicture;
	}

	if (key == 'a') {
		data+=0x00000001;
	} 
	if (key == 'd') {
		data+=0x00000002;
	}
	if (key == 'h') {
		data+=0x00000004;
	}
	if (key == 'k') {
		data+=0x00000008;
	}
	if (key == ' ') {
		data+=0x00000010;
	}
	if (key == 'b') {
		data+=0x00000013;
	}
	if (key == '1') {
		for (int i=0; i < 8; i++) {
			data^=0x00000001;
			DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
			Sleep(100);
		}
	}
	if (key == '2') {
		for (int i=0; i < 4; i++) {
			data^=0x00000001;
			DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
			Sleep(100);
		}
		for (int i=0; i < 4; i++) {
			data^=0x00000002;
			DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
			Sleep(100);
		}
		for (int i=0; i < 4; i++) {
			data^=0x00000004;
			DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
			Sleep(100);
		}
	}
	if (key == '3') {
		for (int i=0; i < 2; i++) {
			data^=0x00000001;
			DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
			Sleep(100);
		}
		for (int i=0; i < 2; i++) {
			data^=0x00000002;
			DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
			Sleep(100);
		}
		for (int i=0; i < 2; i++) {
			data^=0x00000001;
			DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
			Sleep(100);
		}
		for (int i=0; i < 2; i++) {
			data^=0x00000004;
			DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
			Sleep(100);
		}
		for (int i=0; i < 2; i++) {
			data^=0x00000001;
			DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
			Sleep(100);
		}
		for (int i=0; i < 4; i++) {
			data^=0x00000006;
			DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
			Sleep(100);
		}
	}
	DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
		
	

	if (loopCount == 99 && StringAnalyzer::startup) {
		StringAnalyzer::startupDone();
		cout << "Startup Complete!\n";
	}
	if (play) {
		loopCount = (loopCount+1)%100;
	}
	
  }
  
  recordFile.close();

  // Release the capture device, housekeeping
  cvReleaseCapture( &capture );
  cvDestroyWindow( "Source" );
  cvDestroyWindow( "Inverse Perspective" );
  cvDestroyWindow("Buffer 1");
  cvDestroyWindow("Buffer 2");
  cvDestroyWindow("Buffer 3");
  cvDestroyWindow("Buffer 4");

  if( taskHandle!=0 ) {
	/*********************************************/
	// DAQmx Stop Code
	/*********************************************/
	DAQmxStopTask(taskHandle);
	DAQmxClearTask(taskHandle);
  }
  
  return 0;
}