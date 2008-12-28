#ifndef CONTROLPOINTCONTROLLER_H_
#define CONTROLPOINTCONTROLLER_H_

#include "global.h"
#include "utils.h"

#define CONTROL_POINT_FILE_NAME "tmpcontrolPoints.yaml"

#define CPC_NUM_CONTROL_POINTS 4
#define CPC_NO_CONTROL_POINT -1

class ControlPointController {
	
private:
    CvPoint controlPoints[CPC_NUM_CONTROL_POINTS];

    int nextControlPointIndex;
    
    CvMat *perspectiveTransform;
    
    int controlPointRadius;
    
    bool controlPointMoving;

    int movingControlPointIndex;
    
    CvMemStorage *storage;
    
    static ControlPointController *sharedControlPointController;
    
    ControlPointController();

public:
    
    static ControlPointController *getSharedControlPointController();
    
    ~ControlPointController();
    
    int addControlPoint(int pX, int pY);
    
    void clearControlPoints();
    
    void writeControlPoints();
    
    void readControlPoints();
    
    void startMovingControlPointWithIndex(int pControlPointIndex);

	void endMovingControlPoint();

	bool isMovingControlPoint();

	int controlPointIndexForCoordinates(int pX, int pY);
    
    void drawControlPoints(IplImage *pImage);
    
    CvMat *getPerspectiveTransformForSize(CvSize pSize);
    
    void invalidatePerspectiveTransform();
    
    void sortControlPoints();
    
    double getParallelogramRatio();
};

#endif /*CONTROLPOINTCONTROLLER_H_*/