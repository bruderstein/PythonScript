#ifndef SCINTILLACALLBACKCOUNTER_20140315_H
#define SCINTILLACALLBACKCOUNTER_20140315_H

#include "DepthCounter.h"

namespace NppPythonScript
{
class ScintillaCallbackCounter
{
public:
	static DepthLevel inCallback() { return s_depth.increase(); }
	static bool isInCallback() { return s_depth.getDepth() != 0; }

private:
    static DepthCounter s_depth;
};

}
#endif // SCINTILLACALLBACKCOUNTER_20140315_H