/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    modules.cpp

Abstract:

    List of all test modules

Environment:

    Xbox

Revision History:

--*/


//
// Tell the linker to include the following symbols so that "export table"
// of each test module will be placed and sorted in "export directory" inside
// the test harness
//

#pragma comment( linker, "/include:_address_ExportTableDirectory" )
#pragma comment( linker, "/include:_alphabld_ExportTableDirectory" )
#pragma comment( linker, "/include:_alphacmp_ExportTableDirectory" )
#pragma comment( linker, "/include:_basicrst_ExportTableDirectory" )
#pragma comment( linker, "/include:_blend_ExportTableDirectory" )
#pragma comment( linker, "/include:_bump_ExportTableDirectory" )
#pragma comment( linker, "/include:_clear_test_ExportTableDirectory" )
#pragma comment( linker, "/include:_compress_ExportTableDirectory" )
#pragma comment( linker, "/include:_cubemap_ExportTableDirectory" )
#pragma comment( linker, "/include:_emboss_ExportTableDirectory" )
#pragma comment( linker, "/include:_fog_ExportTableDirectory" )
#pragma comment( linker, "/include:_gradient_ExportTableDirectory" )
#pragma comment( linker, "/include:_lightmap_ExportTableDirectory" )
#pragma comment( linker, "/include:_linetex_ExportTableDirectory" )
#pragma comment( linker, "/include:_luminanc_ExportTableDirectory" )
#pragma comment( linker, "/include:_mapping_ExportTableDirectory" )
#pragma comment( linker, "/include:_mipfilter_ExportTableDirectory" )
#pragma comment( linker, "/include:_overdraw_ExportTableDirectory" )
#pragma comment( linker, "/include:_perspective_ExportTableDirectory" )
#pragma comment( linker, "/include:_pointsprite_ExportTableDirectory" )
#pragma comment( linker, "/include:_pointtex_ExportTableDirectory" )
#pragma comment( linker, "/include:_pshader_ExportTableDirectory" )
#pragma comment( linker, "/include:_srt_ExportTableDirectory" )
#pragma comment( linker, "/include:_stateblocks_ExportTableDirectory" )
#pragma comment( linker, "/include:_stencil_ExportTableDirectory" )
#pragma comment( linker, "/include:_texturestage_ExportTableDirectory" )
#pragma comment( linker, "/include:_tlvertexclip_ExportTableDirectory" )
#pragma comment( linker, "/include:_volume_ExportTableDirectory" )
#pragma comment( linker, "/include:_wbuffer_ExportTableDirectory" )
#pragma comment( linker, "/include:_wcmp_ExportTableDirectory" )
#pragma comment( linker, "/include:_zbuffer_ExportTableDirectory" )
#pragma comment( linker, "/include:_zcmp_ExportTableDirectory" )

#pragma comment( linker, "/include:_alphabnt_ExportTableDirectory" )
#pragma comment( linker, "/include:_ball_ExportTableDirectory" )
#pragma comment( linker, "/include:_basic_ExportTableDirectory" )
#pragma comment( linker, "/include:_bees_ExportTableDirectory" )
#pragma comment( linker, "/include:_bumpduv1_ExportTableDirectory" )
#pragma comment( linker, "/include:_clipping_ExportTableDirectory" )
#pragma comment( linker, "/include:_depthbuf_ExportTableDirectory" )
#pragma comment( linker, "/include:_dlight_ExportTableDirectory" )
#pragma comment( linker, "/include:_dxtnoise_ExportTableDirectory" )
#pragma comment( linker, "/include:_fsaa_ExportTableDirectory" )
#pragma comment( linker, "/include:_fvertex1_ExportTableDirectory" )
#pragma comment( linker, "/include:_gammarmp_ExportTableDirectory" )
#pragma comment( linker, "/include:_getrstat_ExportTableDirectory" )
#pragma comment( linker, "/include:_hightide_ExportTableDirectory" )
#pragma comment( linker, "/include:_ibuffer_ExportTableDirectory" )
#pragma comment( linker, "/include:_innrloop_ExportTableDirectory" )
#pragma comment( linker, "/include:_lighting_ExportTableDirectory" )
#pragma comment( linker, "/include:_lightball_ExportTableDirectory" )
#pragma comment( linker, "/include:_litetype_ExportTableDirectory" )
#pragma comment( linker, "/include:_lockdepth_ExportTableDirectory" )
#pragma comment( linker, "/include:_matsrc_ExportTableDirectory" )
#pragma comment( linker, "/include:_mstage1_ExportTableDirectory" )
#pragma comment( linker, "/include:_mstage2_ExportTableDirectory" )
#pragma comment( linker, "/include:_mstage4_ExportTableDirectory" )
#pragma comment( linker, "/include:_mstage5_ExportTableDirectory" )
#pragma comment( linker, "/include:_multilight_ExportTableDirectory" )
#pragma comment( linker, "/include:_mxpalskin_ExportTableDirectory" )
#pragma comment( linker, "/include:_obelisk_ExportTableDirectory" )
#pragma comment( linker, "/include:_palette0_ExportTableDirectory" )
#pragma comment( linker, "/include:_pslights_ExportTableDirectory" )
#pragma comment( linker, "/include:_psverify_ExportTableDirectory" )
#pragma comment( linker, "/include:_reflect0_ExportTableDirectory" )
#pragma comment( linker, "/include:_stencil0_ExportTableDirectory" )
#pragma comment( linker, "/include:_terrain_ExportTableDirectory" )
#pragma comment( linker, "/include:_texform_ExportTableDirectory" )
#pragma comment( linker, "/include:_texgen_ExportTableDirectory" )
#pragma comment( linker, "/include:_texgenxt_ExportTableDirectory" )
#pragma comment( linker, "/include:_textr1_ExportTableDirectory" )
#pragma comment( linker, "/include:_textr2_ExportTableDirectory" )
#pragma comment( linker, "/include:_textr3_ExportTableDirectory" )
#pragma comment( linker, "/include:_textr4_ExportTableDirectory" )
#pragma comment( linker, "/include:_textr5_ExportTableDirectory" )
#pragma comment( linker, "/include:_tunnel_ExportTableDirectory" )
#pragma comment( linker, "/include:_vbbatch_ExportTableDirectory" )
#pragma comment( linker, "/include:_vbib_ExportTableDirectory" )
#pragma comment( linker, "/include:_vpshadr1_ExportTableDirectory" )
#pragma comment( linker, "/include:_vshader_ExportTableDirectory" )
#pragma comment( linker, "/include:_wireframe_ExportTableDirectory" )

#pragma comment( linker, "/include:_chessbrd_ExportTableDirectory" )
#pragma comment( linker, "/include:_clockwrk_ExportTableDirectory" )
#pragma comment( linker, "/include:_rteapot_ExportTableDirectory" )
#pragma comment( linker, "/include:_skulls_ExportTableDirectory" )
#pragma comment( linker, "/include:_snowfall_ExportTableDirectory" )
#pragma comment( linker, "/include:_misorbit_ExportTableDirectory" )

#pragma comment( linker, "/include:_Ani_ExportTableDirectory" )
