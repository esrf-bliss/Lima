#include "EspiaAcq.h"
#include "MemUtils.h"

using namespace lima;
using namespace lima::Espia;

#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)

#define ESPIA_MIN_BUFFER_SIZE	(128 * 1024)

AcqEndCallback::AcqEndCallback()
	: m_acq(NULL)
{
}

AcqEndCallback::~AcqEndCallback()
{
	if (m_acq)
		m_acq->unregisterAcqEndCallback(*this);
}

Acq *AcqEndCallback::getAcq() const
{ 
	return m_acq; 
}

void AcqEndCallback::setAcq(Acq *acq)
{
	if (acq && m_acq)
		throw LIMA_HW_EXC(InvalidValue, "Acq already set");
	else if (!acq && !m_acq)
		throw LIMA_HW_EXC(InvalidValue, "Acq already reset");

	m_acq = acq;
}


Acq::Acq(Dev& dev)
	: m_dev(dev)
{
	m_nb_buffers = m_nb_buffer_frames = 0;
	m_real_frame_factor = m_real_frame_size = 0;
	
	m_nb_frames = 0;
	m_started = false;

	resetFrameInfo(m_last_frame_info);
	m_frame_cb_nb = Invalid;
	m_user_frame_cb_act = false;

	m_acq_end_cb = NULL;

	enableFrameCallback();
}

Acq::~Acq()
{
	if (m_acq_end_cb)
		unregisterAcqEndCallback(*m_acq_end_cb);

	bufferFree();
	disableFrameCallback();
}

int Acq::dispatchFrameCallback(struct espia_cb_data *cb_data)
{
	Acq *espia = (Acq *) cb_data->data;

	void (Acq::*method)(struct espia_cb_data *cb_data) = NULL;

	int& cb_nb = cb_data->cb_nr;
	if (cb_nb == espia->m_frame_cb_nb)
		method = &Acq::processFrameCallback;
	
	if (method) {
		try {
			(espia->*method)(cb_data);
		} catch (...) {
			
		}
	}
	
	return ESPIA_OK;
}

void Acq::enableFrameCallback()
{
	int& cb_nb = m_frame_cb_nb;
	if (cb_nb != Invalid)
		return;

	struct espia_cb_data cb_data;
	cb_data.type    = ESPIA_CB_ACQ;
	cb_data.cb      = dispatchFrameCallback;
	cb_data.data    = this;
	cb_data.timeout = SCDXIPCI_BLOCK_FOREVER;

	struct img_frame_info& req_finfo = cb_data.info.acq.req_finfo;
	req_finfo.buffer_nr    = ESPIA_ACQ_ANY;
	req_finfo.frame_nr     = ESPIA_ACQ_ANY;
	req_finfo.round_count  = ESPIA_ACQ_ANY;
	req_finfo.acq_frame_nr = ESPIA_ACQ_EACH;

	m_dev.registerCallback(cb_data, cb_nb);
}

void Acq::disableFrameCallback()
{
	int& cb_nb = m_frame_cb_nb;
	if (cb_nb == Invalid)
		return;

	m_dev.unregisterCallback(cb_nb);
}

void Acq::setFrameCallbackActive(bool cb_active)
{
	m_user_frame_cb_act = cb_active;
}

void Acq::processFrameCallback(struct espia_cb_data *cb_data)
{
	AutoMutex l = acqLock();

	struct img_frame_info& cb_finfo = cb_data->info.acq.cb_finfo;
	bool aborted = finished_espia_frame_info(&cb_finfo, cb_data->ret);
	bool finished = aborted;
	if (!aborted) {
		m_last_frame_info = cb_finfo;
		bool endless = (m_nb_frames == 0);
		unsigned long last_frame = m_nb_frames - 1;
		finished = (!endless && (cb_finfo.acq_frame_nr == last_frame));
	}

	if (finished)
		m_started = false;

	l.unlock();

	HwFrameInfo hw_finfo;
	if (!aborted)
		real2virtFrameInfo(cb_finfo, hw_finfo);

	if (m_user_frame_cb_act)
		newFrameReady(hw_finfo);

	if ((m_acq_end_cb != NULL) && finished)
		m_acq_end_cb->acqFinished(hw_finfo);
}

void Acq::bufferAlloc(int& nb_buffers, int nb_buffer_frames,
		      const FrameDim& frame_dim)
{
	if (!frame_dim.isValid() || (nb_buffers <= 0) || 
	    (nb_buffer_frames <= 0))
		throw LIMA_HW_EXC(InvalidValue, "Invalid frame_dim, "
				  "nb_buffers and/or nb_buffer_frames");

	if ((frame_dim == m_frame_dim) && (nb_buffers == m_nb_buffers) &&
	    (nb_buffer_frames == m_nb_buffer_frames))
		return;

	bufferFree();

	int& virt_buffers   = nb_buffers;
	int& virt_frames    = nb_buffer_frames;

	int real_buffers    = virt_buffers;
	int real_frames     = virt_frames;
	int real_frame_size = frame_dim.getMemSize();
	if (virt_frames == 1) {
		int page_size;
		GetPageSize(page_size);
		real_frame_size += page_size - 1;
		real_frame_size &= ~(page_size - 1);
	}

	int real_buffer_size = real_frame_size * real_frames;
	int frame_factor = 1;
	if (real_buffer_size < ESPIA_MIN_BUFFER_SIZE) {
		frame_factor  = ESPIA_MIN_BUFFER_SIZE / real_buffer_size;
		real_frames  *= frame_factor;
		real_buffers += frame_factor - 1;
		real_buffers /= frame_factor;
		virt_buffers  = real_buffers * frame_factor;
	}

	CHECK_CALL(espia_buffer_alloc(m_dev, real_buffers, real_frames,
				      real_frame_size));

	m_frame_dim         = frame_dim;
	m_nb_buffers        = virt_buffers;
	m_nb_buffer_frames  = virt_frames;
	m_real_frame_factor = frame_factor;
	m_real_frame_size   = real_frame_size;
}

void Acq::bufferFree()
{
	if ((m_nb_buffers == 0) || (m_nb_buffer_frames == 0))
		return;

	stop();

	CHECK_CALL(espia_buffer_free(m_dev));

	m_frame_dim = FrameDim();
	m_nb_buffers = m_nb_buffer_frames = 0;
	m_real_frame_factor = m_real_frame_size = 0;
}

const FrameDim& Acq::getFrameDim()
{
	return m_frame_dim;
}

void Acq::getNbBuffers(int& nb_buffers)
{
	nb_buffers = m_nb_buffers;
}

void Acq::getNbBufferFrames(int& nb_buffer_frames)
{
	nb_buffer_frames = m_nb_buffer_frames;
}

void *Acq::getBufferFramePtr(int buffer_nb, int frame_nb)
{
	int real_buffer = realBufferNb(buffer_nb, frame_nb);
	int real_frame  = realFrameNb(buffer_nb, frame_nb);
	void *ptr;
	CHECK_CALL(espia_frame_address(m_dev, real_buffer, real_frame, &ptr));
	return ptr;
}

void *Acq::getAcqFramePtr(int acq_frame_nb)
{
	unsigned long buffer_nb = ESPIA_ACQ_ANY;
	void *ptr;
	CHECK_CALL(espia_frame_address(m_dev, buffer_nb, acq_frame_nb, &ptr));
	return ptr;
}

void Acq::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	struct img_frame_info finfo;
	finfo.buffer_nr    = ESPIA_ACQ_ANY;
	finfo.frame_nr     = ESPIA_ACQ_ANY;
	finfo.round_count  = ESPIA_ACQ_ANY;
	finfo.acq_frame_nr = acq_frame_nb;

	int ret = espia_get_frame(m_dev, &finfo, SCDXIPCI_NO_BLOCK);
	if (ret == SCDXIPCI_ERR_NOTREADY) {
		info = HwFrameInfo();
		return;
	}
	CHECK_CALL(ret);

	real2virtFrameInfo(finfo, info);
}

void Acq::real2virtFrameInfo(const struct img_frame_info& real_info, 
			     HwFrameInfoType& virt_info)
{
	char *buffer_ptr = (char *) real_info.buffer_ptr;
	int frame_offset = real_info.frame_nr * m_real_frame_size;

	virt_info.acq_frame_nb    = real_info.acq_frame_nr;
	virt_info.frame_ptr       = buffer_ptr + frame_offset;
	virt_info.frame_dim       = &m_frame_dim;
	virt_info.frame_timestamp = USec2Sec(real_info.time_us);
	virt_info.valid_pixels    = real_info.pixels;
}

void Acq::resetFrameInfo(struct img_frame_info& frame_info)
{
	frame_info.buffer_ptr   = NULL;
	frame_info.buffer_nr    = SCDXIPCI_INVALID;
	frame_info.frame_nr     = SCDXIPCI_INVALID;
	frame_info.round_count  = SCDXIPCI_INVALID;
	frame_info.acq_frame_nr = SCDXIPCI_INVALID;
	frame_info.time_us      = 0;
	frame_info.pixels       = 0;
}

void Acq::setNbFrames(int nb_frames)
{
	if (nb_frames < 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid nb of frames");

	m_nb_frames = nb_frames;
}

void Acq::getNbFrames(int& nb_frames)
{
	nb_frames = m_nb_frames;
}

void Acq::start()
{
	AutoMutex l = acqLock();

	if (m_started)
		throw LIMA_HW_EXC(Error, "Acquisition already running");

	resetFrameInfo(m_last_frame_info);

	CHECK_CALL(espia_start_acq(m_dev, 0, m_nb_frames, SCDXIPCI_NO_BLOCK));

	m_start_ts = Timestamp::now();
	m_started = true;
}

void Acq::stop()
{
	AutoMutex l = acqLock();
	if (!m_started)
		return;

	l.unlock();
	CHECK_CALL(espia_stop_acq(m_dev));
	l.lock();

	m_started = false;
}

void Acq::getStatus(StatusType& status)
{
	AutoMutex l = acqLock();

	unsigned long acq_run_nb;
	int acq_running = espia_acq_active(m_dev, &acq_run_nb);
	CHECK_CALL(acq_running);

	status.running = m_started;
	status.run_nb  = acq_run_nb;
	status.last_frame_nb = m_last_frame_info.acq_frame_nr;
}

void Acq::registerAcqEndCallback(AcqEndCallback& acq_end_cb)
{
	if (m_acq_end_cb)
		throw LIMA_HW_EXC(InvalidValue, 
				  "AcqEndCallback already registered");
	
	acq_end_cb.setAcq(this);
	m_acq_end_cb = &acq_end_cb;
}

void Acq::unregisterAcqEndCallback(AcqEndCallback& acq_end_cb)
{
	if (m_acq_end_cb != &acq_end_cb)
		throw LIMA_HW_EXC(InvalidValue, 
				  "Specified AcqEndCallback not registered");
	
	stop();

	m_acq_end_cb = NULL;
	acq_end_cb.setAcq(NULL);
}
