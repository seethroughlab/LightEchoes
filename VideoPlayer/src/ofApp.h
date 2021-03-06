#pragma once

#include "ofMain.h"
#include <Poco/DateTimeFormatter.h>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        bool bDebug;
    
        //ofDirectory dir;
        //ofImage frame;
        //Poco::Timestamp newestFrame;
        //float nextFramecheck;
        //float frameStart,frameEnd;
        //float frameAlpha;
        //string dropboxPath;
        //string photosDir;
    
        ofVideoPlayer* video;
        ofRectangle bounds;
        float playAt;
        ofFile videoFile;
        ofFile lockFile;
        Poco::Timestamp videoModified;
    
};
