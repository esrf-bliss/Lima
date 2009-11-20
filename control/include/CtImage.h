#ifndef CTIMAGE_H
#define CTIMAGE_H

#include "Constants.h"
#include "HwInterface.h"
#include "HwDetInfoCtrlObj.h"
#include "SoftOpInternalMgr.h"

namespace lima {

class CtImage;

class CtSwBinRoi {
	DEB_CLASS_NAMESPC(DebModControl,"Sofware BinRoi","Control");
    public:
	friend std::ostream& operator<<(std::ostream &os,const CtSwBinRoi &binroi);

	CtSwBinRoi(Size& size);

	void setMaxSize(Size& size);
	void setBin(const Bin& bin);
	void setRoi(const Roi& roi);

	void resetBin();
	void resetRoi();
	void reset();

	const Bin& getBin() const { return m_bin; }
	const Roi& getRoi() const { return m_roi; }
	const Size& getSize();

	bool apply(SoftOpInternalMgr *op);

    private:
	Size	m_max_size, m_size;
	Bin	m_bin;
	Roi	m_roi, m_max_roi;
};


class CtHwBinRoi {
	DEB_CLASS_NAMESPC(DebModControl,"Hardware BinRoi","Control");
    public:
	CtHwBinRoi(HwInterface *hw, CtSwBinRoi *sw_bin_roi, Size& size);
	~CtHwBinRoi();

	bool hasBinCapability() { return m_has_bin; }
	bool hasRoiCapability() { return m_has_roi; }

	void setMaxSize(const Size& size);
	void setBin(Bin& bin, bool round);
	void setRoi(Roi& roi, bool round);

	void resetBin();
	void resetRoi();
	void reset();

	const Bin& getBin() const { return m_bin; }
	const Roi& getRoi() const { return m_roi; }
	const Size& getSize() const { return m_size; }

	void apply();

    private:
	void _updateSize();

	HwBinCtrlObj	*m_hw_bin;
	HwRoiCtrlObj	*m_hw_roi;
	HwFlipCtrlObj	*m_hw_flip;
	CtSwBinRoi	*m_sw_bin_roi;
	bool	m_has_bin, m_has_roi, m_has_flip;
	Size	m_max_size, m_size;
	Bin	m_bin;
	Roi	m_roi, m_max_roi;
};


class CtMaxImageSizeCB : public HwMaxImageSizeCallback
{
    public:
	CtMaxImageSizeCB(CtImage *ct) : m_ct(ct) {}
    protected:	
	void maxImageSizeChanged(const Size& size, ImageType image_type);
    private:
	CtImage *m_ct;
};

	
class CtImage {
	DEB_CLASS_NAMESPC(DebModControl,"Image","Control");
    public:
	friend class CtMaxImageSizeCB;

	enum ImageOpMode {
		HardOnly,
		SoftOnly,
		HardAndSoft,
	};

	CtImage(HwInterface *hw);
	~CtImage();

	void getMaxImageSize(Size& size) const;
	void setMaxImage(const Size &size, ImageType type);

	void getImageType(ImageType& type) const;
	void getHwImageDim(FrameDim& dim) const;
	void getImageDim(FrameDim& dim) const;

	// --- soft
	void getSoft(CtSwBinRoi *& soft) const;
	void getHard(CtHwBinRoi *& hard) const;

	// --- wizard
	void setMode(ImageOpMode mode);
	void getMode(ImageOpMode& mode) const;

	void setRoi(Roi& roi);
	void setBin(Bin& bin);

	void resetRoi();
	void resetBin();

	// --- effective
	void getRoi(Roi& roi) const;
	void getBin(Bin& bin) const;

	void reset();

	void applyHard();
	bool applySoft(SoftOpInternalMgr *op);

    private:
	void _setHSRoi(const Roi &roi);
	void _setHSBin(const Bin &bin);

	HwDetInfoCtrlObj *m_hw_det;
	CtMaxImageSizeCB	*m_cb_size;
	CtSwBinRoi	*m_sw;
	CtHwBinRoi	*m_hw;

	Size		m_max_size;
	ImageType	m_img_type;
	ImageOpMode	m_mode;
};
 
inline std::ostream& operator<<(std::ostream &os,const CtSwBinRoi &binroi)
{
	os << "<"
	   << "m_max_size=" << binroi.m_max_size << ", "
	   << "m_size=" << binroi.m_size << ", "
	   << "m_bin=" << binroi.m_bin << ", "
	   << "m_roi=" << binroi.m_roi << ", "
	   << "m_max_roi=" << binroi.m_max_roi
	   << ">";
	return os;
}

} // namespace lima

#endif // CTIMAGE_H
