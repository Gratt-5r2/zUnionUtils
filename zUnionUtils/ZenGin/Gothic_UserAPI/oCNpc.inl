// Supported with union (c) 2020 Union team

// User API for oCNpc
// Add your methods here

int SearchFloorPosition( zVEC3& position, const float& heightFactor = 1.8f );
int SetOnFloorAtThis();
void SetOnFloor_Union( zVEC3& pos );
void KeepGround();
int IsValidVisual();
int DoHeightCorrection();
void oCNpc::EquipItem_Union( oCItem* item );
void oCNpc::Equip_Union( oCItem* item );
void oCNpc::SplitItem( oCItem* item );
void oCNpc::UnsplitItem( oCItem* item );
void oCNpc::UnequipItem_Union( oCItem* item );
oCVob* oCNpc::RemoveFromSlot_Union( TNpcSlot*, int dropIt );
oCItem* oCNpc::PutInInv_Union( oCItem* item );
oCItem* GetSameEquiped( oCItem* item );
int UseItem_Union( oCItem* );
int oCNpc::EV_InsertInteractItem_Union( oCMsgManipulate* csg );
void UnequipItem( int instance );
int EV_DropVob_Union( oCMsgManipulate* msg );