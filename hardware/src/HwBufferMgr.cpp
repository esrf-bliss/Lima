#include "HwBufferMgr.h"
#include "MemUtils.h"
#include <cstring>

using namespace lima;

/*******************************************************************
 * \brief BufferAllocMgr destructor
 *******************************************************************/

BufferAllocMgr::~BufferAllocMgr()
{
}

void BufferAllocMgr::clearBuffer(int buffer_nb)
{
	ClearBuffer(getBufferPtr(buffer_nb), 1, getFrameDim());
}

void BufferAllocMgr::clearAllBuffers()
{
	int nb_buffers;
	getNbBuffers(nb_buffers);
	for (int i = 0; i < nb_buffers; i++)
		clearBuffer(i);
}


/*******************************************************************
 * \brief SoftBufferAllocMgr constructor
 *******************************************************************/

SoftBufferAllocMgr::SoftBufferAllocMgr()
{
}

SoftBufferAllocMgr::~SoftBufferAllocMgr()
{
	releaseBuffers();
}

int SoftBufferAllocMgr::getMaxNbBuffers(const FrameDim& frame_dim)
{
	return GetDefMaxNbBuffers(frame_dim);
}

void SoftBufferAllocMgr::allocBuffers(int nb_buffers,
				      const FrameDim& frame_dim)
{
	int frame_size = frame_dim.getMemSize();
	if (frame_size <= 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid FrameDim");
       
	int curr_nb_buffers;
	getNbBuffers(curr_nb_buffers);
	if ((frame_dim == m_frame_dim) && (nb_buffers == curr_nb_buffers))
		return;

	releaseBuffers();

	int max_buffers = getMaxNbBuffers(frame_dim);
	if ((nb_buffers < 1) || (nb_buffers > max_buffers))
		throw LIMA_HW_EXC(InvalidValue, "Invalid number of buffers");

	try {
		m_buffer_list.reserve(nb_buffers);
		for (int i = 0; i < nb_buffers; ++i) 
			m_buffer_list.push_back(new char[frame_size]);
	} catch (...) {
		releaseBuffers();
		throw;
	}

	m_frame_dim = frame_dim;
}

void SoftBufferAllocMgr::releaseBuffers()
{
	BufferList& bl = m_buffer_list;
	for (BufferListCIt it = bl.begin(); it != bl.end(); ++it)
		delete [] *it;
	bl.clear();
	m_frame_dim = FrameDim();
}

const FrameDim& SoftBufferAllocMgr::getFrameDim()
{
	return m_frame_dim;
}

void SoftBufferAllocMgr::getNbBuffers(int& nb_buffers)
{
	nb_buffers = m_buffer_list.size();
}

void *SoftBufferAllocMgr::getBufferPtr(int buffer_nb)
{
	return m_buffer_list[buffer_nb];
}


/*******************************************************************
 * \brief BufferCbMgr destructor
 *******************************************************************/

BufferCbMgr::~BufferCbMgr()
{
}

void BufferCbMgr::clearBuffer(int buffer_nb)
{
	int nb_concat_frames;
	getNbConcatFrames(nb_concat_frames);
	ClearBuffer(getBufferPtr(buffer_nb, 0), nb_concat_frames, 
		    getFrameDim());
}

void BufferCbMgr::clearAllBuffers()
{
	int nb_buffers;
	getNbBuffers(nb_buffers);
	for (int i = 0; i < nb_buffers; i++)
		clearBuffer(i);
}

void BufferCbMgr::setStartTimestamp(Timestamp start_ts)
{
	if (!start_ts.isSet())
		throw LIMA_HW_EXC(InvalidValue, "Invalid start timestamp");
	m_start_ts = start_ts;
}

void BufferCbMgr::getStartTimestamp(Timestamp& start_ts) 
{
	start_ts = m_start_ts;
}

void BufferCbMgr::getBufferFrameDim(const FrameDim& single_frame_dim,
				    int nb_concat_frames, 
				    FrameDim& buffer_frame_dim)
{
	if (nb_concat_frames < 1)
		throw LIMA_HW_EXC(InvalidValue, "Invalid nb concat frames");

	buffer_frame_dim = single_frame_dim;
	Size buffer_size = buffer_frame_dim.getSize();
	buffer_size *= Point(1, nb_concat_frames);
	buffer_frame_dim.setSize(buffer_size);
}
 
void BufferCbMgr::acqFrameNb2BufferNb(int acq_frame_nb, int& buffer_nb,
				      int& concat_frame_nb)
{
	int nb_buffers, nb_concat_frames;
	getNbBuffers(nb_buffers);
	getNbConcatFrames(nb_concat_frames);

	buffer_nb = (acq_frame_nb / nb_concat_frames) % nb_buffers;
	concat_frame_nb = acq_frame_nb % nb_concat_frames;
}

BufferCbMgr::Cap lima::operator |(BufferCbMgr::Cap c1, BufferCbMgr::Cap c2)
{
	return BufferCbMgr::Cap(int(c1) | int(c2));
}

BufferCbMgr::Cap lima::operator &(BufferCbMgr::Cap c1, BufferCbMgr::Cap c2)
{
	return BufferCbMgr::Cap(int(c1) & int(c2));
}



/*******************************************************************
 * \brief StdBufferCbMgr constructor
 *******************************************************************/

StdBufferCbMgr::StdBufferCbMgr(BufferAllocMgr& alloc_mgr)
	: m_alloc_mgr(alloc_mgr)

{
	m_nb_concat_frames = 1;
	m_fcb_act = false;
}

StdBufferCbMgr::~StdBufferCbMgr()
{
}

BufferCbMgr::Cap StdBufferCbMgr::getCap()
{
	return Basic | Concat;
}

int StdBufferCbMgr::getMaxNbBuffers(const FrameDim& frame_dim, 
				    int nb_concat_frames)
{
	FrameDim buffer_frame_dim;
	getBufferFrameDim(frame_dim, nb_concat_frames, buffer_frame_dim);
	return m_alloc_mgr.getMaxNbBuffers(buffer_frame_dim);
}

void StdBufferCbMgr::allocBuffers(int nb_buffers, int nb_concat_frames, 
				  const FrameDim& frame_dim)
{
	int frame_size = frame_dim.getMemSize();
	if (frame_size <= 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid FrameDim");
	if (nb_concat_frames < 1)
		throw LIMA_HW_EXC(InvalidValue, "Invalid nb concat frames");

	int curr_nb_buffers;
	getNbBuffers(curr_nb_buffers);
	if ((nb_buffers == curr_nb_buffers) && (frame_dim == m_frame_dim) && 
	    (nb_concat_frames == m_nb_concat_frames))
		return;

	releaseBuffers();

	try {
		FrameDim buffer_frame_dim;
		getBufferFrameDim(frame_dim, nb_concat_frames, 
				  buffer_frame_dim);

		m_alloc_mgr.allocBuffers(nb_buffers, buffer_frame_dim);
		m_frame_dim = frame_dim;
		m_nb_concat_frames = nb_concat_frames;

		int nb_frames = nb_buffers * nb_concat_frames;
		m_info_list.reserve(nb_frames);
		for (int i = 0; i < nb_frames; ++i)
			m_info_list.push_back(HwFrameInfoType());
	} catch (...) {
		releaseBuffers();
		throw;
	}
}

void StdBufferCbMgr::releaseBuffers()
{
	m_alloc_mgr.releaseBuffers();
	m_info_list.clear();
	m_nb_concat_frames = 1;
	m_frame_dim = FrameDim();
}

void StdBufferCbMgr::setFrameCallbackActive(bool cb_active)
{
	m_fcb_act = cb_active;
}

bool StdBufferCbMgr::newFrameReady(HwFrameInfoType& frame_info)
{
	Timestamp now = Timestamp::now();
	Timestamp start;
	getStartTimestamp(start);
	if (!frame_info.frame_timestamp.isSet())
		frame_info.frame_timestamp = now - start;

        int buffer_nb, concat_frame_nb;
	acqFrameNb2BufferNb(frame_info.acq_frame_nb, buffer_nb,
			    concat_frame_nb);
	void *ptr = getBufferPtr(buffer_nb, concat_frame_nb);
	if (!frame_info.frame_ptr)
		frame_info.frame_ptr = ptr;
	else if (frame_info.frame_ptr != ptr)
		throw LIMA_HW_EXC(InvalidValue, "Invalid frame ptr");

	const FrameDim& frame_dim = getFrameDim();
	if (!frame_info.frame_dim)
		frame_info.frame_dim = &frame_dim;
	else if (*frame_info.frame_dim != frame_dim)
		throw LIMA_HW_EXC(InvalidValue, "Invalid frame dim");

	if (frame_info.valid_pixels == 0)
		frame_info.valid_pixels = Point(frame_dim.getSize()).getArea();

	int frame_nb = buffer_nb * m_nb_concat_frames + concat_frame_nb;
	m_info_list[frame_nb] = frame_info;

	if (!m_fcb_act)
		return false;

	return HwFrameCallbackGen::newFrameReady(frame_info);
}

const FrameDim& StdBufferCbMgr::getFrameDim()
{
	return m_frame_dim;
}

void StdBufferCbMgr::getNbBuffers(int& nb_buffers)
{
	m_alloc_mgr.getNbBuffers(nb_buffers);
}

void StdBufferCbMgr::getNbConcatFrames(int& nb_concat_frames)
{
	nb_concat_frames = m_nb_concat_frames;
}

void *StdBufferCbMgr::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
	if (concat_frame_nb >= m_nb_concat_frames)
		throw LIMA_HW_EXC(InvalidValue, "Invalid concat frame nb");
	char *ptr = (char *) m_alloc_mgr.getBufferPtr(buffer_nb);
	return ptr + concat_frame_nb * m_frame_dim.getMemSize();
}

void StdBufferCbMgr::clearBuffer(int buffer_nb)
{
	m_alloc_mgr.clearBuffer(buffer_nb);
}

void StdBufferCbMgr::clearAllBuffers()
{
	m_alloc_mgr.clearAllBuffers();
}

void StdBufferCbMgr::getFrameInfo(int acq_frame_nb, HwFrameInfo& info)
{
	int nb_buffers;
	getNbBuffers(nb_buffers);
	int nb_frames = nb_buffers * m_nb_concat_frames;
	int frame_nb = acq_frame_nb % nb_frames;
	if (m_info_list[frame_nb].acq_frame_nb != acq_frame_nb)
		throw LIMA_HW_EXC(Error, "Frame not available");

	info = m_info_list[frame_nb];
}

/*******************************************************************
 * \brief BufferCtrlMgr constructor
 *******************************************************************/

BufferCtrlMgr::BufferCtrlMgr(BufferCbMgr& acq_buffer_mgr)
	: m_nb_concat_frames(1), m_nb_acc_frames(1),
	  m_acq_buffer_mgr(acq_buffer_mgr), 
	  m_aux_buffer_mgr(m_aux_alloc_mgr),
	  m_frame_cb(*this),
	  m_frame_cb_act(false)
{
	m_acq_buffer_mgr.registerFrameCallback(m_frame_cb);
	m_effect_buffer_mgr = &m_acq_buffer_mgr;
}

BufferCtrlMgr::~BufferCtrlMgr()
{
	releaseBuffers();
}

void BufferCtrlMgr::releaseBuffers()
{
	m_acq_buffer_mgr.releaseBuffers();
	m_aux_buffer_mgr.releaseBuffers();
}

void BufferCtrlMgr::setFrameDim(const FrameDim& frame_dim)
{
	if (frame_dim != m_frame_dim)
		releaseBuffers();

	m_frame_dim = frame_dim;
}

void BufferCtrlMgr::getFrameDim(FrameDim& frame_dim)
{
	frame_dim = m_effect_buffer_mgr->getFrameDim();
}

void BufferCtrlMgr::setNbConcatFrames(int nb_concat_frames)
{
	bool ask_concat = (nb_concat_frames > 1);
	if ((getAcqMode() == Acc) && ask_concat)
		throw LIMA_HW_EXC(InvalidValue, "Frame acc. is active");

	bool can_concat = (m_acq_buffer_mgr.getCap() & BufferCbMgr::Concat);
	if (ask_concat && !can_concat)
		throw LIMA_HW_EXC(NotSupported, "Strip concat. not supported");

	if (nb_concat_frames != m_nb_concat_frames)
		releaseBuffers();

	m_nb_concat_frames = nb_concat_frames;
}

void BufferCtrlMgr::getNbConcatFrames(int& nb_concat_frames)
{
	nb_concat_frames = m_nb_concat_frames;
}

void BufferCtrlMgr::setNbAccFrames(int nb_acc_frames)
{
	bool ask_acc = (nb_acc_frames > 1);
	if ((getAcqMode() == Concat) && ask_acc)
		throw LIMA_HW_EXC(InvalidValue, "Stripe concat. is active");

	if (ask_acc && (m_effect_buffer_mgr != &m_aux_buffer_mgr))
		m_effect_buffer_mgr = &m_aux_buffer_mgr;
	else if (!ask_acc && (m_effect_buffer_mgr != &m_acq_buffer_mgr))
		m_effect_buffer_mgr = &m_acq_buffer_mgr;

	if (nb_acc_frames != m_nb_acc_frames)
		releaseBuffers();

	m_nb_acc_frames = nb_acc_frames;
}

void BufferCtrlMgr::getNbAccFrames(int& nb_acc_frames)
{
	nb_acc_frames = m_nb_acc_frames;
}

void BufferCtrlMgr::setNbBuffers(int nb_buffers)
{
	int curr_nb_buffers;
	getNbBuffers(curr_nb_buffers);
	if (nb_buffers == curr_nb_buffers)
		return;

	int max_nb_buffers;
	getMaxNbBuffers(max_nb_buffers);
	if ((nb_buffers > 0) && (nb_buffers > max_nb_buffers))
		throw LIMA_HW_EXC(InvalidValue, "Too many buffers");
	else if (nb_buffers == 0)
		nb_buffers = max_nb_buffers;

	bool is_acc = (getAcqMode() == Acc);
	int acc_nb_buffers = 2 * m_nb_acc_frames;
	int acq_nb_buffers = is_acc ? acc_nb_buffers : nb_buffers;
	m_acq_buffer_mgr.allocBuffers(acq_nb_buffers, m_nb_concat_frames, 
				      m_frame_dim);

	if (is_acc) {
		FrameDim aux_frame_dim = m_frame_dim;
		aux_frame_dim.setImageType(Bpp32);
		m_aux_buffer_mgr.allocBuffers(nb_buffers, 1, aux_frame_dim);
	}
}

void BufferCtrlMgr::getNbBuffers(int& nb_buffers)
{
	m_effect_buffer_mgr->getNbBuffers(nb_buffers);
}

void BufferCtrlMgr::getMaxNbBuffers(int& max_nb_buffers)
{
	int concat_frames = m_nb_concat_frames;
	max_nb_buffers = m_effect_buffer_mgr->getMaxNbBuffers(m_frame_dim,
							      concat_frames);
}

void BufferCtrlMgr::setFrameCallbackActive(bool cb_active)
{
	m_frame_cb_act = cb_active;
}

BufferCbMgr& BufferCtrlMgr::getAcqBufferMgr()
{
	return m_acq_buffer_mgr;
}

void BufferCtrlMgr::setStartTimestamp(Timestamp start_ts)
{
	m_acq_buffer_mgr.setStartTimestamp(start_ts);
	if (getAcqMode() == Acc)
		m_aux_buffer_mgr.setStartTimestamp(start_ts);
}

void BufferCtrlMgr::getStartTimestamp(Timestamp& start_ts)
{
	m_effect_buffer_mgr->getStartTimestamp(start_ts);
}

void BufferCtrlMgr::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	m_effect_buffer_mgr->getFrameInfo(acq_frame_nb, info);
}

void *BufferCtrlMgr::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
	return m_effect_buffer_mgr->getBufferPtr(buffer_nb, concat_frame_nb);
}

void *BufferCtrlMgr::getFramePtr(int acq_frame_nb)
{
	HwFrameInfoType info;
	getFrameInfo(acq_frame_nb, info);
	return info.frame_ptr;
}

BufferCtrlMgr::AcqMode BufferCtrlMgr::getAcqMode()
{
	if (m_nb_concat_frames > 1)
		return Concat;
	else if (m_nb_acc_frames > 1)
		return Acc;
	else
		return Normal;
}

bool BufferCtrlMgr::acqFrameReady(const HwFrameInfoType& acq_frame_info)
{
	if (!acq_frame_info.isValid()) {
		if (m_frame_cb_act)
			return newFrameReady(acq_frame_info);
		return true;
	}

	if (getAcqMode() == Acc) {
		StdBufferCbMgr& aux_mgr = m_aux_buffer_mgr;

		int frame_nb = acq_frame_info.acq_frame_nb;
		int aux_frame_nb = frame_nb / m_nb_acc_frames;
		int acc_idx = frame_nb % m_nb_acc_frames;
		int aux_buffer_nb, aux_buffer_frame;
		aux_mgr.acqFrameNb2BufferNb(aux_frame_nb,aux_buffer_nb,
					    aux_buffer_frame);
		if (acc_idx == 0)
			aux_mgr.clearBuffer(aux_buffer_nb);

		void *acq_frame_ptr = acq_frame_info.frame_ptr;
		void *aux_frame_ptr = aux_mgr.getBufferPtr(aux_buffer_nb,
							   aux_buffer_frame);

		const FrameDim& acq_frame_dim = *acq_frame_info.frame_dim;
		const FrameDim& aux_frame_dim = aux_mgr.getFrameDim();

		int valid_pixels = acq_frame_info.valid_pixels;
		accFrame(acq_frame_ptr, acq_frame_dim, 
			 aux_frame_ptr, aux_frame_dim, valid_pixels);
		
		if ((acc_idx == m_nb_acc_frames - 1) && (m_frame_cb_act)) {
			Timestamp start_ts;
			aux_mgr.getStartTimestamp(start_ts);
			Timestamp frame_ts = Timestamp::now() - start_ts;
			HwFrameInfoType aux_frame_info(aux_frame_nb,
						       aux_frame_ptr,
						       &aux_frame_dim,
						       frame_ts,
						       valid_pixels);
			return newFrameReady(aux_frame_info);
		}
	} else {
		if (m_frame_cb_act)
			return newFrameReady(acq_frame_info);
	}

	return true;
}


template <class sdepth, class ddepth> 
void accumulateFrame( void *src_ptr, const FrameDim& src_frame_dim,
                      void *dst_ptr, const FrameDim& dst_frame_dim )
{
	int swidth = src_frame_dim.getSize().getWidth();
	int sheight = src_frame_dim.getSize().getHeight();

	int dwidth = dst_frame_dim.getSize().getWidth();
	int dheight = dst_frame_dim.getSize().getHeight();

	if( (dwidth < swidth) || (dheight < sheight) ) { // Do we need this?
		throw LIMA_HW_EXC(InvalidValue, "Accumulation buffer is too "
		                                "small");
	}

	sdepth *sp = (sdepth *) src_ptr;
	ddepth *dp0 = (ddepth *) dst_ptr, *dp;

	int x0 = (dwidth - swidth)/2;
	int y0 = (dheight - sheight)/2;
	for( int y=y0; y<sheight+y0; y++ ) {
		for( int x=x0; x<swidth+x0; x++ ) {
			dp = dp0 + y*dwidth + x;
			*dp += *sp++;
		}
	}
}


void BufferCtrlMgr::accFrame(void *src_ptr, const FrameDim& src_frame_dim,
			     void *dst_ptr, const FrameDim& dst_frame_dim,
			     int& valid_pixels)
{
	ImageType s_type = src_frame_dim.getImageType();
	ImageType d_type = dst_frame_dim.getImageType();

	if( s_type > d_type ) {
		throw LIMA_HW_EXC(InvalidValue, "Accumulation buffer is too "
		                                "small");
	}

	int type = s_type + d_type;
	switch( type ) {  // In our specific case the sum is unambiguous!
		case (Bpp8 + Bpp8):
			accumulateFrame <unsigned char,unsigned char>(src_ptr,
			              src_frame_dim, dst_ptr, dst_frame_dim );
			break;
		case (Bpp8 + Bpp16):
			accumulateFrame <unsigned char,unsigned short>(src_ptr,
			              src_frame_dim, dst_ptr, dst_frame_dim );
			break;
		case (Bpp16 + Bpp16):
			accumulateFrame <unsigned short,unsigned short>(src_ptr,
			              src_frame_dim, dst_ptr, dst_frame_dim );
			break;
		case (Bpp8 + Bpp32):
			accumulateFrame <unsigned char,unsigned int>(src_ptr,
			               src_frame_dim, dst_ptr, dst_frame_dim );
			break;
		case (Bpp16 + Bpp32):
			accumulateFrame <unsigned short,unsigned int>(src_ptr,
			               src_frame_dim, dst_ptr, dst_frame_dim );
			break;
		case (Bpp32 + Bpp32):
			accumulateFrame <unsigned int,unsigned int>(src_ptr,
			               src_frame_dim, dst_ptr, dst_frame_dim );
			break;
		default:
			throw LIMA_HW_EXC(InvalidValue,"Unsupported image type");
	}
}

