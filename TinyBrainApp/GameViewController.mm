//
//  GameViewController.m
//  TinyBrainApp
//
//  Created by Tim Omernick on 10/7/16.
//  Copyright (c) 2016 Tim Omernick. All rights reserved.
//

#import "GameViewController.h"

#import "SOM.h"

static DataSet<const char*> dataSet = {
    {
        { 1.0f, 0.0f, 0.0f },
        { "red" }
    },
    
    {
        { 0.0f, 1.0f, 0.0f },
        { "green" }
    },
    
    {
        { 0.0f, 0.0f, 1.0f },
        { "blue" }
    },
    
    {
        { 1.0f, 1.0f, 0.0f },
        { "yellow" }
    },
    
    {
        { 0.0f, 1.0f, 1.0f },
        { "cyan" }
    },
    
    {
        { 1.0f, 0.0f, 1.0f },
        { "magenta" }
    },
};

@implementation GameViewController

- (void)_initBrain
{
    srandom(time(NULL));
    
    // Get size of data (input) vector in data set (assumed to all be the same size).
    size_t inputSize = dataSet[0].first.size();
    
    // Get size of label (ouptut) vector in data set (assumed to all be the same size).
    size_t outputSize = dataSet[0].second.size();
    
    // Create a SOM for this data.
    _brain = new SOM<const char *>(inputSize, outputSize, 400);
}

- (void)_train
{
    while (1) {
        // Train the SOM.
        _brain->Train(dataSet);
    }
}
    
- (void)_visualizeBrain
{
    if (!_neuronContainerNode) {
        _neuronContainerNode = [SCNNode node];
        [self.gameView.scene.rootNode addChildNode:_neuronContainerNode];
    }
    
    size_t numNeurons = _brain->GetNumNeurons();
    for (uint32_t i = 0; i < numNeurons; i++) {
        Data weights = _brain->GetNeuronWeights(i);
        
        float x = 0.0f;
        float y = 0.0f;
        _brain->GetNeuronPosition(i, x, y);
        
        NSArray *children = _neuronContainerNode.childNodes;
        SCNNode *neuronNode;
        if (children.count <= i) {
            neuronNode = [SCNNode nodeWithGeometry:[SCNSphere sphereWithRadius:0.04f]];
            [_neuronContainerNode addChildNode:neuronNode];
        } else {
            neuronNode = children[i];
        }
        
        neuronNode.position = SCNVector3Make(x * 2.0f, y * 2.0f, 0.0f);
        neuronNode.geometry.firstMaterial.diffuse.contents = [NSColor colorWithRed:weights[0] green:weights[1] blue:weights[2] alpha:1.0f];
    }
}

-(void)awakeFromNib
{
    [super awakeFromNib];

    // Create and start training the brain
    [self _initBrain];
    [NSThread detachNewThreadSelector:@selector(_train) toTarget:self withObject:nil];
    
    // create a new scene
    SCNScene *scene = [SCNScene scene];
    
    // create and add a camera to the scene
    SCNNode *cameraNode = [SCNNode node];
    cameraNode.camera = [SCNCamera camera];

    [scene.rootNode addChildNode:cameraNode];
    
    // place the camera
    cameraNode.position = SCNVector3Make(1, 1, 3);
    
    // create and add a light to the scene
    SCNNode *lightNode = [SCNNode node];
    lightNode.light = [SCNLight light];
    lightNode.light.type = SCNLightTypeOmni;
    lightNode.position = SCNVector3Make(0, 0, 10);
    [scene.rootNode addChildNode:lightNode];
    
    // create and add an ambient light to the scene
    SCNNode *ambientLightNode = [SCNNode node];
    ambientLightNode.light = [SCNLight light];
    ambientLightNode.light.type = SCNLightTypeAmbient;
    ambientLightNode.light.color = [NSColor darkGrayColor];
    [scene.rootNode addChildNode:ambientLightNode];
    
    // set the scene to the view
    self.gameView.scene = scene;
    
    // allows the user to manipulate the camera
    self.gameView.allowsCameraControl = YES;
    
    // show statistics such as fps and timing information
    self.gameView.showsStatistics = YES;
    
    // configure the view
    self.gameView.backgroundColor = [NSColor blackColor];
    
    [self _visualizeBrain];
    [NSTimer scheduledTimerWithTimeInterval:(1.0f / 30.0f) target:self selector:@selector(_visualizeBrain) userInfo:nil repeats:YES];
}

@end
