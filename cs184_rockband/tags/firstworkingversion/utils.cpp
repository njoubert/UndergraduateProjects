#include "utils.h"

bool pointInRect(CvPoint pPoint, CvRect pRect) { // includes the rect boundary
    // cout << "Point: " << pPoint.x << " " << pPoint.y << endl;
    // cout << "Rect: " << pRect.x << " " << pRect.y << " " << pRect.width << " " << pRect.height << endl;
    
    if (pPoint.x >= pRect.x && pPoint.x <= pRect.x + pRect.width && pPoint.y >= pRect.y && pPoint.y <= pRect.y + pRect.height) {
        return true;
    } else {
        return false;
    }
}

CvRect insetRect(CvRect pRect, int pDx, int pDy) {
    return cvRect(pRect.x + pDx, pRect.y + pDy, pRect.width - 2*pDx, pRect.height - 2*pDy);
}
