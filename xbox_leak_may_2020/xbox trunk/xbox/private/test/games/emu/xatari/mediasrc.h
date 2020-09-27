#ifndef MEDIASOURCE_HXX
#define MEDIASOURCE_HXX

class MediaSource;

#include "bspf.h"

/**
  This class provides an interface for accessing graphics data.

  @author  Bradford W. Mott
  @version $Id: MediaSrc.hxx,v 1.2 1998/07/15 20:51:20 bwmott Exp $
*/
class MediaSource
{
  public:
    /**
      Create a new media source
    */
    MediaSource();
 
    /**
      Destructor
    */
    virtual ~MediaSource();

  public:
    /**
      This method should be called at an interval corresponding to
      the desired frame rate to update the media source.  
    */
    virtual void update() = 0;

    /**
      Answers the current frame buffer

      @return Pointer to the current frame buffer
    */
    virtual uInt8* currentFrameBuffer() const = 0;

    /**
      Answers the previous frame buffer

      @return Pointer to the previous frame buffer
    */
    virtual uInt8* previousFrameBuffer() const = 0;

  public:
    /**
      Get the palette which maps frame data to RGB values.

      @return Array of integers which represent the palette (RGB)
    */
    virtual const uInt32* palette() const = 0;

    /**
      Answers the height of the frame buffer

      @return The frame's height
    */
    virtual uInt32 height() const = 0;

    /**
      Answers the width of the frame buffer

      @return The frame's width
    */
    virtual uInt32 width() const = 0;

  private:
    // Copy constructor isn't supported by this class so make it private
    MediaSource(const MediaSource&);

    // Assignment operator isn't supported by this class so make it private
    MediaSource& operator = (const MediaSource&);
};
#endif

