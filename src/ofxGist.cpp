/*
 *  ofxGist.cpp
 *  Gist
 *
 *  Created by Andreas Borg on 30/03/2015
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxGist.h"



ofEvent <GistEvent> GistEvent::ON;
ofEvent <GistEvent> GistEvent::OFF;





vector<string>ofxGist::_featureNames;
vector<GIST_FEATURE>ofxGist::_features;
//------------------------------------------------------------------
void ofxGist::processAudio(const vector<float>& input, int bufferSize, int nChannels, int sampleRate){
    
    if(_defaultBufferSize != bufferSize){
        gist.setAudioFrameSize(bufferSize);
        _defaultBufferSize = bufferSize;
    }
    
    if(_defaultSampleRate != sampleRate){
        //gist.setAudioFrameSize(bufferSize);
        _defaultSampleRate = sampleRate;
    }
    
    
    gist.processAudioFrame(input);
    
    
    int v = _features.size();
    while(v--){
        
        //store max/min/avg
        if( _values[_features[v]] > _maxValues[_features[v]]){
            _maxValues[_features[v]] =  _values[_features[v]];
        }
        
        if( _values[_features[v]] < _minValues[_features[v]]){
            _minValues[_features[v]] =  _values[_features[v]];
        }
        
        if(_avgValueNum[_features[v]]){
            _avgValues[_features[v]] = _values[_features[v]];
        }else{
            _avgValues[_features[v]] = (_avgValues[_features[v]]*_avgValueNum[_features[v]] + _values[_features[v]])/(float)(_avgValueNum[_features[v]]+1);
        }
        _avgValueNum[_features[v]]++;
        
        
        
        
        
        
        
        switch (_features[v] ){
            case GIST_PITCH:
                if(_doDetect[_features[v]] ){
                   // yin.setSamplingFrequency(sampleRate);
                    //_values[GIST_PITCH] = MAX(0,yin.pitchYin(input));
                    
                    _values[GIST_PITCH] = MAX(0,gist.pitchYin());
                    
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                }
                break;
            case GIST_NOTE:
                if(_doDetect[_features[v]] ){
                 //this might take last frame unfortunately
                    _values[GIST_NOTE] = getNoteFrequency();
                    
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                }
                break;
                
               
                
            case GIST_PEAK_ENERGY:
                if(_doDetect[_features[v]] ){
                    //CoreTimeDomainFeatures<float> tdf;
                    //_values[GIST_PEAK_ENERGY] = tdf.peakEnergy(input);
                     _values[GIST_PEAK_ENERGY] = gist.peakEnergy();
                    
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                }
                break;
                
            case GIST_ROOT_MEAN_SQUARE:
                if(_doDetect[_features[v]] ){
                    //CoreTimeDomainFeatures<float> tdf;
                    //_values[GIST_ROOT_MEAN_SQUARE] = tdf.rootMeanSquare(input);
                    _values[GIST_ROOT_MEAN_SQUARE] = gist.rootMeanSquare();
                    
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                    
                }
                break;
            case GIST_ZERO_CROSSING_RATE:
                if(_doDetect[_features[v]] ){
                    //CoreTimeDomainFeatures<float> tdf;
                    //_values[GIST_ZERO_CROSSING_RATE] = tdf.zeroCrossingRate(input);
                    _values[GIST_ZERO_CROSSING_RATE] = gist.zeroCrossingRate();
                    
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                }
                break;
            case GIST_SPECTRAL_CENTROID:
                if(_doDetect[_features[v]] ){
                    //CoreFrequencyDomainFeatures<float> fdf;
                    //_values[GIST_SPECTRAL_CENTROID] = fdf.spectralCentroid(gist.getMagnitudeSpectrum());
                    _values[GIST_SPECTRAL_CENTROID] = gist.spectralCentroid();
                    
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                }
                break;
            case GIST_SPECTRAL_FLATNESS:
                if(_doDetect[_features[v]] ){
                    //CoreFrequencyDomainFeatures<float> fdf;
                    //_values[GIST_SPECTRAL_FLATNESS] = fdf.spectralFlatness(input);
                    _values[GIST_SPECTRAL_FLATNESS] = gist.spectralFlatness();
                    
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                }
                break;
            case GIST_SPECTRAL_DIFFERENCE:
                if(_doDetect[_features[v]] ){
                    _values[GIST_SPECTRAL_DIFFERENCE] = gist.spectralDifference();
                    
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                }
                break;
                
            case GIST_SPECTRAL_DIFFERENCE_COMPLEX:
                if(_doDetect[_features[v]] ){
                    //OnsetDetectionFunction<float> odf(bufferSize);
                    //_values[GIST_DIFFERENCE] = odf.spectralDifference(input);
                    
                    _values[GIST_SPECTRAL_DIFFERENCE_COMPLEX] = gist.complexSpectralDifference();
                    
                    
                    
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                    
                    
                    
                }
                break;
                
            case GIST_HIGH_FREQUENCY_CONTENT:
                if(_doDetect[_features[v]] ){
                    
                    _values[GIST_HIGH_FREQUENCY_CONTENT] = gist.highFrequencyContent();
                    
                   
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                    
                    
                }
                break;
                
                
                
                
                
                
            case GIST_SPECTRAL_DIFFERENCE_HALFWAY:
                if(_doDetect[_features[v]] ){
         
                    _values[GIST_SPECTRAL_DIFFERENCE_HALFWAY] = gist.spectralDifferenceHWR();
                    
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                }
                break;
                
                
                
            case GIST_SPECTRAL_CREST:
                if(_doDetect[_features[v]] ){
                    //CoreFrequencyDomainFeatures<float> fdf;
                    //_values[GIST_SPECTRAL_CREST] = fdf.spectralCrest(input);
                    
                    _values[GIST_SPECTRAL_CREST] = gist.spectralCrest();
                    
                    if(_useForOnsetDetection[_features[v]]){
                        processOnsetDetection(_features[v]);
                    }
                }
                break;

            default:
                
                break;
        }
        
        

        
        
        
        
        
    }
    
    
    
    
    
    if(_calculateMFCC){
       
        vector<float>mfcc = getMelFrequencyCepstralCoefficients();
        int mfccs = mfcc.size();

        
        while(mfccs--){
            float val = mfcc[mfccs];
            
            //store max/min/avg
            if( val > _mfccMaxValues[mfccs]){
                _mfccMaxValues[mfccs] =  val;
            }

            
            if(val < _mfccMinValues[mfccs]){
                _mfccMinValues[mfccs] =  val;
            }
            
            if(_mfccAvgValues[mfccs]){
                _mfccAvgValues[mfccs] = val;
            }else{
                _mfccAvgValues[mfccs] = (_mfccAvgValues[mfccs]*_mfccAvgValueNum[mfccs] + val)/(float)(_mfccAvgValueNum[mfccs]+1);
            }
            _mfccAvgValueNum[mfccs]++;

            
        }
        
    }
    
    
};


/*
 Any feature can be used for some type of onset detection really
 */
void ofxGist::processOnsetDetection(GIST_FEATURE feature){
    
    
    _history[feature].push_back(_values[feature]);
    

    
    int bigChange = 0;
    
    
    //check changes between last few events
    int frames = _history[feature].size();
    if(frames>1){
        float delta = _history[feature][frames-1] -_history[feature][frames-2];
        
        
        float diffThreshold = _maxValues[feature]*_thresholds[feature];
        //float diffThreshold = _avgValues[feature]/2.0f;
        
        float angle = atan(delta);
        if(delta > diffThreshold){
            bigChange = 1;
        }else if(angle < 0){
            //whenever starts to decline
            bigChange = -1;
        }
    }
    
    
    if(bigChange == 1 && !_isNoteOn){
        GistEvent e;
        e.energy = gist.peakEnergy();
        e.frequency = gist.pitchYin();
        e.feature = feature;
        e.note = getNoteFrequency();
        e.onsetAmount = _values[feature];
        ofNotifyEvent(GistEvent::ON,e);
        _isNoteOn = 1;
    }else if(bigChange == -1 && _isNoteOn){
        GistEvent e;
        e.energy = gist.peakEnergy();
        e.frequency = gist.pitchYin();
        e.feature = feature;
        e.note = getNoteFrequency();
        e.onsetAmount = _values[feature];
        ofNotifyEvent(GistEvent::OFF,e);
        _isNoteOn = 0;
    }
    
    

    int historySize = 100;
    
    if(_history[feature].size()>historySize){
        _history[feature].erase(_history[feature].begin(),_history[feature].begin()+ _history[feature].size()-historySize);
    }

};


void ofxGist::setDetect(GIST_FEATURE f,bool b){
    ofLog()<<"ofxGist will detect "<<ofxGist::getFeatureNames()[f]<<" "<<b<<endl;
    _doDetect[f] = b;
};

bool ofxGist::getDetect(GIST_FEATURE f){
    return _doDetect[f];
};


void ofxGist::setUseForOnsetDetection(GIST_FEATURE f,bool b){
    _useForOnsetDetection[f] = b;
    
    if(!getDetect(f)){
        setDetect(f);
    }
};

bool ofxGist::getUseForOnsetDetection(GIST_FEATURE f){
    return _useForOnsetDetection[f];
};




float ofxGist::getValue(GIST_FEATURE f){
    if(_doDetect[f]){
        return _values[f];
    }else{
        return 0;
    }

};

float ofxGist::getValue(int f){
    GIST_FEATURE feature = static_cast<GIST_FEATURE>(f);
    return getValue(feature);
};


float ofxGist::getMin(GIST_FEATURE f){
    return _minValues[f];
    
};


float ofxGist::getMin(int f){
    GIST_FEATURE feature = static_cast<GIST_FEATURE>(f);
    return getMin(feature);
};


float ofxGist::getMax(GIST_FEATURE f){
    return _maxValues[f];

};


float ofxGist::getMax(int f){
    GIST_FEATURE feature = static_cast<GIST_FEATURE>(f);
    return getMax(feature);
};


float ofxGist::getAvg(GIST_FEATURE f){
    return _avgValues[f];
    
};


float ofxGist::getAvg(int f){
    GIST_FEATURE feature = static_cast<GIST_FEATURE>(f);
    return getAvg(feature);
};



void ofxGist::setThreshold(GIST_FEATURE f, float t){
    _thresholds[f] = t;
};

float ofxGist::getThreshold(GIST_FEATURE f){
    return _thresholds[f];
};

float ofxGist::getThreshold(int f){
    GIST_FEATURE feature = static_cast<GIST_FEATURE>(f);
    return getThreshold(feature);
}




float ofxGist::frequencyToMidi(float freq){
    float midi;
    if (freq < 2. || freq > 100000.) return 0.; // avoid nans and infs
    /* log(freq/A-2)/log(2) */
    midi = freq / 6.875;
    midi = logf (midi) / 0.69314718055995;
    midi *= 12;
    midi -= 3;
    return MAX(0,midi);
};

float ofxGist::getNoteFrequency(){
  if(_doDetect[GIST_PITCH] ){
      return frequencyToMidi(_values[GIST_PITCH]);
  }
  return 0.0;
};


string  ofxGist::getNoteName(){
    int val = getNoteFrequency();
    int relVal = val % 12;
    int oct = floor(val/12);
    
    string n[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    
    return n[relVal]+"-"+ofToString(oct);

};



void ofxGist::clearHistory(){
    for(GIST_FEATURE feature:_features){
        _values[feature] = 0;
        _minValues[feature] = 100000000;
        _maxValues[feature] = 0;
        _avgValues[feature] = 0;
        _avgValueNum[feature] = 0;
    }
    
    
    int v = getMelFrequencyCepstralCoefficients().size();
    
    _mfccMinValues.clear();
    _mfccMaxValues.clear();
    _mfccAvgValues.clear();
    _mfccAvgValueNum.clear();
    
    
    while(v--){
        _mfccMinValues.push_back(100000000);
        _mfccMaxValues.push_back(0);
        _mfccAvgValues.push_back(0);
        _mfccAvgValueNum.push_back(0);
        
    }

};



vector<float> ofxGist::getMelFrequencySpectrum(){
    _calculateMFCC = true;
    return gist.melFrequencySpectrum();
};

vector<float> ofxGist::getMelFrequencyCepstralCoefficients(){
    _calculateMFCC = true;
    return gist.melFrequencyCepstralCoefficients();
};



float ofxGist::getMFCCMin(int coeffNum){
    _calculateMFCC = true;
    return _mfccMinValues[coeffNum];
};

float ofxGist::getMFCCMax(int coeffNum){
    _calculateMFCC = true;
    return _mfccMaxValues[coeffNum];
};

float ofxGist::getMFCCAvg(int coeffNum){
    _calculateMFCC = true;
    return _mfccAvgValues[coeffNum];
};


