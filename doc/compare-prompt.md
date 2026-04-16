Analyze the legacy java implementation and our modern C++ JUCE-based port. Create an MD document file with detailed report:
- What is done the same way in both implementations?
- What is done differently?

Legacy:                                                                                                                                                                           
@java/com/harmoneye/analysis/AnalyzedFrame.java                                                                                                                                   
@java/com/harmoneye/analysis/ExpSmoother.java                                                                                                                                     
@java/com/harmoneye/analysis/HarmonicPatternPitchClassDetector.java                                                                                                               
@java/com/harmoneye/analysis/MovingAverageAccumulator.java                                                                                                                        
@java/com/harmoneye/analysis/MusicAnalyzer.java                                                                                                                                   
@java/com/harmoneye/analysis/NoiseGate.java                                                                                                                                       
@java/com/harmoneye/analysis/PercussionSuppressor.java                                                                                                                            
@java/com/harmoneye/analysis/ScalarExpSmoother.java                                                                                                                               
@java/com/harmoneye/analysis/SpectralEqualizer.java                                                                                                                               
@java/com/harmoneye/app/AbstractHarmonEyeApp.java                                                                                                                                 
@java/com/harmoneye/app/CaptureHarmonEyeApp.java                                                                                                                                  
@java/com/harmoneye/app/Config.java                                                                                                                                               
@java/com/harmoneye/app/HarmonEye.java                                                                                                                                            
@java/com/harmoneye/audio/ByteConverter.java                                                                                                                                      
@java/com/harmoneye/audio/Capture.java                                                                                                                                            
@java/com/harmoneye/audio/DecibelCalculator.java                                                                                                                                  
@java/com/harmoneye/audio/DoubleRingBuffer.java                                                                                                                                   
@java/com/harmoneye/audio/MultiRateRingBufferBank.java                                                                                                                            
@java/com/harmoneye/audio/SoundConsumer.java                                                                                                                                      
@java/com/harmoneye/math/cqt/Cqt.java                                                                                                                                             
@java/com/harmoneye/math/cqt/CqtCalculator.java                                                                                                                                   
@java/com/harmoneye/math/cqt/CqtContext.java                                                                                                                                      
@java/com/harmoneye/math/cqt/FastCqt.java                                                                                                                                         
@java/com/harmoneye/math/filter/ButterworthFilter.java                                                                                                                            
@java/com/harmoneye/math/filter/Decimator.java                                                                                                                                    
@java/com/harmoneye/math/matrix/SparseRCComplexMatrix2D.java                                                                                                                      
@java/com/harmoneye/math/stats/Median.java                                                                                                                                        
@java/com/harmoneye/math/window/HammingWindow.java                                                                                                                                
@java/com/harmoneye/math/window/WindowFunction.java                                                                                                                               
@java/com/harmoneye/viz/ColorFunction.java                                                                                                                                        
@java/com/harmoneye/viz/OpenGlCircularVisualizer.java                                                                                                                             
@java/com/harmoneye/viz/Visualizer.java                                                                                                                                           
@java/com/harmoneye/viz/SwingVisualizer.java                                                                                                                                      
@java/com/pitchenga/domain/Tone.java                                                                                                                                              
@java/com/pitchenga/domain/Pitch.java

Modern:                                                                                                                                                                           
@Source/Analyzers.cpp @Source/Analyzers.h                                                                                                                                         
@Source/CqtEngine.cpp @Source/CqtEngine.h                                                                                                                                         
@Source/PluginEditor.h @Source/PluginEditor.cpp                                                                                                                                   
@Source/PluginProcessor.h @Source/PluginProcessor.cpp                                                                                                                             
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄