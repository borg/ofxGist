#pragma once

#include "ofMain.h"

#include "ofxOpenALSoundPlayer.h"
#include "ofxHistoryPlot.h"
#include "ofxGist.h"

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
		
        ofSoundStream soundStream;
        ofxOpenALSoundPlayer player;
        void processAudio(float * input, int bufferSize, int nChannels);
        
        void audioIn(float * input, int bufferSize, int nChannels);
        
        vector<float>fftSmoothed;
    
        vector<float>mfccSmoothed;
    float mfccMax;
    
        
        int bufferSize;
        int sampleRate;
        bool useMic;
        
        bool isPaused;
        
        void clear();
        void loadSong(string str, bool isLead =0);

        
        vector<ofxHistoryPlot *>plots;
        map<string,ofxHistoryPlot *>plotMap;
        
        ofxHistoryPlot* addGraph(string varName,float max,ofColor color);

        
        ofxGist gist;
        void onNoteOn(GistEvent &e);
        void onNoteOff(GistEvent &e);
    
        int noteOnRadius;
    
    
        bool showMFCC;
        vector<ofxHistoryPlot *>mfccPlots;
    
};
