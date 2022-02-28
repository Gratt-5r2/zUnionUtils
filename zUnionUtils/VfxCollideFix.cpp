// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  static bool VfxCollideFix = True; // ReadOption<bool>( "PARAMETERS", "VfxCollideFix" );

  HOOK Ivk_oCVisualFX_ProcessQueuedCollisions PATCH_IF( &oCVisualFX::ProcessQueuedCollisions, &oCVisualFX::ProcessQueuedCollisions_Union, VfxCollideFix );

  int oCVisualFX::ProcessQueuedCollisions_Union() {
    bool32 collideBegin = False;

    // Указатель на список столкновений в самом родительском VFX
    zCArray<zCVob*>* baseCollidedVobs = Null;

    // Взятие самого родительского VFX
    // и получение его списка коллизий
    oCVisualFX* vfx = this;
    while( vfx ) {
      baseCollidedVobs = &vfx->collidedVobs;
      if( vfx == vfx->parent )
        break;

      vfx = vfx->parent;
    }

    if( !baseCollidedVobs )
      return False;

    // Поиск и добавление коллизии в список самого родительского VFX
    for( int i = 0; i < queuedCollisions.GetNum(); i++ ) {
      if( !baseCollidedVobs->IsInList( queuedCollisions[i].foundVob ) ) {
        collideBegin |= ProcessCollision( queuedCollisions[i] );
        baseCollidedVobs->Insert( queuedCollisions[i].foundVob );
        queuedCollisions[i].foundVob->AddRef();
      }

      zRELEASE( queuedCollisions[i].foundVob );
    }

    queuedCollisions.DeleteList();

    if( collideBegin )
      Collide( True );

    return collideBegin;
  };
}