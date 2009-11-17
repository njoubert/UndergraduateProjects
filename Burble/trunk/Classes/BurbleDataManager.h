//
//  BurbleDataManager.h
//  
// This is a singleton class that manages all the data for our application. It's the heart and soul of the app.
// 
//

#import <Foundation/Foundation.h>


@interface BurbleDataManager : NSObject {

}
+ (BurbleDataManager *) sharedDataManager;
- (void)loadData;
- (void)saveData;

@end
