#include "stdafx.h"
#include "DepthCounter.h"

namespace NppPythonScript
{

DepthLevel::~DepthLevel() {
	if (NULL != m_depthCounter) {
          m_depthCounter->decrease();
	}
}

DepthLevel& DepthLevel::operator=(const DepthLevel& rhs) {
    m_depthCounter = rhs.m_depthCounter;
    if (NULL != m_depthCounter) {
        m_depthCounter->increaseAsCopy();
	}
    return *this;
}


DepthLevel::DepthLevel(const DepthLevel& copy)
	: m_depthCounter(copy.m_depthCounter)
{
    // The copy will also decrement the counter when destroyed, so we need to increase the counter to compensate
    if (m_depthCounter) {
        m_depthCounter->increaseAsCopy();
    }
}

}