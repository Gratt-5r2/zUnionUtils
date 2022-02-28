// Supported with union (c) 2020 Union team

// User API for oCMobInter
// Add your methods here

void oCMobInter::InterruptInteraction_Union( oCNpc* npc );
void oCMobInter::EndInteraction_Union( oCNpc* npc, int playEndAni );
void oCMobInter::StopInteraction_Union( oCNpc* npc );
void oCMobInter::SendEndInteraction_Union( oCNpc* npc, int from, int to );
int oCMobInter::AI_UseMobToState_Union( oCNpc* npc, int target );
int oCMobInter::CanInteractWith_Union( oCNpc* npc );
int oCMobInter::IsAvailable_Union( oCNpc* npc );
void oCMobInter::CheckStateChange_Union( oCNpc* );
void StartInteraction_Union(oCNpc*);