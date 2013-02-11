namespace PointGrey
{
  class Interface : HwInterface
  {
%TypeHeaderCode
#include <PointGreyInterface.h>
%End
  public:
    Interface(PointGrey::Camera& cam);
    virtual ~Interface();

    //- from HwInterface
    virtual void	getCapList(std::vector<HwCap> &cap_list /Out/) const;
    virtual void	reset(ResetLevel reset_level);
    virtual void 	prepareAcq();
    virtual void 	startAcq();
    virtual void 	stopAcq();
    virtual void 	getStatus(StatusType& status /Out/);
    virtual int 	getNbHwAcquiredFrames();    
    
    //- camera specific
    void    getAutoExpTime(bool& auto_frame_rate /Out/) const;
    void    setAutoExpTime(bool  auto_exp_time);

    void    getAutoGain(bool& auto_gain /Out/) const;    
    void    setAutoGain(bool  auto_gain);
  };
};
