#ifndef CTSPSIMAGE_H
#define CTSPSIMAGE_H

#include <string>

#include "ThreadUtils.h"
#include "Debug.h"
#include "SizeUtils.h"

#include <Data.h>

namespace lima
{

class _SpsCBK;
class _SpsImage;

class CtSpsImage
{
	DEB_CLASS_NAMESPC(DebModControl,"CtSpsImage","Control");

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

 private:
	friend class _SpsCBK;
	void _update_finnished(Data&);
	void _check_data_size(Data&);
	void _post_sps_task(Data&);

	Cond		m_cond;
	bool		m_ready_flag;
	bool 		m_active_flag;
	
	_SpsCBK 	*m_sps_cbk;
	_SpsImage	*m_sps_cnt;
	Data		m_next_data;
	
};

inline void CtSpsImage::setActive(bool aFlag) 
{
	m_active_flag = aFlag;
}

inline bool CtSpsImage::isActive() const 
{
	return m_active_flag;
}

} // namespace lima

#endif // CTSPSIMAGE_H
