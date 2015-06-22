//
//  SourceMaterial.cpp
//  Laser02
//
//  Created by Jeffrey Crouse on 6/17/15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "SourceMaterial.h"
#include "ofxModifierKeys.h"

#define SOURCE_DIR "SourceAlt"
#define SOURCE_MATERIAL_STATE_JSON "source-state.json"
#define SOURCE_MATERIAL_WARP_JSON "warper.json"
#define NUM_ROWS 30
#define NUM_COLS 4


// -------------------------------------------------
void SourceMaterial::setup() {
    allocate(800, 1600);
    
    fonts["small"].loadFont("fonts/HelveticaNeueLTCom-Th.ttf", 100);
    fonts["med"].loadFont("fonts/HelveticaNeueLTCom-Th.ttf", 200);
    fonts["large"].loadFont("fonts/HelveticaNeueLTCom-Th.ttf", 300);
    
    
    begin();
    ofClear(ofColor::black);
    ofDrawBitmapStringHighlight("NOT LOADED", 10, 20, ofColor::red, ofColor::white);
    end();
    
    v = 0;
    bWarpMode=false;
    
    
    state.open(SOURCE_MATERIAL_STATE_JSON);
    
    if (!state.isMember("index")) state["index"] = 0;
    index = state["index"].asInt();
 
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    float col_pos[NUM_COLS] = {0.0, 170/800.0, 628/800.0, 1.0};
    
    ofVec3f vert;
    ofVec3f normal(0, 0, 1); // always facing forward //
    ofVec2f texcoord;
    int rows = NUM_ROWS;
    int columns = NUM_COLS;
    for(int iy = 0; iy < rows; iy++) {
        for(int ix = 0; ix < columns; ix++) {
            
            // normalized tex coords //
            //texcoord.x = ((float)ix/((float)columns-1.f));
            texcoord.x = col_pos[ix];
            texcoord.y = ((float)iy/((float)rows-1.f));
            
            vert.x = texcoord.x * getWidth();
            vert.y = texcoord.y * getHeight();
            
            mesh.addVertex(vert);
            mesh.addTexCoord(texcoord);
            mesh.addNormal(normal);
        }
    }
    
    // Triangles //
    for(int y = 0; y < rows-1; y++) {
        for(int x = 0; x < columns-1; x++) {
            // first triangle //
            mesh.addIndex((y)*columns + x);
            mesh.addIndex((y)*columns + x+1);
            mesh.addIndex((y+1)*columns + x);
            
            // second triangle //
            mesh.addIndex((y)*columns + x+1);
            mesh.addIndex((y+1)*columns + x+1);
            mesh.addIndex((y+1)*columns + x);
        }
    }
    
    dir.listDir(SOURCE_DIR);
    loadWarp();
    loadImage();
}

// -------------------------------------------------
void SourceMaterial::onKeyReleased(int key) {
    
    if(key=='w') {
        bWarpMode = !bWarpMode;
        saveWarp();
    }
    
    if(bWarpMode) {
        vector<ofPoint>& verts = mesh.getVertices();
        if(key=='v') {
            ++v %= verts.size(); // oooh, tricky
            saveWarp();
        }
        if(key=='V') {
            --v %= verts.size(); // oooh, tricky
            saveWarp();
        }
        if(key==OF_KEY_LEFT) {
            verts[v].x -= ofGetModifierPressed(OF_KEY_SHIFT) ? 4 : 0.5;
            updatePixels();
            saveWarp();
        }
        if(key==OF_KEY_RIGHT) {
            verts[v].x += ofGetModifierPressed(OF_KEY_SHIFT) ? 4 : 0.5;
            updatePixels();
            saveWarp();
        }
        if(key==OF_KEY_UP) {
            verts[v].y -= ofGetModifierPressed(OF_KEY_SHIFT) ? 4 : 0.5;
            updatePixels();
            saveWarp();
        }
        if(key==OF_KEY_DOWN) {
            verts[v].y += ofGetModifierPressed(OF_KEY_SHIFT) ? 4 : 0.5;
            updatePixels();
            saveWarp();
        }
    }
    drawIntoFBO();
}


// -------------------------------------------------
void SourceMaterial::saveWarp() {
    vector<ofVec3f>& verts = mesh.getVertices();
    for(int i=0; i<verts.size(); i++) {
        warper["verts"][i]["x"] = verts[i].x;
        warper["verts"][i]["y"] = verts[i].y;
    }
    warper["v"] = v;
    warper["show"] = bWarpMode;
    warper.save(SOURCE_MATERIAL_WARP_JSON, true);
    
}
// -------------------------------------------------
void SourceMaterial::loadWarp() {
    if(!ofFile::doesFileExist(SOURCE_MATERIAL_WARP_JSON)) return;
    
    warper.open(SOURCE_MATERIAL_WARP_JSON);
    vector<ofVec3f>& verts = mesh.getVertices();
    for(int i=0; i<verts.size(); i++) {
        verts[i].x = warper["verts"][i]["x"].asFloat();
        verts[i].y = warper["verts"][i]["y"].asFloat();
    }
    v = warper["v"].asInt();
    bWarpMode = warper["show"].asBool();
}

// -------------------------------------------------
ofFloatColor SourceMaterial::getColor(int x, int y) {
    return (ofFloatColor)pixels.getColor(x, y);
}

// -------------------------------------------------
void SourceMaterial::increment() {
    index++;
    index %= dir.size();
    state["index"] = index;
    state.save(SOURCE_MATERIAL_STATE_JSON, true);
    loadImage();
}

// -------------------------------------------------
int SourceMaterial::getIndex() {
    return index;
}

// -------------------------------------------------
void SourceMaterial::updatePixels() {
    readToPixels(pixels);
}

// -------------------------------------------------
void SourceMaterial::loadImage() {
    
    if(dir.size()==0 || index > dir.size()-1 || index < 0)
        return false;
    
    ofLogNotice("SourceMaterial") << "Loading " << index;
    
    name = dir.getName(index);
    image.clear();
    image.loadImage(dir.getPath(index));
    image.mirror(true, true);
    
    drawIntoFBO();
    updatePixels();
}

// -------------------------------------------------
void SourceMaterial::drawIntoFBO() {
    begin();
    
    ofClear(ofColor::black);
    ofSetColor(ofColor::white);
    
    ofEnableNormalizedTexCoords();
    image.bind();
    mesh.draw();
    image.unbind();
    ofDisableNormalizedTexCoords();
    
    if(bWarpMode) {
        
        vector<ofPoint>& verts = mesh.getVertices();
        for(int i=0; i<verts.size(); i++) {
            if(i==v) {
                ofFill();
                ofSetColor(ofColor::green);
            } else {
                ofNoFill();
                ofSetColor(ofColor::gray);
            }
            ofCircle(verts[i], 10);
        }
    }
    end();
}
