#include "UtilMac.h"
#import <Foundation/Foundation.h>

namespace UtilMac {

juce::File getSharedApplicationDirectory() {
    NSString* groupId = @"group.com.github.pitchenga";
    NSURL* containerUrl = [[NSFileManager defaultManager] containerURLForSecurityApplicationGroupIdentifier:groupId];
    
    if (containerUrl != nil) {
        juce::String path = juce::String([containerUrl.path UTF8String]);
        return juce::File(path).getChildFile("Pitchenga");
    }
    
    // Fallback if not running in a group container or if the group doesn't exist
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Pitchenga");
}

}
