// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
#if ENGINE <= Engine_G1A
	static bool RenderItemEnabled = ENGINE <= Engine_G2 && ReadOption<bool>( "PARAMETERS", "FixRenderItem" );

	bool_t bRenderItem = False;

  HOOK Hook_oCItem_RenderItem PATCH_IF( &oCItem::RenderItem, &oCItem::RenderItem_Union, RenderItemEnabled );

	void oCItem::RenderItem_Union( zCWorld* world, zCViewBase* viewport, float rotation ) {
		bRenderItem = True;
		THISCALL( Hook_oCItem_RenderItem )( world, viewport, rotation );
		bRenderItem = False;
	}

  HOOK Hook_zCWorld_Render PATCH_IF( &zCWorld::Render, &zCWorld::Render_Union, RenderItemEnabled );

	void zCWorld::Render_Union( class zCCamera& camera ) {
		if( bRenderItem ) {
			static zCView* pView			 = new zCView( 0, 0, 8192, 8192, zTviewID::VIEW_ITEM );
			static zCWorld* pRendWorld = new zCWorld();
			static zCCamera* pWorldCam = new zCCamera();
			static zCVob* pCamVob			 = new zCVob();
			static bool init					 = false;

			if( !init && pWorldCam && pView && pRendWorld ) {
				pWorldCam->connectedVob = pCamVob;
				pRendWorld->AddVob( pCamVob );
				pWorldCam->SetRenderTarget( pView );
				pRendWorld->GetBspTree()->bspTreeMode = zBSP_MODE_INDOOR;
				zCSkyControler* sky = pRendWorld->GetActiveSkyControler();
				sky->SetFillBackground( False );
				init = true;
			}

			if( init ) {
				zCCamera* oldcam = zCCamera::activeCam;
				zCEventManager::disableEventManagers = True;
				(pRendWorld->*Hook_zCWorld_Render)(*pWorldCam);
				zCEventManager::disableEventManagers = False;
				zCCamera::activeCam = oldcam;
			}
		}

		THISCALL( Hook_zCWorld_Render )(camera);
	}
#endif
}