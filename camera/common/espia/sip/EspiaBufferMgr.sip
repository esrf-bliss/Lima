
namespace Espia
{

class BufferMgr : BufferCbMgr
{

%TypeHeaderCode
#include "EspiaBufferMgr.h"
#include "MemUtils.h"
%End

 public:
	BufferMgr(Espia::Acq& acq);
	virtual ~BufferMgr();

	virtual Cap getCap();

	virtual int getMaxNbBuffers(const FrameDim& frame_dim,
				    int nb_concat_frames);
	virtual void allocBuffers(int nb_buffers, int nb_concat_frames, 
				  const FrameDim& frame_dim);
	virtual const FrameDim& getFrameDim();
	virtual void getNbBuffers(int& nb_buffers /Out/);
	virtual void getNbConcatFrames(int& nb_concat_frames /Out/);
	virtual void releaseBuffers();

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb);

	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info /Out/);

	virtual void setStartTimestamp(Timestamp  start_ts);
	virtual void getStartTimestamp(Timestamp& start_ts /Out/);

 protected:
/* XXX This gives errors in SIP
	class FrameCallback : public HwFrameCallback
	{
	public:
		FrameCallback(BufferMgr& buffer_mgr) 
			: m_buffer_mgr(buffer_mgr) {}

		virtual bool newFrameReady(const HwFrameInfoType& frame_info)
		{
			return m_buffer_mgr.newFrameReady(frame_info);
		}
			
	protected:
		BufferMgr& m_buffer_mgr;
	};
*/
	virtual void setFrameCallbackActive(bool cb_active);
	
};


}; // namespace Espia
