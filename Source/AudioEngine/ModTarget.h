//
//  ModTarget.hpp
//  Trio
//
//  Created by Matthias Pueski on 09.12.16.
//
//

#ifndef ModTarget_hpp
#define ModTarget_hpp

#include <stdio.h>
#include "Modulator.h"

class ModTarget {

public:
    
    inline virtual ~ModTarget() {};
    
    virtual void setModulator(Modulator* mod) = 0;

private:
	
    
};


#endif /* ModTarget_hpp */
