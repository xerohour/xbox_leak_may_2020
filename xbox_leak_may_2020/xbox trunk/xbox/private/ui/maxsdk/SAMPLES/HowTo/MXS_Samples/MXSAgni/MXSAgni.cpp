/**********************************************************************
 *<
	FILE: MXSAgni.cpp

	DESCRIPTION: All the extension functions in MXSAgni.dlx

	CREATED BY: Ravi Karra, 1998

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/


#include "MXSAgni.h"
#include "LclClass.h"
#include "ExtClass.h"
#include "commdlg.h"
#include "Bmmlib.h"
#include "BitMaps.h"

#include "defextfn.h"
#	include "ExtKeys.h"

#include "definsfn.h"
#	include "ExtFuncs.h"

// calling functions for MXS functions declared in extfuncs.h
Value*
get_MAX_version_cf(Value** arg_list, int count)
{
	// maxVersion ()
	check_arg_count(MAXVersion, 0, count);
	return Float::intern(VERSION_3DSMAX / 100.0f);
}
 
Value*
get_root_node()
{
	return MAXNode::intern(MAXScript_interface->GetRootNode());
}

Value*
set_root_node(Value* val)
{
	throw RuntimeError ("rootNode is read-only");
    return &undefined;
}

Value*
set_rend_output_filename(Value* val)
{
	TCHAR *name = val->to_string();
	if (_tcscmp(name, _T("")) == 0)
		MAXScript_interface->SetRendSaveFile(FALSE);
	BitmapInfo& bi = MAXScript_interface->GetRendFileBI();
	bi.SetName(name);
	return val;
}

Value*
get_rend_output_filename()
{
	BitmapInfo& bi = MAXScript_interface->GetRendFileBI();
	return new String((TCHAR*)bi.Name());
}

// this is called by the dlx initializer, register the global vars here
void MXSAgni_init()
{
	CharStream* out = thread_local(current_stdout);
	out->puts(_T("--* MXSAgni.dlx 3.00 loaded *--\n"));

	// Definitions for globals and struct globals
	define_system_global("rootNode", get_root_node, set_root_node);
	define_system_global("rendOutputFilename", get_rend_output_filename, set_rend_output_filename);
#include "lclimpfn.h"
#	include "ExtKeys.h"
}

Value*
is_group_head_cf(Value** arg_list, int count)
{
	check_arg_count(isGroupHead, 1, count);	
	
	get_valid_node((MAXNode*)arg_list[0], isGroupHead);

	return node->IsGroupHead() ? &true_value : &false_value;
}

Value*
is_group_member_cf(Value** arg_list, int count)
{
	check_arg_count(isGroupMember, 1, count);
	
	get_valid_node((MAXNode*)arg_list[0], isGroupMember);

	return node->IsGroupMember() ? &true_value : &false_value;
}

Value*
is_open_group_member_cf(Value** arg_list, int count)
{
	check_arg_count(isOpenGroupMember, 1, count);
	
	get_valid_node((MAXNode*)arg_list[0], isOpenGroupMember);

	return node->IsOpenGroupMember() ? &true_value : &false_value;	
}

Value*
is_open_group_head_cf(Value** arg_list, int count)
{
	check_arg_count(isOpenGroupHead, 1, count);
	
	get_valid_node((MAXNode*)arg_list[0], isOpenGroupHead);

	return node->IsOpenGroupHead() ? &true_value : &false_value;	
}

Value*
set_group_member_cf(Value** arg_list, int count)
{
	check_arg_count(setGroupMember, 2, count);
	
	get_valid_node((MAXNode*)arg_list[0], setGroupMember);

	BOOL val = arg_list[1]->to_bool();
	
	node->SetGroupMember(val);
	return val ? &true_value : &false_value;
}

Value*
set_group_head_cf(Value** arg_list, int count)
{
	check_arg_count(setGroupHead, 2, count);
	
	get_valid_node((MAXNode*)arg_list[0], setGroupHead);

	BOOL val = arg_list[1]->to_bool();
	
	node->SetGroupMemberOpen(val);
	return val ? &true_value : &false_value;
}

Value*
set_group_head_open_cf(Value** arg_list, int count)
{
	check_arg_count(setGroupHeadOpen, 2, count);

	get_valid_node((MAXNode*)arg_list[0], setGroupHeadOpen);

	BOOL val = arg_list[1]->to_bool();
	
	node->SetGroupHeadOpen(val);
	return val ? &true_value : &false_value;
}

Value*
set_group_member_open_cf(Value** arg_list, int count)
{
	check_arg_count(setGroupMemberOpen, 2, count);

	get_valid_node((MAXNode*)arg_list[0], setGroupMemberOpen);

	BOOL val = arg_list[1]->to_bool();
	
	node->SetGroupMemberOpen(val);
	return val ? &true_value : &false_value;
}

Value*
rescaleWorldUnits_cf(Value** arg_list, int count)
{
	if (count < 1)
		throw ArgCountError (_T("rescaleWorldUnits "), 1, count);
	MAXScript_interface->RescaleWorldUnits(
		arg_list[0]->to_float(), 
		arg_list[count-1] == n_selOnly );
	redraw_views();
	return &ok;
}

Value*
h_marker_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(hMarker, 2, count);
	
	MarkerType	mt;
	int			i = 0;
	Point3		p = arg_list[0]->to_point3();
	IPoint3		d = IPoint3((int)p.x, (int)p.y, (int)p.z);
	Point3		col; 
	Value*		col_val = key_arg(color);
	
	if (col_val == &unsupplied) 
		col = Point3(255, 0, 0);
	else
		col = col_val->to_point3();
	
	while (i < count)
	{
		Value* mtype = arg_list[i];

		if(mtype == n_point)
			mt = POINT_MRKR; 
		else if(mtype == n_hollowBox)
			mt = HOLLOW_BOX_MRKR;
		else if(mtype == n_plusSign)
			mt = PLUS_SIGN_MRKR;
		else if(mtype == n_asterisk)
			mt = ASTERISK_MRKR;
		else if(mtype == n_xMarker)
			mt = X_MRKR;
		else if(mtype == n_bigBox)
			mt = BIG_BOX_MRKR;
		else if(mtype == n_circle)
			mt = CIRCLE_MRKR;
		else if(mtype == n_triangle)
			mt = TRIANGLE_MRKR;
		else if(mtype == n_diamond)
			mt = DIAMOND_MRKR;
		else if(mtype == n_smallHollowBox)
			mt = SM_HOLLOW_BOX_MRKR;
		else if(mtype == n_smallCircle)
			mt = SM_CIRCLE_MRKR;
		else if(mtype == n_smallTriangle)
			mt = SM_TRIANGLE_MRKR;
		else 
			mt = SM_DIAMOND_MRKR;
		i++;
	}

	ViewExp* vp = MAXScript_interface->GetActiveViewport();
	GraphicsWindow* gw = vp->getGW();

	gw->setColor(LINE_COLOR, col);
	gw->hMarker(&d, mt);

	gw->resetUpdateRect();
	gw->enlargeUpdateRect(NULL);
	gw->updateScreen();
	
	MAXScript_interface->ReleaseViewport(vp); 
	return &ok;
}


Value*
load_dlls_from_dir_cf(Value** arg_list, int count)
{
	check_arg_count(loadDllsFromDir, 2, count);
	return MAXScript_interface->GetDllDir().LoadDllsFromDir(
					arg_list[0]->to_string(), arg_list[1]->to_string()) ?
		&true_value : &false_value; 
}

Value*
get_INI_setting_cf(Value** arg_list, int count)
{
	TCHAR val[MAX_PATH];
	check_arg_count(getIniSetting, 3, count);
	GetPrivateProfileString(
		arg_list[1]->to_string(), 
		arg_list[2]->to_string(), 
		_T(""),	val, MAX_PATH, 
		arg_list[0]->to_string());
	return new String(val);	
}

Value*
set_INI_setting_cf(Value** arg_list, int count)
{
	check_arg_count(setINISetting, 4, count);
	WritePrivateProfileString(
		arg_list[1]->to_string(), 
		arg_list[2]->to_string(), 
		arg_list[3]->to_string(), 
		arg_list[0]->to_string());
	return &ok;
}

Value*
get_file_version_cf(Value** arg_list, int count)
{
	check_arg_count(getFileVersion, 1, count);
	
	DWORD	tmp; 
	LPTSTR	file = arg_list[0]->to_string();	
	DWORD	size = GetFileVersionInfoSize(file, &tmp);	
	if (!size) return &undefined;
	
	TCHAR*	data = (TCHAR*)malloc(size);	
	if(data && GetFileVersionInfo(file, NULL, size, data))
	{
		UINT len;
		VS_FIXEDFILEINFO *qbuf;
		TCHAR buf[256];
		if (VerQueryValue(data, "\\", (void**)&qbuf, &len))
		{
			DWORD fms = qbuf->dwFileVersionMS;
            DWORD fls = qbuf->dwFileVersionLS;
			DWORD pms = qbuf->dwProductVersionMS;
            DWORD pls = qbuf->dwProductVersionLS;
            	
			free(data);
			sprintf(buf, _T("%i,%i,%i,%i\t\t%i,%i,%i,%i"), 
				HIWORD(pms), LOWORD(pms), HIWORD(pls), LOWORD(pls),
				HIWORD(fms), LOWORD(fms), HIWORD(fls), LOWORD(fls));
			
			return new String(buf);
		}
		free(data);
	}
	return &undefined;
}

Value*
getWinSizeX_cf(Value** arg_list, int count)
{
	check_arg_count(getWinSizeX, 0, count);
	ViewExp *vpt = MAXScript_interface->GetActiveViewport();
	int sizeX = vpt->getGW()->getWinSizeX();
	MAXScript_interface->ReleaseViewport(vpt);
	return Integer::intern(sizeX);

}

Value*
getWinSizeY_cf(Value** arg_list, int count)
{
	check_arg_count(getWinSizeY, 0, count);
	ViewExp *vport = MAXScript_interface->GetActiveViewport();
	int sizeY = vport->getGW()->getWinSizeY();
	MAXScript_interface->ReleaseViewport(vport);
	return Integer::intern(sizeY);
}


Value*
get_viewport_dib_cf(Value** arg_list, int count)
{
	check_arg_count(getViewportDib, 0, count);
	ViewExp *vpt = MAXScript_interface->GetActiveViewport();
	GraphicsWindow *gw = vpt->getGW();
	BITMAPINFO *bmi = NULL;
	BITMAPINFOHEADER *bmih;
	BitmapInfo bi;
	Bitmap *bmp;
	int size;
	gw->getDIB(NULL, &size);
	bmi  = (BITMAPINFO *)malloc(size);
	bmih = (BITMAPINFOHEADER *)bmi;
	gw->getDIB(bmi, &size);
	bi.SetWidth((WORD)bmih->biWidth);
	bi.SetHeight((WORD)bmih->biHeight);
	bi.SetType(BMM_TRUE_32);
	bmp = TheManager->Create(&bi);
	bmp->OpenOutput(&bi);
	bmp->FromDib(bmi);
	bmp->Write(&bi);
	bmp->Close(&bi);
	
	MAXScript_interface->ReleaseViewport(vpt);

	return new MAXBitMap(bi, bmp);
}


//mcr_func_imp



