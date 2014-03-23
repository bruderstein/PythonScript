#ifndef DEPTHCOUNTER_20140311_H
#define DEPTHCOUNTER_20140311_H

namespace NppPythonScript
{
    class DepthCounter;

    class DepthLevel {
    friend class DepthCounter;
	public:
		DepthLevel()
			: m_depthCounter(NULL)
		{}
		
        ~DepthLevel();
        
		DepthLevel& operator=(const DepthLevel& rhs);
		DepthLevel(const DepthLevel& copy);
		

	private:
        DepthCounter* m_depthCounter;
        

        DepthLevel(DepthCounter* depthCounter)
			: m_depthCounter(depthCounter)
		{}

	};

    class DepthCounter {
    friend class DepthLevel;
	public:
		DepthCounter() : m_depth(0) {}

        DepthLevel increase() {
            ++m_depth;
            return DepthLevel(this);
		}

		int getDepth() { return m_depth; }

	protected:
        void increaseAsCopy() {
            ++m_depth;
		}

        void decrease() {
            --m_depth;
		}
         
	private:
        int m_depth;
	};
	


}


#endif // DEPTHCOUNTER_20140311_H