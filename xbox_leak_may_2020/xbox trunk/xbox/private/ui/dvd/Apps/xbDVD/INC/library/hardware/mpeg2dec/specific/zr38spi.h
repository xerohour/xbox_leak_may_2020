// FILE:			library\hardware\mpeg3dec\specific\zr38spi.h
// AUTHOR:		Viona
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		05.12.95
//
// PURPOSE:		
//
// HISTORY:
//
// ZR38SPI.H
//

#ifndef ZR38SPI_H
#define ZR38SPI_H

#include "..\..\..\common\prelude.h"
#include "..\..\..\lowlevel\hardwrio.h"
#include "..\..\..\lowlevel\timer.h"
#include "library\general\dynarray.h"      
      
#define GNR_ZR38SPI_INVALID_OPCODE					MKERR(ERROR, AC3, UNIMPLEMENTED, 0x0101)
#define GNR_ZR38SPI_INVALID_PARAMETERS 			MKERR(ERROR, AC3, PARAMS,			0x0102)
#define GNR_ZR38SPI_NOT_READY							MKERR(ERROR, AC3, BUSY,				0x0103)
#define GNR_ZR38SPI_WRONG_NUMBER_OF_ARGUMENTS   MKERR(ERROR, AC3, PARAMS,			0x0104)

//
// Generic SPI port
//
class ZR38SPIPort
	{
	private:
		BitInputPort	*	si;
		BitOutputPort	*	ss;
		BitOutputPort	*	sck;
		BitOutputPort	*	so;		
		int					delay;
		
		//
		// Primitive serial bus operations
		//
		void Delay(void);
		Error Start(void);
		Error Stop(void);
		Error SendByte(BYTE data, BYTE __far&result);

	protected:
		//
		// Basic communication function
		//
		Error SendCommandA(BYTE command, int numResult, void __far * result, int xparams, DWORD __far *xparam, int __far *param);
	public:
		ZR38SPIPort(BitInputPort * si, BitOutputPort * ss, BitOutputPort * sck, BitOutputPort * so, int delay);
		
		//
		// Derivatives of the basic communication function, to make live
		// a little easier
		//
		Error SendCommand(BYTE command, int numResult, void __far * result, int xparams, DWORD __far *xparam, int param, ...)
			{return SendCommandA(command, numResult, result, xparams, xparam, &param);}

		Error SendCommand(BYTE command, int numResult, void __far * result, int param, ...)
			{return SendCommandA(command, numResult, result, 0, NULL, &param);}
		Error SendCommand(BYTE command) 
			{return SendCommandA(command, 0, NULL, 0, NULL, NULL);}
		Error SendCommand(BYTE command, int param, ...)
			{return SendCommandA(command, 0, NULL, 0, NULL, &param);}
		Error SendCommand(BYTE command, int numResult, void __far * result)
			{return SendCommandA(command, numResult, result, 0, NULL, NULL);}
		
		//
		// Send a file over the SPI bus
		//	
		Error SendFile(const char * fname);
		Error SendArray(DynamicByteArray array);
	};

//
// Specific SPI port for Zoran ZR38xxx chips, shall be separated from
// the generic SPI port in future.
//
class ZR385XXSPIComPort : public ZR38SPIPort
	{
	public:
		//
		// Parameter structure for the different ZR38xxx commands.
		//
	   class __far CFGParams
	   	{
	   	public:
	   		BOOL	wfa, wfb, aw, bw;
	   		BYTE	wait;
	   		BOOL	db, da, cb, mb, ma, tb, ta;
	   		BOOL	sb, sa;
	   		BYTE	frb, fra;
	   		BYTE	sr;
	   		BOOL	mpe, err, dreq, isp, osp;
	   		BOOL	ays, spo;
	   		BYTE	inw, outw, raw;
	   		BYTE	spas;
	   		BYTE	spbs;
	   		BYTE	sppr;
	   		
	   		CFGParams(	BOOL	wfa, BOOL wfb, BOOL aw, BOOL bw,
								BYTE	wait,
								BOOL	db, BOOL da, BOOL cb, BOOL mb, BOOL ma, BOOL tb, BOOL ta,
								BOOL	sb, BOOL sa,
								BYTE	frb, BYTE fra,
								BYTE	sr,
								BOOL	mpe, BOOL err, BOOL dreq, BOOL isp, BOOL osp,
								BOOL	ays, BOOL spo,
								BYTE	inw, BYTE outw, BYTE raw,
								BYTE	spas,
								BYTE	spbs,
								BYTE	sppr);
				Error Send(ZR38SPIPort * port);
	   	};

	   class __far PNGParams
	   	{
	   	public:
	   		BOOL	ns, l, c, r, ls, rs, sw;
	   		WORD	pcmsf;
	   	
	   		PNGParams(BOOL ns, BOOL l, BOOL c, BOOL r, BOOL ls, BOOL rs, BOOL sw,
	   		          WORD pcmsf);
				Error Send(ZR38SPIPort * port);
	   	};

		class __far PCMParams
			{
			public:
				BOOL	de;
				BYTE	dmm;
				WORD	pcmsf;
				
				PCMParams(BOOL de, BYTE dmm, WORD pcmsf);
				Error Send(ZR38SPIPort * port);
			};
			
		class __far PCMXParams
			{ 
			public:
				BOOL	de, ns;
				BYTE	ocfg;
				WORD	pcmsf;
				BYTE	errcnt;
				BYTE	drc;
				
				PCMXParams(BOOL de, BOOL ns, BYTE ocfg, WORD pcmsf, BYTE errcnt, BYTE drc);
				Error Send(ZR38SPIPort * port);
			};
			
		class __far AC3Params
			{
			public:
				BYTE	prlg;
				BOOL	sif;
				BYTE	comp;
				BYTE	dmm;
				BOOL	sf, ab;
				BYTE	pbcfg;
				BOOL	sw;
				BYTE	ocfg;
				BYTE	cdly;
				BYTE	srdly;
				WORD	dynrng;
				BYTE	dsn;
				BOOL	kar;
				BYTE	rpc;
				WORD	pcmsf;
				
				AC3Params(BYTE prlg, BOOL sif, BYTE comp, BYTE dmm, BOOL sf, BOOL ab,
							 BYTE pbcfg, BOOL sw, BYTE ocfg, BYTE cdly, BYTE srdly,
							 WORD dynrng, BYTE dsn, BOOL kar, BYTE rpc, WORD pcmsf);
				Error Send(ZR38SPIPort * port);
			};
			
		class __far PROLParams
			{ 
			public:
				BOOL	sf;
				BOOL	ab;
				BYTE	bcfg;
				BOOL	sw;
				BYTE	ocfg;
				BYTE	cdly;
				BYTE	srdly;
				WORD	pcmsf;
				
				PROLParams(BOOL sf, BOOL ab, BYTE bcfg, BOOL sw, BYTE ocfg, BYTE cdly, BYTE srdly, WORD pcmsf);
				Error Send(ZR38SPIPort * port);
			};		
				
		class __far MPEG1Params
			{ 
			public:
				BYTE	dmm;
				WORD	pcmsf;
				
				MPEG1Params(BYTE dmm, WORD pcmsf);
				Error Send(ZR38SPIPort * port);
			};
			
		class __far AC3Status
			{
			public:
				BYTE	status;
				BYTE	rst;
				BYTE	bufst;
				BYTE	ptst;
				WORD	dift;

				BYTE	ac3dst;
				BYTE	ac3ist;
				WORD	bitstreamConfig;
				WORD	miscInfo;
				WORD	dialogNorm;
				WORD	languageCode;
				WORD	audioProduction;        
				
				Error Receive(ZR38SPIPort * port);
			};
			
		class __far PCMStatus
			{
			public:
				BYTE	status;
				BYTE	rst;
				BYTE	bufst;
				BYTE	ptst;
				WORD	dift;

				BOOL	e;
				BOOL	m;
				BYTE	afn;
				BYTE	qwl;
				BYTE	sr;
				BYTE	nac;

				Error Receive(ZR38SPIPort * port);
			};           
			
		class __far MPEG1Status
			{
			public:
				BYTE	status;
				BYTE	rst;
				BYTE	bufst;
				BYTE	ptst;
				WORD	dift;

				BYTE	mpgst;
				BOOL	id;
				BYTE	lay;
				BOOL	prt;
				BYTE	br;
				BYTE	sfr;
				BOOL	pad;
				BOOL	prv;
				BYTE	mode;
				BYTE	mext;
				BOOL	cpr;
				BOOL	org;
				BYTE	emph;

				Error Receive(ZR38SPIPort * port);
			};
		
		class __far USERParams
			{
			public:
				BYTE	data[8];
			};
			
		ZR385XXSPIComPort(BitInputPort * si, BitOutputPort * ss, BitOutputPort * sck, BitOutputPort * so, int delay);
	
		//
		// ZR38xxx commands, represented as functions
		//
		
		//
		// Main mode commands
		//
		Error SetCFG(CFGParams __far& params);
		Error SetPNG(PNGParams __far& params);
		Error SetPCM(PCMParams __far& params);
		Error SetPCMX(PCMXParams __far& params);
		Error SetAC3(AC3Params __far& params);
		Error SetSCR(DWORD scr, WORD vd);
		Error SetPROL(PROLParams __far& params);
		Error SetMPEG(MPEG1Params __far& params);

		Error ComUSER(USERParams __far& params);
	
		//
		// Decoding control commands
		//
		Error ComNOP(void);
		Error ComPLAY(void);
		Error ComMUTE(void);
		Error ComUNMUTE(void);
		Error ComSTOP(void);
		Error ComSTOPF(void);
		
		//
		// Status commands
		//
		Error StatAC3(AC3Status __far& status);
		Error StatPCM(PCMStatus __far& status);
		Error StatMPEG1(MPEG1Status __far& status);
		Error StatSTCR(DWORD __far& stcr);
		Error StatVER(DWORD &version);
		
		//
		// Extended commands
		//
		Error ComBOOT(BYTE __far& data, int size);
		Error SetIO(BYTE gpioc, BYTE gpio, BYTE __far& gipor);
		Error ComPOKE(DWORD addr, int size, DWORD __far& data);
		Error ComPEEK(DWORD addr, int size, DWORD __far& data);
		
	};

////////////////////////// Inline implementation /////////////////////////////////////

inline ZR385XXSPIComPort::CFGParams::CFGParams(	BOOL	wfa, BOOL wfb, BOOL aw, BOOL bw,
																BYTE	wait,
																BOOL	db, BOOL da, BOOL cb, BOOL mb, BOOL ma, BOOL tb, BOOL ta,
																BOOL	sb, BOOL sa,
																BYTE	frb, BYTE fra,
																BYTE	sr,
																BOOL	mpe, BOOL err, BOOL dreq, BOOL isp, BOOL osp,
																BOOL	ays, BOOL spo,
																BYTE	inw, BYTE outw, BYTE raw,
																BYTE	spas,
																BYTE	spbs,
																BYTE	sppr)
	{
	this->wfa = wfa;
	this->wfb = wfb;
	this->aw = aw;
	this->bw = bw;
	this->wait = wait;
	this->db = db;
	this->da = da;
	this->cb = cb;
	this->mb = mb;
	this->ma = ma;
	this->tb = tb;
	this->ta = ta;
	this->sb = sb;
	this->sa = sa;
	this->frb = frb;
	this->fra = fra;
	this->sr = sr;
	this->mpe = mpe;
	this->err = err;
	this->dreq = dreq;
	this->isp = isp;
	this->osp = osp;
	this->ays = ays;
	this->spo = spo;
	this->inw = inw;
	this->outw = outw;
	this->raw = raw;
	this->spas = spas;
	this->spbs = spbs;
	this->sppr = sppr;
	}

inline ZR385XXSPIComPort::PNGParams::PNGParams(BOOL ns, BOOL l, BOOL c, BOOL r, BOOL ls, BOOL rs, BOOL sw, WORD pcmsf)
	{
	this->ns = ns;
	this->l = l;
	this->c = c;
	this->r = r;
	this->ls = ls;
	this->rs = rs;
	this->sw = sw;       
	this->pcmsf = pcmsf;
	}

inline ZR385XXSPIComPort::PCMParams::PCMParams(BOOL de, BYTE dmm, WORD pcmsf)
	{
	this->de = de;
	this->dmm = dmm;
	this->pcmsf = pcmsf;
	}

inline ZR385XXSPIComPort::PCMXParams::PCMXParams(BOOL de, BOOL ns, BYTE ocfg, WORD pcmsf, BYTE errcnt, BYTE drc)
	{
	this->de = de;
	this->ns = ns;
	this->ocfg = ocfg;
	this->pcmsf = pcmsf;
	this->errcnt = errcnt;
	this->drc = drc;
	}
	
inline ZR385XXSPIComPort::AC3Params::AC3Params(BYTE prlg, BOOL sif, BYTE comp, BYTE dmm, BOOL sf, BOOL ab,
			 											     BYTE pbcfg, BOOL sw, BYTE ocfg, BYTE cdly, BYTE srdly,
			 											     WORD dynrng, BYTE dsn, BOOL kar, BYTE rpc, WORD pcmsf)
	{
	this->prlg = prlg;
	this->sif = sif;
	this->comp = comp;
	this->dmm = dmm;
	this->sf = sf;
	this->ab = ab;
	this->pbcfg = pbcfg;
	this->sw = sw;
	this->ocfg = ocfg;
	this->cdly = cdly;
	this->srdly = srdly;
	this->dynrng = dynrng;
	this->dsn = dsn;
	this->kar = kar;
	this->rpc = rpc;
	this->pcmsf = pcmsf;
	}
	
inline ZR385XXSPIComPort::PROLParams::PROLParams(BOOL sf, BOOL ab, BYTE bcfg, BOOL sw, BYTE ocfg, BYTE cdly, BYTE srdly, WORD pcmsf)
	{
	this->sf = sf;
	this->ab = ab;
	this->bcfg = bcfg;
	this->sw = sw;
	this->ocfg = ocfg;
	this->cdly = cdly;
	this->srdly = srdly;
	this->pcmsf = pcmsf;
	}
	
inline ZR385XXSPIComPort::MPEG1Params::MPEG1Params(BYTE dmm, WORD pcmsf)
	{
	this->dmm = dmm;
	this->pcmsf = pcmsf;
	}
	
#endif

