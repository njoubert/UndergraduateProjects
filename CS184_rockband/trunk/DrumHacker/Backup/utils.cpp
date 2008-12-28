#include "utils.h"

bool pointInRect(CvPoint pPoint, CvRect pRect) {  
    if (pPoint.x >= pRect.x && pPoint.x <= pRect.x + pRect.width && pPoint.y >= pRect.y && pPoint.y <= pRect.y + pRect.height) {
        return true;
    } else {
        return false;
    }
}