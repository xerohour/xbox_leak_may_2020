#ifndef __OGLSTEREO_H
#define __OGLSTEREO_H

/*
**
** MODULE: oglstereo.h
**
** OS dependent functions releated to oglstereo reside here
**
** Copyright (C) 2000 NVidia Corporation. All Rights Reserved.
**
*/

BOOL  bOglStereoGlassesSwitchOff(PDEV* ppdev);
BOOL  bOglStereoGlassesSwitchOn(PDEV* ppdev);
BOOL  bOglIsStereoClient(struct _NV_OPENGL_CLIENT_INFO *pClientInfo);
ULONG cOglStereoClients(PPDEV ppdev);
BOOL  bOglIsStereoModeEnabled(PPDEV ppdev);
BOOL  bOglStereoModeUpdate(PPDEV ppdev);
BOOL  bOglStereoModeEnable(PPDEV ppdev);
BOOL  bOglStereoModeDisable(PPDEV ppdev);
BOOL  bOglGetNextActiveStereoClient(struct _NV_OPENGL_CLIENT_INFO **ppClientInfo);
BOOL  bOglGetNextStereoClient(struct _NV_OPENGL_CLIENT_INFO **ppClientInfo);


#endif //#ifndef __OGLSTEREO_H