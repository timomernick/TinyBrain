//
//  GameViewController.h
//  TinyBrainApp
//
//  Created by Tim Omernick on 10/7/16.
//  Copyright (c) 2016 Tim Omernick. All rights reserved.
//

#import <SceneKit/SceneKit.h>

#import "GameView.h"
#import "SOM.h"

typedef SOM<const char *> Brain;

@interface GameViewController : NSViewController

@property (assign) IBOutlet GameView *gameView;

@property (assign) Brain *brain;
    
@property (retain) SCNNode *neuronContainerNode;
    
@end
