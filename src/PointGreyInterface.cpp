//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#include "PointGreyInterface.h"
#include <algorithm>

using namespace lima;
using namespace lima::PointGrey;
using namespace std;

/*******************************************************************
 * \brief DetInfoCtrlObj constructor
 *******************************************************************/
DetInfoCtrlObj::DetInfoCtrlObj(Camera& cam)
    : m_cam(cam)
{
    DEB_CONSTRUCTOR();
}


//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getMaxImageSize(Size& size)
{
    DEB_MEMBER_FUNCT();
    m_cam.getDetectorImageSize(size);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getDetectorImageSize(Size& size)
{
    DEB_MEMBER_FUNCT();
    m_cam.getDetectorImageSize(size);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getDefImageType(ImageType& image_type)
{
    DEB_MEMBER_FUNCT();
    image_type = Bpp16;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getCurrImageType(ImageType& image_type)
{
    DEB_MEMBER_FUNCT();

    VideoMode video_mode;
    m_cam.getVideoMode(video_mode);

    switch (video_mode)
    {
    case Y8:
    	image_type = Bpp8;
    	break;
    case Y16:
    	image_type = Bpp16;
    	break;
    default:
    	// TODO: handle error
    	return;
    }
    DEB_RETURN() << DEB_VAR1(image_type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::setCurrImageType(ImageType image_type)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(image_type);

    VideoMode video_mode;
    switch (image_type)
    {
    case Bpp8:
    	video_mode = Y8;
    	break;
    case Bpp16:
    	video_mode = Y16;
    	break;
    default:
    	// TODO: handle error
    	return;
    }
    m_cam.setVideoMode(video_mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getPixelSize(double& x_size, double& y_size)
{
    DEB_MEMBER_FUNCT();
    x_size = y_size = -1.; // TODO: don't know
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getDetectorType(std::string& type)
{
    DEB_MEMBER_FUNCT();
    m_cam.getDetectorType(type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getDetectorModel(std::string& model)
{
    DEB_MEMBER_FUNCT();
    m_cam.getDetectorModel(model);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
    DEB_MEMBER_FUNCT();
    m_cam.registerMaxImageSizeCallback(cb);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
    DEB_MEMBER_FUNCT();
    m_cam.unregisterMaxImageSizeCallback(cb);
}

/*******************************************************************
 * \brief SyncCtrlObj constructor
 *******************************************************************/
SyncCtrlObj::SyncCtrlObj(Camera& cam)
    : HwSyncCtrlObj()
    , m_cam(cam)
{
    DEB_CONSTRUCTOR();
    m_cam.m_sync = this;
}


//-----------------------------------------------------
//
//-----------------------------------------------------
bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
    DEB_MEMBER_FUNCT();
    return true;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
    DEB_MEMBER_FUNCT();
    if (!checkTrigMode(trig_mode))
        THROW_HW_ERROR(InvalidValue) << "Invalid " << DEB_VAR1(trig_mode);
    m_cam.setTrigMode(trig_mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
    m_cam.getTrigMode(trig_mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setExpTime(double exp_time)
{
    m_cam.setExpTime(exp_time);
    ValidRangesType valid_ranges;
    getValidRanges(valid_ranges);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getExpTime(double& exp_time)
{
    m_cam.getExpTime(exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setLatTime(double lat_time)
{
    m_cam.setLatTime(lat_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getLatTime(double& lat_time)
{
    m_cam.getLatTime(lat_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setNbHwFrames(int nb_frames)
{
    m_cam.setNbFrames(nb_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
    m_cam.getNbFrames(nb_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
    DEB_MEMBER_FUNCT();
    double min_time;
    double max_time;

    m_cam.getExpTimeRange(min_time, max_time);
    valid_ranges.min_exp_time = min_time;
    valid_ranges.max_exp_time = max_time;

    m_cam.getLatTimeRange(min_time, max_time);
    valid_ranges.min_lat_time = min_time;
    valid_ranges.max_lat_time = max_time;
}

/*******************************************************************
 * \brief RoiCtrlObj constructor
 *******************************************************************/

RoiCtrlObj::RoiCtrlObj(Camera& cam)
    : m_cam(cam)
{
    DEB_CONSTRUCTOR();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void RoiCtrlObj::checkRoi(const Roi& set_roi, Roi& hw_roi)
{
    DEB_MEMBER_FUNCT();
    m_cam.checkRoi(set_roi, hw_roi);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void RoiCtrlObj::setRoi(const Roi& roi)
{
    DEB_MEMBER_FUNCT();
    Roi real_roi;
    checkRoi(roi,real_roi);
    m_cam.setRoi(real_roi);

}

//-----------------------------------------------------
//
//-----------------------------------------------------
void RoiCtrlObj::getRoi(Roi& roi)
{
    DEB_MEMBER_FUNCT();
    m_cam.getRoi(roi);
}

/*******************************************************************
 * \brief BinCtrlObj constructor
 *******************************************************************/
BinCtrlObj::BinCtrlObj(Camera &cam)
    : m_cam(cam)
{
    DEB_CONSTRUCTOR();
}

void BinCtrlObj::setBin(const Bin& aBin)
{
    m_cam.setBin(aBin);
}

void BinCtrlObj::getBin(Bin &aBin)
{
    m_cam.getBin(aBin);
}

void BinCtrlObj::checkBin(Bin &aBin)
{
    m_cam.checkBin(aBin);
}


/*******************************************************************
 * \brief VideoCtrlObj constructor
 *******************************************************************/
VideoCtrlObj::VideoCtrlObj(Camera &cam)
    : m_cam(cam)
    , m_live(false)
{
    DEB_CONSTRUCTOR();
    m_cam.m_video = this;
}


void VideoCtrlObj::getSupportedVideoMode(std::list<VideoMode> &mode_list) const
{
    DEB_MEMBER_FUNCT();
    mode_list.push_back(Y8);
    mode_list.push_back(Y16);
}

void VideoCtrlObj::getVideoMode(VideoMode& mode) const
{
    DEB_MEMBER_FUNCT();
    m_cam.getVideoMode(mode);
    DEB_RETURN() << DEB_VAR1(mode);
}

void VideoCtrlObj::setVideoMode(VideoMode mode)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(mode);
    m_cam.setVideoMode(mode);
}

void VideoCtrlObj::getLive(bool &live) const
{
    DEB_MEMBER_FUNCT();
    live = m_live;
    DEB_RETURN() << DEB_VAR1(live);
}

void VideoCtrlObj::setLive(bool live)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(live);
    m_live = live;
    if (live){
        m_cam.setNbFrames(0);
	m_cam.startAcq();
    }
    else
        m_cam.stopAcq();
}

void VideoCtrlObj::getGain(double &gain) const
{
    DEB_MEMBER_FUNCT();
    m_cam.getGain(gain);
    DEB_RETURN() << DEB_VAR1(gain);
}

void VideoCtrlObj::setGain(double gain)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(gain);
    m_cam.setGain(gain);
}

/*******************************************************************
 * \brief Hw Interface constructor
 *******************************************************************/

Interface::Interface(Camera& cam)
    : m_cam(cam)
    , m_det_info(cam)
    , m_sync(cam)
    , m_bin(cam)
    , m_roi(cam)
    , m_video(cam)
{
    DEB_CONSTRUCTOR();

    HwDetInfoCtrlObj *det_info = &m_det_info;
    m_cap_list.push_back(HwCap(det_info));
    
    HwSyncCtrlObj *sync = &m_sync;
    m_cap_list.push_back(HwCap(sync));
    
    HwRoiCtrlObj *roi = &m_roi;
    m_cap_list.push_back(HwCap(roi));

    HwVideoCtrlObj *video = &m_video;
    m_cap_list.push_back(HwCap(video));

    HwBufferCtrlObj *buffer = &(m_video.getHwBufferCtrlObj());
    m_cap_list.push_back(HwCap(buffer));

    HwBinCtrlObj *bin = &m_bin;
    m_cap_list.push_back(HwCap(bin));
}


//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::getCapList(HwInterface::CapList &cap_list) const
{
    DEB_MEMBER_FUNCT();
    cap_list = m_cap_list;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::reset(ResetLevel reset_level)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(reset_level);
    m_cam.reset();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::prepareAcq()
{
    DEB_MEMBER_FUNCT();
    m_cam.prepareAcq();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::startAcq()
{
    DEB_MEMBER_FUNCT();
    m_cam.startAcq();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::stopAcq()
{
    DEB_MEMBER_FUNCT();
    m_cam.stopAcq();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::getStatus(StatusType& status)
{
    DEB_MEMBER_FUNCT();
    Camera::Status pg_status = Camera::Ready;
    m_cam.getStatus(pg_status);
    switch (pg_status)
    {
    case Camera::Ready:
        status.det = DetIdle;
	status.acq = AcqReady;
	break;
    case Camera::Exposure:
        status.det = DetExposure;
	status.acq = AcqRunning;
	break;
    case Camera::Readout:
        status.det = DetReadout;
	status.acq = AcqRunning;
	break;
    case Camera::Latency:
        status.det = DetLatency;
	status.acq = AcqRunning;
	break;
    case Camera::Fault:
        status.det = DetFault;
	status.acq = AcqFault;
    }
    status.det_mask = DetExposure | DetReadout | DetLatency;
    DEB_RETURN() << DEB_VAR1(status);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
int Interface::getNbHwAcquiredFrames()
{
    DEB_MEMBER_FUNCT();
    int acq_frames;
    m_cam.getNbHwAcquiredFrames(acq_frames);
    return acq_frames;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::setAutoGain(bool auto_gain) { m_cam.setAutoGain(auto_gain); }
void Interface::getAutoGain(bool& auto_gain) const { m_cam.getAutoGain(auto_gain); }

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::setAutoExpTime(bool auto_exp_time) { m_cam.setAutoExpTime(auto_exp_time); }
void Interface::getAutoExpTime(bool& auto_exp_time) const { m_cam.getAutoExpTime(auto_exp_time); }
