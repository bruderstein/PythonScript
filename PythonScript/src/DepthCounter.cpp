#include "stdafx.h"
#include "DepthCounter.h"

namespace NppPythonScript
{

DepthLevel::~DepthLevel() {
	if (NULL != m_depthCounter) {
          m_depthCounter->decrease();
	}
}

}