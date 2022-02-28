// This file added in headers queue
// File: "Sources.h"
#include "resource.h"

namespace GOTHIC_ENGINE {
  void Game_Entry() {
    s_DistMult            =                             ReadOption<float>( "PARAMETERS", "DrawDistanceMultiplier" );
    s_LowLimitWindowed    =                             ReadOption<int>  ( "PARAMETERS", "AdaptiveSightDistance_LowerFPS" );
    s_UpLimitWindowed     =                             ReadOption<int>  ( "PARAMETERS", "AdaptiveSightDistance_UpperFPS" );
    s_AdaptiveSightInfo   =                             ReadOption<bool> ( "PARAMETERS", "AdaptiveSightDistance_ShowInfo" );
    s_AdaptiveSightOfView = !CUnion::Dx11IsEnabled() && ReadOption<bool> ( "PARAMETERS", "AdaptiveSightDistance" );
    s_VisualizePerfomance =                             ReadOption<bool> ( "PARAMETERS", "VisualizePerfomance" );
    s_FpsLimit            =                             ReadOption<int>  ( "PARAMETERS", "FPS_Limit" );

    if( s_FpsLimit && s_AdaptiveSightOfView ) {
      if( s_FpsLimit < s_UpLimitWindowed ) {
        Message::Error( string::Combine( "FpsLimit should not be less than AdaptiveSightDistance_UpperFPS" ) );
        s_AdaptiveSightOfView = false;
      }
    }

    if( s_AdaptiveSightOfView ) {
      if( s_LowLimitWindowed >= s_UpLimitWindowed ) {
        Message::Fatal( string::Combine( "AdaptiveSightDistance_LowerFPS should not be less than AdaptiveSightDistance_UpperFPS" ) );
        s_AdaptiveSightOfView = false;
      }
    }

    Union.GetDefaultOption().Write( 0, "INTERNAL", "UnionActivated" );
    Union.GetDefaultOption().SaveAll();
  }

  void SetOptionEnabled() {
    zoptions->WriteInt( "INTERNAL", "UnionActivated", 1, 1 );
  }

  void SetScriptEnabled() {
    zCPar_Symbol* symbol = parser->GetSymbol( "UnionActivated" );
    if( symbol != Null )
      symbol->SetValue( 1, 0 );
  }
  
  void Game_Init() {
    SetOptionEnabled();
    SetScriptEnabled();
  }

  void Game_Exit() {
  }

  void CheckFullScreenEvents() {
    auto screenMode = zrenderer->Vid_GetScreenMode();
    if( zKeyPressed( KEY_LALT ) || zKeyPressed( KEY_RALT ) ) {
      if( zKeyToggled( KEY_RETURN ) ) {
        if( screenMode == zTRnd_ScreenMode::zRND_SCRMODE_FULLSCREEN )
          zrenderer->Vid_SetScreenMode( zTRnd_ScreenMode::zRND_SCRMODE_WINDOWED );
        else
          zrenderer->Vid_SetScreenMode( zTRnd_ScreenMode::zRND_SCRMODE_FULLSCREEN );
      }
      else if( zKeyToggled( KEY_TAB ) )
        /* pass */;
    }


  }

  void Game_PreLoop() {
    CheckFullScreenEvents();
  }

  void Game_Loop() {
    if( s_AdaptiveSightOfView )
      AdaptiveSightOfView();

    if( s_VisualizePerfomance ) {
      GetPerfomanceVisualizer()->Tick();
      GetPerfomanceVisualizer()->Render();
    }
    FpsLimit( s_FpsLimit );
  }

  void Game_PostLoop() {
  }

  void Game_MenuLoop() {
    CheckFullScreenEvents();
  }

  // Information about current saving or loading world
  TSaveLoadGameInfo& SaveLoadGameInfo = UnionCore::SaveLoadGameInfo;

  void Game_SaveBegin() {
  }

  void Game_SaveEnd() {
  }

  void LoadBegin() {
  }

  void LoadEnd() {
    SetScriptEnabled();
  }

  void Game_LoadBegin_NewGame() {
    LoadBegin();
  }

  void Game_LoadEnd_NewGame() {
    LoadEnd();
  }

  void Game_LoadBegin_SaveGame() {
    LoadBegin();
  }

  void Game_LoadEnd_SaveGame() {
    LoadEnd();
  }

  void Game_LoadBegin_ChangeLevel() {
    LoadBegin();
  }

  void Game_LoadEnd_ChangeLevel() {
    LoadEnd();
  }

  void Game_LoadBegin_Trigger() {
  }
  
  void Game_LoadEnd_Trigger() {
  }
  
  void Game_Pause() {
  }
  
  void Game_Unpause() {
  }
  
  void Game_DefineExternals() {
  }

  void Game_ApplyOptions() {
  }

  /*
  Functions call order on Game initialization:
    - Game_Entry           * Gothic entry point
    - Game_DefineExternals * Define external script functions
    - Game_Init            * After DAT files init
  
  Functions call order on Change level:
    - Game_LoadBegin_Trigger     * Entry in trigger
    - Game_LoadEnd_Trigger       *
    - Game_Loop                  * Frame call window
    - Game_LoadBegin_ChangeLevel * Load begin
    - Game_SaveBegin             * Save previous level information
    - Game_SaveEnd               *
    - Game_LoadEnd_ChangeLevel   *
  
  Functions call order on Save game:
    - Game_Pause     * Open menu
    - Game_Unpause   * Click on save
    - Game_Loop      * Frame call window
    - Game_SaveBegin * Save begin
    - Game_SaveEnd   *
  
  Functions call order on Load game:
    - Game_Pause              * Open menu
    - Game_Unpause            * Click on load
    - Game_LoadBegin_SaveGame * Load begin
    - Game_LoadEnd_SaveGame   *
  */

#define AppDefault True
  CApplication* lpApplication = !CHECK_THIS_ENGINE ? Null : CApplication::CreateRefApplication(
    Enabled( AppDefault )               Game_Entry,
    Enabled( AppDefault )               Game_Init,
    Enabled( AppDefault )               Game_Exit,
    Enabled( !CUnion::Dx11IsEnabled() ) Game_PreLoop,
    Enabled( AppDefault )               Game_Loop,
    Enabled( AppDefault )               Game_PostLoop,
    Enabled( !CUnion::Dx11IsEnabled() ) Game_MenuLoop,
    Enabled( AppDefault )               Game_SaveBegin,
    Enabled( AppDefault )               Game_SaveEnd,
    Enabled( AppDefault )               Game_LoadBegin_NewGame,
    Enabled( AppDefault )               Game_LoadEnd_NewGame,
    Enabled( AppDefault )               Game_LoadBegin_SaveGame,
    Enabled( AppDefault )               Game_LoadEnd_SaveGame,
    Enabled( AppDefault )               Game_LoadBegin_ChangeLevel,
    Enabled( AppDefault )               Game_LoadEnd_ChangeLevel,
    Enabled( AppDefault )               Game_LoadBegin_Trigger,
    Enabled( AppDefault )               Game_LoadEnd_Trigger,
    Enabled( AppDefault )               Game_Pause,
    Enabled( AppDefault )               Game_Unpause,
    Enabled( AppDefault )               Game_DefineExternals,
    Enabled( AppDefault )               Game_ApplyOptions
  );
}