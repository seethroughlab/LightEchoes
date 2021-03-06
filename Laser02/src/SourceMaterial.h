//
//  SourceMaterial.h
//  Laser02
//
//  Created by Jeffrey Crouse on 6/17/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"
#include "ofxJSON.h"


class SourceMaterial : public ofFbo {
public:

    void setup();
    ofFloatColor getColor(int x, int y);
    void increment(bool forward=true);
    int getIndex();
    string getName() {
        if(index > dir.size()-1 || dir.size()==0 || index < 0) return "NONE";
        return dir.getName(index);
    }
    void onKeyReleased(int key);
    
protected:
    
    void updatePixels();
    void loadImage();
    void drawIntoFBO();
    ofPixels pixels;
    string name;
    ofImage image;
    string dirPath;
    int index;
    Poco::Path sourceState;
    ofxJSONElement state;
    ofxJSONElement warper;
    ofDirectory dir;
    map<string,ofTrueTypeFont> fonts;
    Poco::Path sourcePath;
    
    void saveWarp();
    void loadWarp();
    
    int v;
    bool bWarpMode;
    ofMesh mesh;
    
    ofPlanePrimitive plane;
};
