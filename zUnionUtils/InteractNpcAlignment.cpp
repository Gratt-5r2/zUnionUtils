// Supported with union (c) 2020 Union team
// Union SOURCE file
#include <intrin.h>
#pragma intrinsic(_ReturnAddress)
namespace GOTHIC_ENGINE {
  int oCNpc::DoHeightCorrection() {
    zCCollObjectCharacter* collObject = dynamic_cast<zCCollObjectCharacter*>( GetCollisionObject() );
    if( !collObject )
      return False;

    if( !collObject->m_oConfig.m_eDoHeightCorrection )
      return False;

    return
      collObject->m_oConfig.m_eState == zCCollObjectCharacter::zCONFIG_STATE_STAND ||
      collObject->m_oConfig.m_eState == zCCollObjectCharacter::zCONFIG_STATE_SLIDE;
  }

  int oCNpc::SearchFloorPosition( zVEC3& position, const float& heightFactor ) {
    static zVEC3 vector = 0.0f;
    vector[VY] = human_ai->config.zMV_STEP_HEIGHT * -heightFactor * 2.0f;
#if 0
    int found = homeWorld->TraceRayNearestHit( GetNewTrafoObjToWorld().GetTranslation(), vector, interactMob, zTRACERAY_STAT_POLY | zTRACERAY_POLY_NORMAL | zTRACERAY_VOB_IGNORE_CHARACTER );
    if( found )
      position = homeWorld->traceRayReport.foundIntersection;

    return found;
#endif

    zVEC3 vobPoint      = interactMob->GetPositionWorld();
    zVEC3 startPoint    = GetNewTrafoObjToWorld().GetTranslation();
    int worldCollFound  = homeWorld->TraceRayNearestHit( startPoint, vector, interactMob, zTRACERAY_STAT_POLY | zTRACERAY_POLY_NORMAL | zTRACERAY_VOB_IGNORE | zTRACERAY_VOB_IGNORE_CHARACTER );
    if( !worldCollFound )
      return False;

    position            = homeWorld->traceRayReport.foundIntersection;
    startPoint[VY]      = vobPoint[VY] + 30.0f;
    int vobCollFound1   = homeWorld->TraceRayNearestHit( startPoint + 5.0f, vector, interactMob, zTRACERAY_STAT_POLY | zTRACERAY_POLY_NORMAL | zTRACERAY_VOB_IGNORE_CHARACTER );
    int vobCollFound2   = homeWorld->TraceRayNearestHit( startPoint,        vector, interactMob, zTRACERAY_STAT_POLY | zTRACERAY_POLY_NORMAL | zTRACERAY_VOB_IGNORE_CHARACTER );
    zVEC3 vobIntersec   = homeWorld->traceRayReport.foundIntersection;
    if( vobCollFound1 && vobCollFound2 )
      if( !worldCollFound || vobIntersec[VY] > position[VY] )
        position = vobIntersec;
    
    return worldCollFound || (vobCollFound1 && vobCollFound2);
  }


  int oCNpc::IsValidVisual() {
    if( !homeWorld )
      return False;

    zCModel* model = GetModel();
    if( !model )
      return False;

    return True;
  }

  int oCNpc::SetOnFloorAtThis() {
    if( !DoHeightCorrection() )
      return False;

    if( !IsValidVisual() )
      return False;

    zCModel* model = (zCModel*)visual;
    zVEC3 floor;
    if( SearchFloorPosition( floor ) ) {
      float rootHeight = model->rootPosLocal.n[VY] + 2.0f;
      floor[VY] += rootHeight;
      GetNewTrafoObjToWorld().SetTranslation( floor );
    }
    else
      MoveWorld( 0.0f, -0.01f, 0.0f );

    return True;
  }


  void oCNpc::KeepGround() {
    if( !IsValidVisual() )
      return;

    zCCollObjectCharacter* movingCollCharacter = dynamic_cast<zCCollObjectCharacter*>(m_poCollisionObject);
    if( movingCollCharacter )
      movingCollCharacter->UpdateFloorLevel();

    if( !interactMob->CastTo<oCMobLadder>() )
      return;

    static bool enabled = ReadOption<bool>( "PARAMETERS", "HeightCorrectionOnLadder" );
    if( !enabled )
      return;

    zVEC3 floor;
    if( SearchFloorPosition( floor, 100.0f ) ) {
      float minHeight = floor[VY] + human_ai->config.zMV_STEP_HEIGHT * 1.45f;
      zVEC3 positon   = GetNewTrafoObjToWorld().GetTranslation();

      if( positon[VY] < minHeight ) {
        positon[VY] = minHeight;
        GetNewTrafoObjToWorld().SetTranslation( positon );
      }
    }
  }


  void zCCollObjectCharacter::UpdateFloorLevel() {
    zCVob* vob = (zCVob*)m_pvClientData;
    FindFloorWaterCeiling( vob->GetPositionWorld(), m_oSpatialState );
  }

  HOOK Hook_zCCollisionDetector_DetectCollisions PATCH_IF( &zCCollisionDetector::DetectCollisions, &zCCollisionDetector::DetectCollisions_Union, ReadOption<bool>( "PARAMETERS", "InteractionCollision" ) );

  void __fastcall zCCollisionDetector::DetectCollisions_Union( zCCollisionObject* movingCollObject, const zCArray<zCCollisionObject*>& otherCollObjectListInput, zCArray<zCCollisionReport*>& collReportList ) {
    zCVob* vob = (zCVob*)movingCollObject->m_pvClientData;
    if( vob->type == zVOB_TYPE_NSC ) {
      zCCollObjectCharacter* movingCollCharacter = dynamic_cast<zCCollObjectCharacter*>(movingCollObject);

      if( movingCollCharacter ) {
        oCNpc* npc = (oCNpc*)vob;
        oCMobInter* mobInter = npc->interactMob;
        if( mobInter /*&& mobInter->CastTo<oCMobLadder>()*/ ) {
          static bool advancedFloorCheck = ReadOption<bool>("PARAMETERS", "AdvancedFloorCheckForInteract");
          if( advancedFloorCheck || mobInter->CastTo<oCMobLadder>() )
            if( !npc->SetOnFloorAtThis() )
              npc->KeepGround();
        }
      }
    }
  
    THISCALL( Hook_zCCollisionDetector_DetectCollisions )(movingCollObject, otherCollObjectListInput, collReportList);
  }
}