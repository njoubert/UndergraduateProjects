#include "ControlPointController.h"

ControlPointController::ControlPointController() {
    nextControlPointIndex = 0;
    perspectiveTransform = NULL;

    controlPointRadius = 4;

    controlPointMoving = false;
    movingControlPointIndex = -1;
    
    storage = cvCreateMemStorage(0);
    
    readControlPoints();
}

ControlPointController::~ControlPointController() {
    cvReleaseMemStorage(&storage);
}

ControlPointController* ControlPointController::getSharedControlPointController() {
    if (NULL == sharedControlPointController) {
        sharedControlPointController = new ControlPointController();
    }
    
    return sharedControlPointController;
}
    

int ControlPointController::addControlPoint(int pX, int pY) { // This function is also called to move a control point. Returns the control point added/moved.
    // cout << "Add control point" << endl;
    int index = CPC_NO_CONTROL_POINT;
    
    if (!controlPointMoving) {
        if (nextControlPointIndex < CPC_NUM_CONTROL_POINTS) {
            controlPoints[nextControlPointIndex].x = pX;
            controlPoints[nextControlPointIndex].y = pY;
            index = nextControlPointIndex;
            nextControlPointIndex++;
        }
    } else {
        controlPoints[movingControlPointIndex].x = pX;
        controlPoints[movingControlPointIndex].y = pY;
        
        invalidatePerspectiveTransform();
        
        index = movingControlPointIndex;
    }
    
    writeControlPoints();
    
    return index;
}
    
void ControlPointController::clearControlPoints() {
    endMovingControlPoint();
    invalidatePerspectiveTransform();
    nextControlPointIndex = 0;
    writeControlPoints();
}

void ControlPointController::writeControlPoints() {
    CvFileStorage *file = cvOpenFileStorage(CONTROL_POINT_FILE_NAME, storage, CV_STORAGE_WRITE);
    if (NULL != file) {
        for (int i = 0; i < nextControlPointIndex; i++) {
            char name[3];
            
            sprintf(name, "x%d", i);
            cvWriteInt(file, name, controlPoints[i].x);
            
            sprintf(name, "y%d", i);
            cvWriteInt(file, name, controlPoints[i].y);
        }
        
    } else {
        printf("Could not write control point file: %s. Ignoring.\n", CONTROL_POINT_FILE_NAME);
    }
    
    cvReleaseFileStorage(&file); 
}
    
void ControlPointController::readControlPoints() {
    CvFileStorage* file = cvOpenFileStorage( CONTROL_POINT_FILE_NAME, storage, CV_STORAGE_READ );
    if (NULL != file) {
        CvFileNode *lRoot = cvGetRootFileNode(file);
        
        for (int i = 0; i < CPC_NUM_CONTROL_POINTS; i++ ) {
            char name[3];

            sprintf(name, "x%d", i);
            int x = cvReadIntByName(file, lRoot, name, -1 /* default value */ );
            
            sprintf(name, "y%d", i);
            int y = cvReadIntByName(file, lRoot, name, -1 /* default value */ );
                
            if (x >=0 && y >= 0) {
                controlPoints[i].x = x;
                controlPoints[i].y = y;
                nextControlPointIndex = i+1;
                
                printf("Read saved control point %d: (%d, %d)\n", i, x, y );
            } else {
                break; // no more, or malformatted, control points
            }
        }    
    }
        
    cvReleaseFileStorage(&file);
}

void ControlPointController::startMovingControlPointWithIndex(int pControlPointIndex) {
    if (pControlPointIndex >= nextControlPointIndex) {
        printf("Control point index out of bounds. Ignoring.\n");
        return;
    }
    
    // cout << "Moving: " << pControlPointIndex << endl;
    
    controlPointMoving = true;
    movingControlPointIndex = pControlPointIndex;
}

void ControlPointController::endMovingControlPoint() {
    controlPointMoving = false;
    movingControlPointIndex = -1;
}

bool ControlPointController::isMovingControlPoint() {
    return controlPointMoving;
}

int ControlPointController::controlPointIndexForCoordinates(int pX, int pY) {        
    for (int i = 0; i < this->nextControlPointIndex; i++) {
        CvRect controlPointBounds = cvRect(this->controlPoints[i].x - this->controlPointRadius, this->controlPoints[i].y - this->controlPointRadius, this->controlPointRadius*2, this->controlPointRadius*2);
        if (pointInRect(cvPoint(pX, pY), controlPointBounds)) {
            return i;
        }
    }
    
    return CPC_NO_CONTROL_POINT;
}

void ControlPointController::drawControlPoints(IplImage *pImage) {
    for (int i = 0; i < nextControlPointIndex - 1; i++) {
        cvLine(pImage, controlPoints[i], controlPoints[i+1], LINE_COLOR);
    }
    
    if (nextControlPointIndex == CPC_NUM_CONTROL_POINTS) {
        cvLine(pImage, controlPoints[nextControlPointIndex-1], controlPoints[0], LINE_COLOR);
    }
    
    for (int i = 0; i < nextControlPointIndex; i++) {
        if (controlPointMoving && movingControlPointIndex == i) {
            cvCircle(pImage, controlPoints[i], 4, CV_RGB(255,0,0), -1);
        } else {
            cvCircle(pImage, controlPoints[i], 4, CV_RGB(0,0,255), -1);
        }
    }
}

CvMat* ControlPointController::getPerspectiveTransformForSize(CvSize pSize) {
    if (4 == nextControlPointIndex) {
        if (NULL == perspectiveTransform) {
            sortControlPoints();
            
            CvPoint2D32f sourcePoints[CPC_NUM_CONTROL_POINTS];
            CvPoint2D32f destPoints[CPC_NUM_CONTROL_POINTS] = { {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0} };
            
            destPoints[1].x = (float) pSize.width;
            destPoints[2].x = (float) pSize.width; destPoints[2].y = (float) pSize.height;
            destPoints[3].y = (float )pSize.height;

            for (int i= 0; i < CPC_NUM_CONTROL_POINTS; i++) {
                sourcePoints[i] = cvPointTo32f(controlPoints[i]);
            }

            perspectiveTransform = cvCreateMat(3, 3, CV_32FC1);
            cvGetPerspectiveTransform(sourcePoints, destPoints, perspectiveTransform);
        }
    }

    return perspectiveTransform;
}

void ControlPointController::invalidatePerspectiveTransform() {
    cvReleaseMat(&perspectiveTransform);
    perspectiveTransform = NULL;
}

void ControlPointController::sortControlPoints() {
    // Sort the control points to that the order is UL, UR, LR, LL.
    CvPoint swapPoint;
    
    for (int i = 1; i < nextControlPointIndex; i++) {
        for (int j = i; j > 0; j--) {
            if (controlPoints[j-1].y > controlPoints[j].y) {
                swapPoint = controlPoints[j-1];
                controlPoints[j-1] = controlPoints[j];
                controlPoints[j] = swapPoint;
            } else {
                break;
            }
        }
    }
    
    if (controlPoints[0].x > controlPoints[1].x) {
        swapPoint = controlPoints[0];
        controlPoints[0] = controlPoints[1];
        controlPoints[1] = swapPoint;
    }
    
    if (controlPoints[3].x > controlPoints[2].x) {
        swapPoint = controlPoints[2];
        controlPoints[2] = controlPoints[3];
        controlPoints[3] = swapPoint;
    }
}

double ControlPointController::getParallelogramRatio() {
	sortControlPoints();
	double width = ((double)(controlPoints[0].x + controlPoints[3].x))/2 - ((double)(controlPoints[1].x + controlPoints[2].x))/2;
	double height = ((double)(controlPoints[0].y + controlPoints[1].y))/2 - ((double)(controlPoints[2].y + controlPoints[3].y))/2;
	return width/height;
}