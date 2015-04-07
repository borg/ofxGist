#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetCircleResolution(80);
    ofSetFrameRate(60);
    ofBackground(255);
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofSetVerticalSync(true);
    
    
    bufferSize = 512;
    sampleRate = 44100;
    
    
    
    
    useMic = 1;
    isPaused = 0;
    
    player.setLoop(true);
    
    mfccMax = 0;
    showMFCC = 0;
    
    vector<string> features = ofxGist::getFeatureNames();
    
    int num = features.size();
    
    for(int v = 0;v<num;v++){
        GIST_FEATURE f = ofxGist::getFeatureFromName(features[v]);
        gist.setDetect(f);
       ofxHistoryPlot * graph = addGraph(features[v],1.0,ofColor(ofRandom(100)+150,ofRandom(100)+150,ofRandom(100)+150));
        plots.push_back(graph);
    }
    
    
    num = 13;//happens to be 13 coefficients by default
    for(int v = 0;v<num;v++){
        ofxHistoryPlot * graph = addGraph("mfcc_"+ofToString(v),1.0,ofColor(ofRandom(100)+150,ofRandom(100)+150,ofRandom(100)+150));
        mfccPlots.push_back(graph);
    }
    
    
    
    
    
    
    
    /*
    //add special crest graph
    ofxHistoryPlot*crest = addGraph("CREST_AVG",1.0,ofColor(ofRandom(100)+150,ofRandom(100)+150,ofRandom(100)+150));
    crest->setShowSmoothedCurve(1);
     */
    
    //gist.setUseForOnsetDetection(GIST_PEAK_ENERGY);
    
    //gist.setUseForOnsetDetection(GIST_SPECTRAL_DIFFERENCE);
    //gist.setThreshold(GIST_SPECTRAL_DIFFERENCE, .2);
    
    gist.setUseForOnsetDetection(GIST_PEAK_ENERGY);
    gist.setThreshold(GIST_PEAK_ENERGY, .05);//
    
    ofAddListener(GistEvent::ON,this,&ofApp::onNoteOn);
    ofAddListener(GistEvent::OFF,this,&ofApp::onNoteOff);
    
    
    
    noteOnRadius = 0;
    
    soundStream.setup(this,0, 1, sampleRate, bufferSize, 1);
    
    
    loadSong("assets/sounds/Coltrane_acc_VUIMM");
}


void ofApp::onNoteOn(GistEvent &e){
    
    noteOnRadius = 100;
};


void ofApp::onNoteOff(GistEvent &e){

    //noteOnRadius = 0;
};


//--------------------------------------------------------------
void ofApp::update(){
    
    
    if(isPaused){
        return;
    }
    if(!useMic){
        if(player.isLoaded()){
            vector<float> output = player.getCurrentBuffer(bufferSize);
            processAudio(&output[0], bufferSize, 2);
            fftSmoothed = player.getFFT();
        }
    }
    
    
    
    int num = ofxGist::getFeatureNames().size();
    
    for(int v = 0;v<num;v++){
        plots[v]->setRange(gist.getMin(v),gist.getMax(v));
        plots[v]->update(gist.getValue(v));
    }

    vector<float>mfcc = gist.getMelFrequencyCepstralCoefficients();

    //vector<float>mfcc = gist.getMelFrequencySpectrum();
    
    if(mfccSmoothed.size()<mfcc.size()){
        mfccSmoothed.assign(mfcc.size(),0.0);
    }
    float damping = .7;
    int f = 0;
    for (int i = 0; i < mfcc.size(); i++){
        // take the max, either the smoothed or the incoming:
        if (mfccSmoothed[f] < mfcc[i] || damping >.999f){
            mfccSmoothed[f] = mfcc[i];
        }
        // let the smoothed value sink to zero:
        mfccSmoothed[i] *= damping;
        f++;
        
        if(mfccMax<mfcc[i]){
            mfccMax = mfcc[i];
        }
        
        
        mfccPlots[i]->setRange(gist.getMFCCMin(i),gist.getMFCCMax(i));
        
        mfccPlots[i]->update(mfcc[i]);
    }
    
    
    
    
    /*
    //crest avg
    plots.back()->setRange(gist.getMin(GIST_SPECTRAL_CREST),gist.getMax(GIST_SPECTRAL_CREST));
    plots.back()->update(gist.getAvg(GIST_SPECTRAL_CREST));
*/
    
    
    if(noteOnRadius>0){
        noteOnRadius--;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    
    
    

    // draw the left:
    //ofSetHexColor(0x333333);
    int waveHeight = ofGetHeight();
    
    //ofRect(0,0,256,waveHeight);
    float barW = ofGetWidth()/(float)fftSmoothed.size();
    float currX = 0;
    
    ofSetColor(255,255,255,100);
    for (int i = 0; i < fftSmoothed.size(); i++){
        //ofLine(i,100,i,100+left[i]*waveHeight);
        
        ofRect(currX,waveHeight,barW,-fftSmoothed[i]*waveHeight);
        currX+=barW;
    }
    
    currX = 0;
    waveHeight = ofGetHeight();
    barW = ofGetWidth()/(float)mfccSmoothed.size();
    ofSetColor(255,100,100,200);
    for (int i = 0; i < mfccSmoothed.size() && showMFCC; i++){
        //ofLine(i,100,i,100+left[i]*waveHeight);
        
        ofRect(currX,ofGetHeight(),barW,- ofMap(mfccSmoothed[i], 0.0,mfccMax, 0.0,1.0,true )*waveHeight);
        currX+=barW;
    }
    

    int num;
    int margin = 5;
    if(showMFCC){
        num = mfccPlots.size();
        int plotHeight = (ofGetHeight()-margin*num)/(float)num;
        
        for(int v = 0;v<num;v++){
            mfccPlots[v]->draw(margin,margin+plotHeight*v, ofGetWidth()-20, 100);
        }
    }else{
        
        
        num = plots.size();
        
        int plotHeight = (ofGetHeight()-margin*num)/(float)num;
        
        for(int v = 0;v<num;v++){
            plots[v]->draw(margin,margin+plotHeight*v, ofGetWidth()-20, 100);
        }
        
    }
    

    
    ofSetColor(255,0,0,200);
    ofCircle(ofGetWidth()/2,ofGetHeight()/2,noteOnRadius);
    
    
    if(!showMFCC){
        
        
        ofSetColor(0,0,0,250);
        ofRect(5,5,600,180);
        stringstream str;
        if(plots.size() ){
            for(int v = 0;v<num;v++){
                str<<plots[v]->getVariableName()<<" | min: "<<gist.getMin(v)<<" | avg: "<<gist.getAvg(v)<<" | max: "<<gist.getMax(v)<<"\n";
                
            }
            
             str<<"Note freq: "<< gist.getNoteFrequency()<<" "<<gist.getNoteName() <<"\n";
        }else{
            str<<"Drag and drop wave files from assets/sounds.";
        }
        
        ofSetColor(255);
        
        ofDrawBitmapString(str.str(),10,10);
    }

    
    

}



ofxHistoryPlot* ofApp::addGraph(string varName,float max,ofColor color){
    
    ofxHistoryPlot* graph = new ofxHistoryPlot(NULL, varName, max, false);	//true for autoupdate
    //graph2->setLowerRange(0); //set only the lowest part of the range upper is adaptative to curve
    graph->setAutoRangeShrinksBack(true); //graph2 scale can shrink back after growing if graph2 curves requires it
    graph->setRange(0,max);
    graph->setColor(color);
    graph->setShowNumericalInfo(true);
    graph->setRespectBorders(true);
    graph->setLineWidth(2);
    
    
    graph->setDrawBackground(false);
    
    graph->setDrawGrid(true);
    graph->setGridColor(ofColor(30)); //grid lines color
    graph->setGridUnit(14);
    graph->setShowSmoothedCurve(0); //graph2 a smoothed version of the values, but alos the original in lesser alpha
    graph->setSmoothFilter(0.1); //smooth filter strength
    
    graph->setMaxHistory(2000);
    
    
    
    return graph;
    
};


void ofApp::clear(){

    
}


void ofApp::loadSong(string str, bool isLead){
    
    cout<<"loadSong "<<str<<endl;
    
    player.stop();
    player.loadSound(str+".wav");
    player.setLoop(true);
    player.play();
    useMic = 0;
    mfccMax = 0;
    gist.clearHistory();
}

void ofApp::processAudio(float * input, int bufferSize, int nChannels){
    //convert float array to vector
    vector<float>buffer;
    buffer.assign(&input[0],&input[bufferSize]);
    
    gist.processAudio(buffer, bufferSize, nChannels,sampleRate);
}


void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    if(!useMic){
        return;
    }
    
    processAudio(input, bufferSize, nChannels);
      
}




//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key =='f'){
        ofToggleFullscreen();
    }
    
    if(key =='m'){
        useMic = !useMic;
        
        if(!useMic){
            player.play();
        }else{
            player.stop();
        }
        
        
        gist.clearHistory();
    }
    
    if(key ==' '){
        isPaused = !isPaused;
        player.setPaused(isPaused);
    }
    
    if(key =='r'){
        gist.clearHistory();
    }
    
    
    if(key =='c'){
        showMFCC = !showMFCC;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(!useMic){
        player.setPosition(x/(float)ofGetWidth());
    }
    
    float t =  x/(float)ofGetWidth();
    gist.setThreshold(GIST_SPECTRAL_DIFFERENCE,t);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
    clear();
    
    
    vector<string> paths = ofSplitString(dragInfo.files[0], "/");
    vector<string> parts = ofSplitString(paths.back(), ".");
    
    //drag and drop only lead
    loadSong("assets/sounds/"+parts[0],true);
    //    cout<<dragInfo.files[0]<<endl;
    
    
    /*
    if(ofStringTimesInString(parts[0], "lead")){
        ofStringReplace(parts[0], "lead", "acc");
        loadSong("assets/sounds/"+parts[0]);
    }*/

}
