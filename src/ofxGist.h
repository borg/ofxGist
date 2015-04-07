/*
 *  ofxGist.h
 *  Gist
 *
 *  Created by Andreas Borg on 30/03/2015
 *  Copyright 2015 Local Projects. All rights reserved.
 *
 *  Wrapper of Adam Stark's Gist
 *  Gist is a C++ based audio analysis library, written for use in real-time applications.
 *
 *  http://www.adamstark.co.uk/sound-analyser/
 *
 *  https://github.com/adamstark/Gist
 */

#ifndef _ofxGist
#define _ofxGist

#include "ofMain.h"

/* Use KISS Fast Fourier Transform */
//needs to be included before Gist..also make sure there is only one kiss instance in your project
#define USE_KISS_FFT

#include "Gist.h"





/*
 
 Spectral Difference, derivate, shows the amount of change…effectively onset
 
 Spectral Crest - How tonal the signal is, useful for distinguishing instuments
 
 Spectral Centroid - Correlates to the brightness of the sound
 
 Mel-frequency spectrum - Human perception FFT
 
 Zero Crossing Rate - Sound brightness
 
 Root mean square - Signal energy
 
 */


//TODO : MFCC and spectral non float values








typedef enum GIST_FEATURE{
    GIST_PITCH,
    GIST_NOTE,
    GIST_ROOT_MEAN_SQUARE,
    GIST_PEAK_ENERGY,
    GIST_SPECTRAL_CREST,
    GIST_ZERO_CROSSING_RATE,
    GIST_SPECTRAL_CENTROID,
    GIST_SPECTRAL_FLATNESS,
    GIST_SPECTRAL_DIFFERENCE,
    GIST_SPECTRAL_DIFFERENCE_COMPLEX,
    GIST_SPECTRAL_DIFFERENCE_HALFWAY,
    GIST_HIGH_FREQUENCY_CONTENT
    
}GIST_FEATURE;

class GistEvent : public ofEventArgs {
    
public:
    
    GistEvent(){};
    float energy;
    float frequency;
    float note;
    float onsetAmount;
    GIST_FEATURE feature;//onset_detection feature that triggered event
    
    
    //these are global to keep all in sync
    static ofEvent <GistEvent> ON;
    static ofEvent <GistEvent> OFF;
    
    
};




#define GIST_DEFAULT_SAMPLING_FREQUENCY 44100
#define GIST_DEFAULT_BUFFERSIZE 512

class ofxGist {
	
  public:
	
    
     //If you add _GIST_FEATURE above make sure to add here
    static  vector<string>getFeatureNames(){
        
        if(ofxGist::_featureNames.size()){
            return ofxGist::_featureNames;
        }
        stringstream str;
        
        str<<"GIST_PITCH,";
        str<<"GIST_NOTE,";
        str<<"GIST_ROOT_MEAN_SQUARE,";
        str<<"GIST_PEAK_ENERGY,";
        str<<"GIST_SPECTRAL_CREST,";
        str<<"GIST_ZERO_CROSSING_RATE,";
        str<<"GIST_SPECTRAL_CENTROID,";
        str<<"GIST_SPECTRAL_FLATNESS,";
        str<<"GIST_SPECTRAL_DIFFERENCE,";
        str<<"GIST_SPECTRAL_DIFFERENCE_COMPLEX,";
        str<<"GIST_SPECTRAL_DIFFERENCE_HALFWAY,";
        str<<"GIST_HIGH_FREQUENCY_CONTENT";
        
        
        ofxGist::_featureNames = ofSplitString(str.str(),",");
        return ofxGist::_featureNames;
    };
    
    
    static  GIST_FEATURE getFeatureFromName(string n){
        
        init();
        
        
        for ( int it = 0; it != ofxGist::getFeatureNames().size(); it++ ){
            if(ofxGist::getFeatureNames()[it] == n){
                GIST_FEATURE feature = static_cast<GIST_FEATURE>(it);
                return feature;
            }
        
        
        }
        
        ofLogWarning()<<"ofGist feature "<<n<<" doesn't exist"<<endl;
        return static_cast<GIST_FEATURE>(0);

    }
    
    
    ofxGist():gist(GIST_DEFAULT_BUFFERSIZE,GIST_DEFAULT_SAMPLING_FREQUENCY){
        setup();
    };
    
    void setup(){
        ofxGist::init();
        
        _defaultSampleRate = GIST_DEFAULT_SAMPLING_FREQUENCY;
        _defaultBufferSize = GIST_DEFAULT_BUFFERSIZE;

        _isNoteOn = 0;
        
        gist.setAudioFrameSize(_defaultBufferSize);
        
        //not detecting anything by default
        for(GIST_FEATURE feature:_features){
            _doDetect[feature] = 0;
            _useForOnsetDetection[feature] = 0;
            _thresholds[feature]=0;
        }
        
        
        
        //some near guess defaults
        _thresholds[GIST_SPECTRAL_DIFFERENCE_COMPLEX] = .5;
        
        
        clearHistory();
        
        _calculateMFCC = false;
    }
	
	void processAudio(const vector<float>& input, int bufferSize = 512, int nChannels = 2,int sampleRate = 44100);
    
    
    void setDetect(GIST_FEATURE,bool b = true);
    bool getDetect(GIST_FEATURE f);
    
    
    void setUseForOnsetDetection(GIST_FEATURE,bool b = true);
    bool getUseForOnsetDetection(GIST_FEATURE f);
    
    
    float getValue(GIST_FEATURE f);
    float getValue(int f);//will be cast to enum
    

    float getMin(GIST_FEATURE f);
    float getMin(int f);//will be cast to enum
    
    float getMax(GIST_FEATURE f);
    float getMax(int f);//will be cast to enum

    
    float getAvg(GIST_FEATURE f);
    float getAvg(int f);//will be cast to enum
    
    
    //used for onset detection events
    
    void setThreshold(GIST_FEATURE f, float t);
    float getThreshold(GIST_FEATURE f);
    float getThreshold(int f);//will be cast to enum
    
    
    void clearHistory();//min/max etc
    
    
    
    
    float   getNoteFrequency();//midi value
    float   frequencyToMidi(float freq);
    string  getNoteName();
    
    /*
     
     In sound processing, the mel-frequency cepstrum (MFC) is a representation of the short-term power spectrum of a sound, based on a linear cosine transform of a log power spectrum on a nonlinear mel scale of frequency.
     
     Mel-frequency cepstral coefficients (MFCCs) are coefficients that collectively make up an MFC. They are derived from a type of cepstral representation of the audio clip (a nonlinear "spectrum-of-a-spectrum"). The difference between the cepstrum and the mel-frequency cepstrum is that in the MFC, the frequency bands are equally spaced on the mel scale, which approximates the human auditory system's response more closely than the linearly-spaced frequency bands used in the normal cepstrum. This frequency warping can allow for better representation of sound, for example, in audio compression.
     
     MFCCs are commonly derived as follows:[1][2]
     
     Take the Fourier transform of (a windowed excerpt of) a signal.
     Map the powers of the spectrum obtained above onto the mel scale, using triangular overlapping windows.
     Take the logs of the powers at each of the mel frequencies.
     Take the discrete cosine transform of the list of mel log powers, as if it were a signal.
     The MFCCs are the amplitudes of the resulting spectrum.

     */
    
    vector<float> getMelFrequencySpectrum();
    vector<float> getMelFrequencyCepstralCoefficients();
    
    
    float getMFCCMin(int coeffNum);//of 13
    float getMFCCMax(int coeffNum);
    float getMFCCAvg(int coeffNum);

    
    
protected:
    
    static void init(){
        
        if(ofxGist::_features.size()==0){
            for ( int it = 0; it != ofxGist::getFeatureNames().size(); it++ ){
                GIST_FEATURE feature = static_cast<GIST_FEATURE>(it);
                _features.push_back(feature);
            }
        }
        
    }
    

    
    
    void processOnsetDetection(GIST_FEATURE);
    
    
    int _defaultSampleRate;
    int _defaultBufferSize;
    
    Gist<float> gist;
    
    
    bool _isNoteOn;
    
    bool _calculateMFCC;
    
    static vector<string>_featureNames;
    
    static  vector<GIST_FEATURE>_features;
    map<GIST_FEATURE,bool>_doDetect;
    map<GIST_FEATURE,float>_values;
    map<GIST_FEATURE,bool>_useForOnsetDetection;
    
    
    map<GIST_FEATURE,float>_minValues;
    map<GIST_FEATURE,float>_maxValues;
    map<GIST_FEATURE,float>_avgValues;
	map<GIST_FEATURE,float>_avgValueNum;
    
    
    vector<float>  _mfccMinValues;
    vector<float>  _mfccMaxValues;
    vector<float>  _mfccAvgValues;
    vector<float>  _mfccAvgValueNum;
    
    //onset delta thresholds, ie. relative change
    map<GIST_FEATURE,float>_thresholds;
    map<GIST_FEATURE, vector<float> > _history;//used to track limited relative changes
    
};

#endif
