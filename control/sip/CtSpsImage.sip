
class CtSpsImage 
{
%TypeHeaderCode
#include <CtSpsImage.h>
using namespace lima;
%End  
 public:
	CtSpsImage();
	~CtSpsImage();
	
	void setNames(const std::string& spec_name, 
		      const std::string& array_name);
	void prepare(const FrameDim &frame_dim);
	void frameReady(Data&);
	void reset();
	void setActive(bool aFlag);
	bool isActive() const;
};

