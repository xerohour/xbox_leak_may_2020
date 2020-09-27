#ifndef _ARB_FIFO_H

#define ARB_FIFO_H

#define DEFAULT_GR_LWM 100
#define DEFAULT_VID_LWM 100
#define DEFAULT_GR_BURST_SIZE 256
#define DEFAULT_VID_BURST_SIZE 128


typedef struct {
  int gdrain_rate;
  int vdrain_rate;
  int mdrain_rate;

  int gburst_size;
  int vburst_size;
  char vid_en;
  char gr_en;
  int wcmocc, wcgocc, wcvocc, wcvlwm, wcglwm;
  int by_gfacc;
  char vid_only_once;
  char gr_only_once;
  char first_vacc;
  char first_gacc;
  char first_macc;
  int vocc;
  int gocc;
  int mocc;
  char cur;
  char engine_en;
  char converged;
  int priority;
} arb_info;


// the following structures are used by nv3

typedef struct {

  int graphics_lwm;  // GR LWM
  int video_lwm;    // Video LWM

  int graphics_burst_size;  //Burst size
  int video_burst_size;   // Burst size

  int graphics_hi_priority; // FBI arbitration
  int media_hi_priority;

  int rtl_values;
  int valid;  // good values


} fifo_info;
typedef struct {

  int pix_bpp;          /* 8, 16 or 32 */

  char enable_video; /* Video scalar enabled */
  char gr_during_vid; /* Graphic displays during video */
  char enable_mp; /* Media Port enabled */

  int memory_width;  /* 64 or 128 bits */

  int video_scale; /* video scale factor */

  long pclk_khz; /* Pixel clock, in KHz */
  long mclk_khz; /* Memory clock, in KHz */

  int mem_page_miss; /* Memory page miss penalty */
  int mem_latency;  /* Memory latency */

  char mem_aligned; /* FB is aligned on a 256 byte boundry */
} sim_state;


// the following structures are used by nv4

typedef struct {

  int graphics_lwm;  // GR LWM
  int video_lwm;    // Video LWM

  int graphics_burst_size;
  int video_burst_size;

  int valid;  // good values

} nv4_fifo_info;

typedef struct {
  int pclk_khz;
  int mclk_khz;
  int nvclk_khz;
  char mem_page_miss;
  char mem_latency; //cas_latency
  int memory_width;
  char enable_video;
  char gr_during_vid;
  char pix_bpp;
  char mem_aligned;
  char enable_mp;
} nv4_sim_state;




#define VIDEO       0
#define GRAPHICS    1
#define MPORT       2
#define ENGINE      3


#define GFIFO_SIZE  320     /* Graphics Fifo Sizes */
#define GFIFO_SIZE_128  256     /* Graphics Fifo Sizes */
#define MFIFO_SIZE  120 /* MP  Fifo Sizes */
#define VFIFO_SIZE  256 /* Video Fifo Sizes */

#define ABS(a)  (a>0?a:-a)


#endif

