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
#ifndef POINTGREYCAMERA_H
#define POINTGREYCAMERA_H

#include <stdlib.h>
#include <limits>
#include "HwMaxImageSizeCallback.h"

#include "FlyCapture2.h"
using namespace std;

#ifdef USE_GIGE
typedef FlyCapture2::GigECamera Camera_t;
typedef FlyCapture2::GigEImageSettings ImageSettings_t;
typedef FlyCapture2::GigEImageSettingsInfo ImageSettingsInfo_t;
#else
typedef FlyCapture2::Camera Camera_t;
typedef FlyCapture2::Format7ImageSettings ImageSettings_t;
typedef FlyCapture2::Format7Info ImageSettingsInfo_t;
#endif

namespace lima
{
namespace PointGrey
{
/*******************************************************************
 * \class Camera
 * \brief object controlling the Point Grey camera via Pylon driver
 *******************************************************************/
class VideoCtrlObj;
class Camera : public HwMaxImageSizeCallbackGen
{
    friend class Interface;

    DEB_CLASS_NAMESPC(DebModCamera, "Camera", "PointGrey");
    friend class VideoCtrlObj;
 public:

    enum Status {
      Ready, Exposure, Readout, Latency, Fault
    };

    Camera(const int camera_serial,
           const int packet_size = -1,
           const int packet_delay = -1);
    ~Camera();

    // hw interface
    void prepareAcq();
    void startAcq();
    void stopAcq();

    void getStatus(Camera::Status& status);
    
    // detector info object
    void getDetectorType(std::string& type);
    void getDetectorModel(std::string& model);
    void getDetectorImageSize(Size& size);

    // synch control object
    void getTrigMode(TrigMode& mode);
    void setTrigMode(TrigMode  mode);

    void getExpTime(double& exp_time);
    void setExpTime(double  exp_time);
    void getExpTimeRange(double& min_exp_time, double& max_exp_time) const;

    void getLatTime(double& lat_time);
    void setLatTime(double  lat_time);
    void getLatTimeRange(double& min_lat_time, double& max_lat_time) const;

    void getNbFrames(int& nb_frames);
    void setNbFrames(int  nb_frames);
    void getNbHwAcquiredFrames(int &nb_acq_frames);

    // roi control object
    void checkRoi(const Roi& set_roi, Roi& hw_roi);
    void getRoi(Roi& hw_roi);
    void setRoi(const Roi& set_roi);

    // bin control object
    void checkBin(Bin&);
    void getBin(Bin& bin);
    void setBin(const Bin& bin);

    // video control object
    void getVideoMode(VideoMode& mode) const;
    void setVideoMode(VideoMode  mode);

    void getGain(double& gain);
    void setGain(double  gain);

    // camera specific
    void getPacketSize(int& packet_size);
    void setPacketSize(int  packet_size);

    void getPacketDelay(int& packet_delay);
    void setPacketDelay(int  packet_delay);

    void getAutoExpTime(bool& auto_frame_rate) const;
    void setAutoExpTime(bool  auto_exp_time);

    void getAutoGain(bool& auto_gain) const;
    void setAutoGain(bool  auto_gain);

 protected:
    void _getPropertyInfo(FlyCapture2::PropertyInfo *property_info);
    void _getProperty(FlyCapture2::Property *property);
    void _setProperty(FlyCapture2::Property *property);

    void _getImageSettingsInfo();
    void _applyImageSettings();
 private:
    class _AcqThread;
    friend class _AcqThread;

    void _setStatus(Camera::Status status,bool force);
    void _stopAcq(bool internalFlag);
    void _forcePGRY16Mode();

    VideoCtrlObj             *m_video;

    Camera::Status            m_status;
    int                       m_nb_frames;
    int                       m_image_number;

    _AcqThread               *m_acq_thread;
    Cond                      m_cond;
    volatile bool             m_quit;
    volatile bool             m_acq_started;
    volatile bool             m_thread_running;

    Camera_t                 *m_camera;
    FlyCapture2::CameraInfo   m_camera_info;
    FlyCapture2::Error 		  m_error;

    ImageSettingsInfo_t       m_image_settings_info;
    ImageSettings_t           m_image_settings;

    FlyCapture2::Property     m_frame_rate_property,
                              m_exp_time_property,
                              m_gain_property;
    
    FlyCapture2::PropertyInfo m_frame_rate_property_info,
                              m_exp_time_property_info,
                              m_gain_property_info;
};
} // namespace PointGrey
} // namespace lima


#endif // POINTGREYCAMERA_H
