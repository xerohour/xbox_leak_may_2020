#include <dsutil.h>

HRESULT TestCreateDSound( DWORD dwDeviceId, LPUNKNOWN pUnk )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
#ifdef    SILVER
	test.SetDeviceId( dwDeviceId );
#endif // SILVER
	test.SetControllingUnknown( pUnk );

	CHECKEXECUTE( test.CreateDSound() );

	return hr;
}

HRESULT TestCreateDSound( void ) 
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwDeviceIds ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_pUnks ); j++ )
		{
			DbgPrint( "dwDeviceID: %u  pDSound: NULL  pUnk: 0x%x\n", g_dwDeviceIds[i], g_pUnks[j] );
			EXECUTE( TestCreateDSound( g_dwDeviceIds[i], g_pUnks[j] ) );
		}
	}

	return hr;
}

HRESULT TestCreateBuffer( DWORD dwDeviceId, LPUNKNOWN pUnk, DWORD dwBufferFlag, DWORD dwBufferSize, LPWAVEFORMATEX pWaveFormat )
{
	HRESULT hr = S_OK;

	CDSoundTest test;

	CHECKEXECUTE( test.ReAllocate() );

#ifdef    SILVER
	test.SetDeviceId( dwDeviceId );
#endif // SILVER

	test.SetControllingUnknown( pUnk );
	test.SetBufferFlags( dwBufferFlag );
	test.SetBufferSize( dwBufferSize );
	test.SetWaveFormat( pWaveFormat );

	CHECKEXECUTE( test.CreateBuffer() );

	return hr;
}

HRESULT TestCreateBuffer( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwDeviceIds ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_pUnks ); j++ )
		{
			for ( DWORD k = 0; k < NUMELEMS( g_dwBufferFlags ); k++ )
			{
				for ( DWORD l = 0; l < NUMELEMS( g_dwBufferSizes ); l++ )
				{
					for ( DWORD m = 0; m < NUMELEMS( g_waveFormats ); m++ )
					{
						for ( int x = -1; x <= 1; x++ )
						{
							DbgPrint( "DeviceId: %u  pUnk: 0x%x  Flags: %u  BufferSize: %u  WaveFormat: 0x%x\n", g_dwDeviceIds[i], g_pUnks[j], g_dwBufferFlags[k], g_dwBufferSizes[l], &g_waveFormats[m] ); 
							EXECUTE( TestCreateBuffer( g_dwDeviceIds[i], g_pUnks[j], g_dwBufferFlags[k], g_dwBufferSizes[l] + x, &g_waveFormats[m] ) );
						}
					}
				}
			}
		}
	}

	return hr;
}

HRESULT TestCreateSoundBuffer( DWORD dwDeviceId, LPUNKNOWN pUnk, DWORD dwBufferFlag, DWORD dwBufferSize, LPWAVEFORMATEX pWaveFormat )
{
	HRESULT hr = S_OK;

	CDSoundTest test;

	CHECKEXECUTE( test.CreateDSound() );
	CHECKEXECUTE( test.ReAllocate() );

#ifdef    SILVER
	test.SetDeviceId( dwDeviceId );
#endif // SILVER

	test.SetControllingUnknown( pUnk );
	test.SetBufferFlags( dwBufferFlag );
	test.SetBufferSize( dwBufferSize );
	test.SetWaveFormat( pWaveFormat );

	CHECKEXECUTE( test.CreateBuffer() );

	return hr;
}

HRESULT TestCreateSoundBuffer( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwDeviceIds ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_pUnks ); j++ )
		{
			for ( DWORD k = 0; k < NUMELEMS( g_dwBufferFlags ); k++ )
			{
				for ( DWORD l = 0; l < NUMELEMS( g_dwBufferSizes ); l++ )
				{
					for ( DWORD m = 0; m < NUMELEMS( g_waveFormats ); m++ )
					{
						for ( int x = -1; x <= 1; x++ )
						{
							DbgPrint( "DeviceId: %u  pUnk: 0x%x  Flags: %u  BufferSize: %u  WaveFormat: 0x%x\n", g_dwDeviceIds[i], g_pUnks[j], g_dwBufferFlags[k], g_dwBufferSizes[l], &g_waveFormats[m] ); 
							EXECUTE( TestCreateSoundBuffer( g_dwDeviceIds[i], g_pUnks[j], g_dwBufferFlags[k], g_dwBufferSizes[l] + x, &g_waveFormats[m] ) );
						}
					}
				}
			}
		}
	}
	return hr;
}

HRESULT TestCommitDefferedSettings( void ) 
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.CreateDSound() );
	CHECKEXECUTE( test.CommitDeferredSettings() );

	return hr;
}
/*
HRESULT TestGetCaps( LPDSCAPS pdsCaps )
{
	HRESULT hr = S_OK;

	CDSoundTest test;

	CHECKEXECUTE( test.CreateDSound() );
	CHECKEXECUTE( test.ReAllocate() );

	test.SetDSCaps( pdsCaps );

	CHECKEXECUTE( test.GetCaps() );

	return hr;
}


HRESULT TestGetCaps( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dsCaps ); i++ )
	{
		DbgPrint( "Caps : 0x%x\n", &g_dsCaps[i] );
		EXECUTE( TestGetCaps( &g_dsCaps[i] ) );
	}

	return hr;
}
*/
/*
HRESULT TestSetSpeakerConfig( DWORD dwSpeakerConfig )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	test.SetSpeakerConfig( dwSpeakerConfig );

	CHECKEXECUTE( test.CreateDSound() );
	CHECKEXECUTE( test.SetSpeakerConfig() );

	return hr;
}

HRESULT TestSetSpeakerConfig( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwSpeakerConfigs ); i++ )
	{
		DbgPrint( "Speaker config: %u\n", g_dwSpeakerConfigs[i] );
		EXECUTE( TestSetSpeakerConfig( g_dwSpeakerConfigs[i] ) );
	}

	return hr;
}
*/
HRESULT TestGetSpeakerConfig( void )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.CreateDSound() );
	CHECKEXECUTE( test.GetSpeakerConfig() );

	return hr;
}

HRESULT TestListenerSetAllParameters( LPDS3DLISTENER pListener, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.CreateDSound() );
	CHECKEXECUTE( test.ReAllocate() );

	test.SetListener( pListener );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.ListenerSetAllParameters() );

	return hr;
}
/*
HRESULT TestListenerSetAllParameters( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_ds3dListeners ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwApplies ); j++ )
		{
			DbgPrint( "Listener: 0x%x  Apply: %u\n", &g_ds3dListeners[i], g_dwApplies[j] );
			EXECUTE( TestListenerSetAllParameters( &g_ds3dListeners[i], g_dwApplies[j] ) );
		}
	}

	return hr;
}
*/
HRESULT TestSetDistanceFactor( FLOAT* pFLOATs, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.CreateDSound() );
//	CHECKEXECUTE( test.ReAllocate() );

	test.SetD3DVals( pFLOATs );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.SetDistanceFactor() );

	return hr;
}

HRESULT TestSetDistanceFactor( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_d3dVals ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwApplies ); j++ )
		{
			DbgPrint( "d3dVal: %d Apply: %u\n", g_d3dVals[i], g_dwApplies[j] );
			EXECUTE( TestSetDistanceFactor( &g_d3dVals[i], g_dwApplies[j] ) );
		}
	}

	return hr;
}

HRESULT TestSetDopplerFactor( FLOAT* pFLOATs, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.CreateDSound() );
//	CHECKEXECUTE( test.ReAllocate() );

	test.SetD3DVals( pFLOATs );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.SetDopplerFactor() );

	return hr;
}

HRESULT TestSetDopplerFactor( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_d3dVals ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwApplies ); j++ )
		{
			DbgPrint( "d3dVal: %d Apply: %u\n", g_d3dVals[i], g_dwApplies[j] );
			EXECUTE( TestSetDopplerFactor( &g_d3dVals[i], g_dwApplies[j] ) );
		}
	}

	return hr;
}

HRESULT TestSetRolloffFactor( FLOAT* pFLOATs, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.CreateDSound() );
//	CHECKEXECUTE( test.ReAllocate() );

	test.SetD3DVals( pFLOATs );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.SetRolloffFactor() );

	return hr;
}

HRESULT TestSetRolloffFactor( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_d3dVals ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwApplies ); j++ )
		{
			DbgPrint( "d3dVal: %d Apply: %u\n", g_d3dVals[i], g_dwApplies[j] );
			EXECUTE( TestSetRolloffFactor( &g_d3dVals[i], g_dwApplies[j] ) );
		}
	}

	return hr;
}

HRESULT TestListenerSetPosition( FLOAT* pFLOATs, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.CreateDSound() );
//	CHECKEXECUTE( test.ReAllocate() );

	test.SetD3DVals( pFLOATs );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.ListenerSetPosition() );

	return hr;
}

HRESULT TestListenerSetPosition( void )
{
	HRESULT hr = S_OK;
	FLOAT ad3dVals[3];

	for ( DWORD i = 0; i < NUMELEMS( g_d3dVals ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_d3dVals ); j++ )
		{
			for ( DWORD k = 0; k < NUMELEMS( g_d3dVals ); k++ )
			{
				for ( DWORD l = 0; l < NUMELEMS( g_dwApplies ); l++ )
				{
					ad3dVals[0] = g_d3dVals[i];
					ad3dVals[1] = g_d3dVals[j];
					ad3dVals[2] = g_d3dVals[k];
					DbgPrint( "d3dVal: %d d3dVal: %d d3dVal: %d Apply: %u\n", g_d3dVals[i], g_d3dVals[j], g_d3dVals[k], g_dwApplies[l] );
					EXECUTE( TestListenerSetPosition( &g_d3dVals[i], g_dwApplies[l] ) );
				}
			}
		}
	}

	return hr;
}

HRESULT TestListenerSetVelocity( FLOAT* pFLOATs, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.CreateDSound() );
//	CHECKEXECUTE( test.ReAllocate() );

	test.SetD3DVals( pFLOATs );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.ListenerSetVelocity() );

	return hr;
}

HRESULT TestListenerSetVelocity( void )
{
	HRESULT hr = S_OK;
	FLOAT ad3dVals[3];

	for ( DWORD i = 0; i < NUMELEMS( g_d3dVals ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_d3dVals ); j++ )
		{
			for ( DWORD k = 0; k < NUMELEMS( g_d3dVals ); k++ )
			{
				for ( DWORD l = 0; l < NUMELEMS( g_dwApplies ); l++ )
				{
					ad3dVals[0] = g_d3dVals[i];
					ad3dVals[1] = g_d3dVals[j];
					ad3dVals[2] = g_d3dVals[k];
					DbgPrint( "d3dVal: %d d3dVal: %d d3dVal: %d Apply: %u\n", g_d3dVals[i], g_d3dVals[j], g_d3dVals[k], g_dwApplies[l] );
					EXECUTE( TestListenerSetVelocity( &g_d3dVals[i], g_dwApplies[l] ) );
				}
			}
		}
	}

	return hr;
}

HRESULT TestSetOrientation( FLOAT* pFLOATs, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.CreateDSound() );
//	CHECKEXECUTE( test.ReAllocate() );

	test.SetD3DVals( pFLOATs );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.SetOrientation() );

	return hr;
}

HRESULT TestSetOrientation( void )
{
	HRESULT hr = S_OK;
	FLOAT ad3dVals[6];

	for ( DWORD i = 0; i < NUMELEMS( g_d3dVals ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_d3dVals ); j++ )
		{
			for ( DWORD k = 0; k < NUMELEMS( g_d3dVals ); k++ )
			{
				for ( DWORD l = 0; l < NUMELEMS( g_d3dVals ); l++ )
				{
					for ( DWORD m = 0; m < NUMELEMS( g_d3dVals ); m++ )
					{
						for ( DWORD n = 0; n < NUMELEMS( g_d3dVals ); n++ )
						{
							for ( DWORD o = 0; o < NUMELEMS( g_dwApplies ); o++ )
							{
								ad3dVals[0] = g_d3dVals[i];
								ad3dVals[1] = g_d3dVals[j];
								ad3dVals[2] = g_d3dVals[k];
								ad3dVals[3] = g_d3dVals[l];
								ad3dVals[4] = g_d3dVals[m];
								ad3dVals[5] = g_d3dVals[n];
								DbgPrint( "d3dVal: %d d3dVal: %d d3dVal: %d d3dVal: %d d3dVal: %d d3dVal: %d Apply: %u\n", g_d3dVals[i], g_d3dVals[j], g_d3dVals[k], g_d3dVals[l], g_d3dVals[m], g_d3dVals[n], g_dwApplies[o] );
								EXECUTE( TestSetOrientation( &g_d3dVals[i], g_dwApplies[o] ) );
							}
						}
					}
				}
			}
		}
	}
	
	return hr;
}

HRESULT TestStop( void )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	
	CHECKEXECUTE( test.Open( BVT_FILE ) );
	CHECKEXECUTE( test.Stop() );

	return hr;
}

#ifdef SILVER

HRESULT TestSetChannelVolume( LPDSCHANNELVOLUME pdscv )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetChannelVolume( pdscv );
	CHECKEXECUTE( test.SetChannelVolume() );

	return hr;
}
	
HRESULT TestSetChannelVolume( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dsChannelVolumes ); i++ )
	{
		DbgPrint( "channel Vol: 0x%x\n", &g_dsChannelVolumes[i] );
		EXECUTE( TestSetChannelVolume( &g_dsChannelVolumes[i] ) );
	}

	return hr;
}

#endif // SILVER

HRESULT TestSetVolume( LONG lVolume )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetVolume( lVolume );
	CHECKEXECUTE( test.SetVolume() );

	return hr;
}

HRESULT TestSetVolume( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_lVolumes ); i++ )
	{
		DbgPrint( "Volume: %d\n", g_lVolumes[i] );
		EXECUTE( TestSetVolume( g_lVolumes[i] ) );
	}

	return hr;
}

HRESULT TestGetStatus( void )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );
	return hr;
}

HRESULT TestSetFrequency( DWORD dwFreq )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetFrequency( dwFreq );
	CHECKEXECUTE( test.SetFrequency() );

	return hr;
}

HRESULT TestSetFrequency( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwFrequencies ); i++ )
	{
		DbgPrint( "Frequency: %u\n", g_dwFrequencies[i] );
		EXECUTE( TestSetFrequency( g_dwFrequencies[i] ) );
	}

	return hr;
}

HRESULT TestBuffer3DSetAllParameters( LPDS3DBUFFER pBuffer, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetBuffer( pBuffer );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.Buffer3DSetAllParameters() );

	return hr;
}
/*
HRESULT TestBuffer3DSetAllParameters( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_ds3dBuffers ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwApplies ); j++ )
		{
			DbgPrint( "Buffer: 0x%x  Apply: %u\n", &g_ds3dBuffers[i], g_dwApplies[j] );
			EXECUTE( TestBuffer3DSetAllParameters( &g_ds3dBuffers[i], g_dwApplies[j] ) );
		}
	}

	return hr;
}
*/
HRESULT TestSetConeOutsideVolume( LONG lVolume, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetConeOutsideVolume( lVolume );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.SetConeOutsideVolume() );

	return hr;
}

HRESULT TestSetConeOutsideVolume( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_lVolumes ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwApplies ); j++ )
		{
			DbgPrint( "Volume: %d  Apply: %u\n", g_lVolumes[i], g_dwApplies[j] );
			EXECUTE( TestSetConeOutsideVolume( g_lVolumes[i], g_dwApplies[j] ) );
		}
	}

	return hr;
}

HRESULT TestSetMaxDistance( FLOAT* pValues, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetD3DVals( pValues );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.SetMaxDistance() );

	return hr;

}

HRESULT TestSetMaxDistance( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_d3dVals ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwApplies ); j++ )
		{
			DbgPrint( "d3dVal: %d Apply: %u\n", g_d3dVals[i], g_dwApplies[j] );
			EXECUTE( TestSetMaxDistance( &g_d3dVals[i], g_dwApplies[j] ) );
		}
	}

	
	return hr;
}

HRESULT TestSetMinDistance( FLOAT* pValues, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetD3DVals( pValues );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.SetMinDistance() );

	return hr;

}

HRESULT TestSetMinDistance( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_d3dVals ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwApplies ); j++ )
		{
			DbgPrint( "d3dVal: %d Apply: %u\n", g_d3dVals[i], g_dwApplies[j] );
			EXECUTE( TestSetMinDistance( &g_d3dVals[i], g_dwApplies[j] ) );
		}
	}

	
	return hr;
}

HRESULT TestSetMode( DWORD dwMode, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetMode( dwMode );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.SetMode() );

	return hr;
}

HRESULT TestSetMode( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwModes ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwApplies ); j++ )
		{
			DbgPrint( "Mode: %u Apply: %u\n", g_dwModes[i], g_dwApplies[j] );
			EXECUTE( TestSetMode( g_dwModes[i], g_dwApplies[j] ) );
		}
	}

	return hr;
}

HRESULT TestSetNotificationPositions( DWORD dwCount, LPCDSBPOSITIONNOTIFY pNotify )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetNotifyCount( dwCount );
	test.SetNotifies( pNotify );

	CHECKEXECUTE( test.SetNotificationPositions() );

	return hr;
}

HRESULT TestSetNotificationPositions( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwNotifyCounts ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_pNotifies ); j++ )
		{
			DbgPrint( "Count: %u Notify: 0x%x\n", g_dwNotifyCounts[i], &g_pNotifies[j] );
			EXECUTE( TestSetNotificationPositions( g_dwNotifyCounts[i], &g_pNotifies[j] ) );
		}
	}
	
	return hr;
}

HRESULT TestSetCurrentPosition( DWORD dwPlayCursor )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetPlayCursor( dwPlayCursor );

	CHECKEXECUTE( test.SetCurrentPosition() );

	return hr;
}

HRESULT TestSetCurrentPosition( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwPlayCursors ); i++ )
	{
		DbgPrint( "PlayCursor: %u\n", g_dwPlayCursors[i] );
		EXECUTE( TestSetCurrentPosition( g_dwPlayCursors[i] ) );
	}

	return hr;
}

HRESULT TestGetCurrentPosition( void )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );
	CHECKEXECUTE( test.GetCurrentPosition() );

	return hr;
}

HRESULT TestSetBufferData( LPVOID pBuffer, DWORD dwSize )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetBufferBytes( dwSize );
	test.SetBufferData( pBuffer );

	CHECKEXECUTE( test.SetBufferData() );

	return hr;
}

HRESULT TestSetBufferData( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_pvBuffers ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwBufferSizes ); j++ )
		{
			DbgPrint( "Data: 0x%x Bytes: %d\n", g_pvBuffers[i], g_dwBufferSizes[j] );
			EXECUTE( TestSetBufferData( g_pvBuffers[i], g_dwBufferSizes[j] ) );
		}
	}

	return hr;
}

HRESULT TestSetLoopRegion( DWORD dwStart, DWORD dwLength )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetLoopStart( dwStart );
	test.SetLoopLength( dwLength );

	CHECKEXECUTE( test.SetLoopRegion() );

	return hr;
}

HRESULT TestSetLoopRegion( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwBufferSizes ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwBufferSizes ); j++ )
		{
			DbgPrint( "Start: %d Length: %d\n", g_dwBufferSizes[i], g_dwBufferSizes[j] );
			EXECUTE( TestSetLoopRegion( g_dwBufferSizes[i], g_dwBufferSizes[j] ) );
		}
	}

	return hr;
}

HRESULT TestSetConeAngles( DWORD dwInsideAngle, DWORD dwOutsideAngle, DWORD dwApply )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetInsideConeAngle( dwInsideAngle );
	test.SetOutsideConeAngle( dwOutsideAngle );
	test.SetApply( dwApply );

	CHECKEXECUTE( test.SetConeAngles() );

	return hr;
}

HRESULT TestSetConeAngles( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwConeAngles ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwConeAngles ); j++ )
		{
			for ( DWORD k = 0; k < NUMELEMS( g_dwApplies ); k++ )
			{
				DbgPrint( "Inside: %d Outside: %d Apply: %d\n", g_dwConeAngles[i], g_dwConeAngles[j], g_dwApplies[k] );
				EXECUTE( TestSetConeAngles( g_dwConeAngles[i], g_dwConeAngles[j], g_dwApplies[k] ) );
			}
		}
	}

	return hr;
}

HRESULT TestPlay( DWORD dwReserved1, DWORD dwReserved2, DWORD dwPlayFlags )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetReserved1( dwReserved1 );
	test.SetReserved2( dwReserved2 );
	test.SetPlayFlags( dwPlayFlags );

	CHECKEXECUTE( test.Play() );

	return hr;
}

HRESULT TestPlay( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwReserves ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwReserves ); j++ )
		{
			for ( DWORD k = 0; k < NUMELEMS( g_dwFlags ); k++ )
			{
				DbgPrint( "Reserve1: %d Reserve2: %d Flags: %d\n", g_dwReserves[i], g_dwReserves[j], g_dwFlags[k] );
				EXECUTE( TestPlay( g_dwReserves[i], g_dwReserves[j], g_dwFlags[k] ) );
			}
		}
	}

	return hr;
}
/*
HRESULT TestUnlock( LPVOID pvBuffer1, DWORD dwSize1, LPVOID pvBuffer2, DWORD dwSize2 )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetLockPointer1( pvBuffer1 );
	test.SetLockPointer2( pvBuffer2 );
	test.SetLockSize1( dwSize1 );
	test.SetLockSize2( dwSize2 );

	CHECKEXECUTE( test.Unlock() );

	return hr;
}

HRESULT TestUnlock( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_pvBuffers ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwBufferSizes ); j++ )
		{
			for ( DWORD k = 0; k < NUMELEMS( g_pvBuffers ); k++ )
			{
				for ( DWORD l = 0; l < NUMELEMS( g_dwBufferSizes ); l++ )
				{
					DbgPrint( "Buffer1: 0x%x size: %u Buffer2: 0x%x size: %u\n", g_pvBuffers[i], g_dwBufferSizes[j], g_pvBuffers[k], g_dwBufferSizes[l] );
					EXECUTE( TestUnlock( g_pvBuffers[i], g_dwBufferSizes[j], g_pvBuffers[k], g_dwBufferSizes[l] ) );
				}
			}
		}
	}

	return hr;
	
}
*/
HRESULT TestLock( DWORD dwWriteCursor, DWORD dwWriteBytes, DWORD dwLockFlags )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetWriteCursor( dwWriteCursor );
	test.SetWriteBytes( dwWriteBytes );
	test.SetLockFlags( dwLockFlags );

	CHECKEXECUTE( test.Lock() );

	return hr;
}


HRESULT TestLock( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_dwWriteCursors ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwBufferSizes ); j++ )
		{
			for ( DWORD k = 0; k < NUMELEMS( g_dwLockFlags ); k++ )
			{
				DbgPrint( "Write Cursor: %u Write Bytes: %u Flags: %u\n", g_dwWriteCursors[i], g_dwBufferSizes[j], g_dwLockFlags[k] );
				EXECUTE( TestLock( g_dwWriteCursors[i], g_dwBufferSizes[j], g_dwLockFlags[k] ) );
			}
		}
	}

	return hr;
}

HRESULT TestStopEx( REFERENCE_TIME rt, DWORD dwFlags )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetTimeStamp( rt );
	test.SetFlags( dwFlags );

	CHECKEXECUTE( test.StopEx() );

	return hr;
}

HRESULT TestStopEx( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_rtTimeStamps ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwFlags ); j++ )
		{
			DbgPrint( "Time Stamp: %u Flags: %u\n", g_rtTimeStamps[i], g_dwFlags[j] );
			EXECUTE( TestStopEx( g_rtTimeStamps[i], g_dwFlags[j] ) );
		}
	}

	return hr;
}

HRESULT TestPlayEx( REFERENCE_TIME rt, DWORD dwFlags )
{
	HRESULT hr = S_OK;

	CDSoundTest test;
	CHECKEXECUTE( test.Open( BVT_FILE ) );

	test.SetTimeStamp( rt );
	test.SetFlags( dwFlags );

	CHECKEXECUTE( test.PlayEx() );

	return hr;
}

HRESULT TestPlayEx( void )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < NUMELEMS( g_rtTimeStamps ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_dwFlags ); j++ )
		{
			DbgPrint( "Time Stamp: %u Flags: %u\n", g_rtTimeStamps[i], g_dwFlags[j] );
			EXECUTE( TestPlayEx( g_rtTimeStamps[i], g_dwFlags[j] ) );
		}
	}

	return hr;
}

HRESULT TestAPI( void )
{
	HRESULT hr = S_OK;

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "CreateDSound" );
	EXECUTE( TestCreateDSound() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "CreateBuffer" );
	EXECUTE( TestCreateBuffer() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "CreateSoundBuffer" );
	EXECUTE( TestCreateSoundBuffer() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "CommitDefferedSettings" );
	EXECUTE( TestCommitDefferedSettings() );

//	SETLOG( g_hLog, "danrose", "DSOUND", "API", "GetCaps" );
//	EXECUTE( TestGetCaps() );

//	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetSpeakerConfig" );
//	EXECUTE( TestSetSpeakerConfig() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "GetSpeakerConfig" );
	EXECUTE( TestGetSpeakerConfig() );

//	SETLOG( g_hLog, "danrose", "DSOUND", "API", "Listener::SetAllParameters" );
//	EXECUTE( TestListenerSetAllParameters() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetDistanceFactor" );
	EXECUTE( TestSetDistanceFactor() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetDopplerFactor" );
	EXECUTE( TestSetDopplerFactor() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetRolloffFactor" );
	EXECUTE( TestSetRolloffFactor() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetPosition" );
	EXECUTE( TestListenerSetPosition() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetVelocity" );
	EXECUTE( TestListenerSetVelocity() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetOrientation" );
	EXECUTE( TestSetOrientation() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "Stop" );
	EXECUTE( TestStop() );

#ifdef    SILVER

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetChannelVolume" );
	EXECUTE( TestSetChannelVolume() );

#endif // SILVER

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetVolume" );
	EXECUTE( TestSetVolume() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "GetStatus" );
	EXECUTE( TestGetStatus() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetFrequency" );
	EXECUTE( TestSetFrequency() );

//	SETLOG( g_hLog, "danrose", "DSOUND", "API", "Buffer3D::SetAllParameters" );
//	EXECUTE( TestBuffer3DSetAllParameters() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetConeOutsideVolume" );
	EXECUTE( TestSetConeOutsideVolume() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetMaxDistance" );
	EXECUTE( TestSetMaxDistance() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetMinDistance" );
	EXECUTE( TestSetMinDistance() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetMode" );
	EXECUTE( TestSetMode() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetNotificationPositions" );
	EXECUTE( TestSetNotificationPositions() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetCurrentPosition" );
	EXECUTE( TestSetCurrentPosition() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "GetCurrentPosition" );
	EXECUTE( TestGetCurrentPosition() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetBufferData" );
	EXECUTE( TestSetBufferData() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetLoopRegion" );
	EXECUTE( TestSetLoopRegion() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "SetConeAngles" );
	EXECUTE( TestSetConeAngles() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "Play" );
	EXECUTE( TestPlay() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "Lock" );
	EXECUTE( TestLock() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "StopEx" );
	EXECUTE( TestStopEx() );

	SETLOG( g_hLog, "danrose", "DSOUND", "API", "PlayEx" );
	EXECUTE( TestPlayEx() );

	return hr;
}

VOID WINAPI DsoundAPIStartTest( HANDLE LogHandle )
{
	HRESULT hr = S_OK;

	SETLOG( LogHandle, "danrose", "DSOUND", "API", "TestAPI");
	CHECK( TestAPI() );
}

VOID WINAPI DsoundAPIEndTest( VOID )
{
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( dsAPI )
#pragma data_seg()

BEGIN_EXPORT_TABLE( dsAPI )
    EXPORT_TABLE_ENTRY( "StartTest", DsoundAPIStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DsoundAPIEndTest )
END_EXPORT_TABLE( dsAPI )