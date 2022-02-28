// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
#if ENGINE <= Engine_G1A
	const int ITM_FLAG_NSPLIT = 1 << 26;

	static bool MultiSlotWeaponsEnabled = ENGINE <= Engine_G2 && ReadOption<bool>( "PARAMETERS", "MultiSlotWeapons" );
	static bool MultiSlotMagicEnabled   = ENGINE <= Engine_G2 && ReadOption<bool>( "PARAMETERS", "MultiSlotMagic" );
	static bool MultiSlotEnabled				= MultiSlotWeaponsEnabled || MultiSlotMagicEnabled;
	static bool NextSplitItemIsNotSplit = false;
	static oCVob* LastRemovedSlotVob		= Null;
	static oCVob* LastDropedVob					= Null;


	static bool ItemIsWeapon( oCItem* item ) {
		return item->HasFlag( ITM_CAT_NF ) || item->HasFlag( ITM_CAT_FF );
	}

	static bool ItemIsMagic( oCItem* item ) {
		return item->HasFlag( ITM_CAT_MAGIC ) || item->HasFlag( ITM_FLAG_RING ) || item->HasFlag( ITM_FLAG_AMULET );
	}

	inline bool ItemHasSplitFlag( oCItem* item ) {
		if( MultiSlotWeaponsEnabled && ItemIsWeapon( item ) )
			return true;

		if( MultiSlotMagicEnabled && ItemIsMagic( item ) )
			return true;

		return false;
	}


	int oCItem::SplitSlot_Union() {
		static int pickAxeID		= parser->GetIndex( "ITMWPICKAXE" );
		static int craftSwordID = parser->GetIndex( "ITMW_1H_SWORD_01" );

		if( HasFlag( ITM_FLAG_NSPLIT ) )
			return False;

		if( instanz == pickAxeID || instanz == craftSwordID )
			return False;

		int splitItem = ItemHasSplitFlag( this );
		if( !splitItem )
			return False;

		if( NextSplitItemIsNotSplit ) {
			NextSplitItemIsNotSplit = False;
			return false;
		}

		return True;
	}


	HOOK Hook_oCItem_MultiSlot PATCH_IF( &oCItem::MultiSlot, &oCItem::MultiSlot_Union, MultiSlotEnabled );

	int oCItem::MultiSlot_Union() {
		if( SplitSlot_Union() ) {
			return !HasFlag( ITM_FLAG_ACTIVE );
		}

		return HasFlag( ITM_FLAG_MULTI ) || HasFlag( ITM_CAT_MUN ) || HasFlag( ITM_CAT_FOOD );
	}


	HOOK Hook_oCNpc_Equip PATCH_IF( &oCNpc::Equip, &oCNpc::Equip_Union, MultiSlotMagicEnabled );

	void oCNpc::Equip_Union( oCItem* item ) {
		if( ItemIsMagic( item ) ) {
			if( item->HasFlag( ITM_FLAG_ACTIVE ) ) {
				THISCALL( Hook_oCNpc_Equip )(item);
				SplitItem( item );
			}
			else {
				THISCALL( Hook_oCNpc_Equip )(item);
				if( item->HasFlag( ITM_FLAG_ACTIVE ) )
					UnsplitItem( item );
			}

			return;
		}

		THISCALL( Hook_oCNpc_Equip )(item);
	}


	HOOK Hook_oCNpc_EquipItem PATCH_IF( &oCNpc::EquipItem, &oCNpc::EquipItem_Union, MultiSlotEnabled );

	void oCNpc::UnsplitItem( oCItem* item ) {
		if( item->SplitSlot_Union() && (item->amount > 1) ) {
			oCItem* split = item->SplitItem( item->amount - 1 );
			if( split ) {
				split->SetFlag( ITM_FLAG_ACTIVE );
				item->amount = 1;
				inventory2.Insert( split );
				split->ClearFlag( ITM_FLAG_ACTIVE );
				zRELEASE( split );
			}
		}
	}

	void oCNpc::EquipItem_Union( oCItem* item ) {
		if( !item || item->HasFlag( ITM_FLAG_ACTIVE ) )
			return;

		UnsplitItem( item );
		return THISCALL( Hook_oCNpc_EquipItem )(item);
	}


	HOOK Hook_oCNpc_UnequipItem PATCH_IF( &oCNpc::UnequipItem, &oCNpc::UnequipItem_Union, MultiSlotEnabled );

	void oCNpc::SplitItem( oCItem* item ) {
		if( !LastRemovedSlotVob || LastDropedVob == item )
			return;

		if( item->SplitSlot_Union() ) {
			zCListSort<oCItem>* ele = inventory2.GetContents();
			while( ele ) {
				oCItem* i = ele->GetData();
				if( i && i->SplitSlot_Union() && !i->HasFlag( ITM_FLAG_ACTIVE ) &&
					(i != item) && (i->GetInstance() == item->GetInstance()) ) {
					i->amount += item->amount;
					inventory2.Remove( item );

					if( item->refCtr >= 2 )
						ogame->GetGameWorld()->RemoveVob( item );

					break;
				}

				ele = ele->GetNextInList();
			}
		}
	}

	void oCNpc::UnequipItem_Union( oCItem* item ) {
		LastRemovedSlotVob = Null;
		THISCALL( Hook_oCNpc_UnequipItem )(item);
    if( item && !item->HasFlag( ITM_FLAG_MULTI ) )
			SplitItem( item );
	}


	HOOK Hook_oCNpc_RemoveFromSlot PATCH_IF( &oCNpc::RemoveFromSlot, &oCNpc::RemoveFromSlot_Union, MultiSlotEnabled );

	oCVob* oCNpc::RemoveFromSlot_Union( TNpcSlot* slot, int dropIt ) {
		LastRemovedSlotVob = THISCALL( Hook_oCNpc_RemoveFromSlot )(slot, dropIt);
		return LastRemovedSlotVob;
	}


	HOOK Hook_oCNpcInventory_Insert PATCH_IF( &oCNpcInventory::Insert, &oCNpcInventory::Insert_Union, MultiSlotEnabled );

	oCItem* oCNpcInventory::Insert_Union( oCItem* item ) {
		if( item->SplitSlot_Union() ) {
			oCItem* sameEquipedItem = GetSameEquiped( item );
			int instance = 0;
			if( sameEquipedItem ) {
				instance = sameEquipedItem->instanz;
				sameEquipedItem->instanz = -1;
			}

			item = THISCALL( Hook_oCNpcInventory_Insert )(item);

			if( sameEquipedItem )
				sameEquipedItem->instanz = instance;

			return item;
		}

		return THISCALL( Hook_oCNpcInventory_Insert )(item);
	}


	oCItem* oCNpcInventory::GetSameEquiped( oCItem* item ) {
		int categoty = GetCategory( item );

		if( ItemHasSplitFlag( item ) ) {
			zCListSort<oCItem>& inv = inventory[categoty];
			zCListSort<oCItem>* list = inv.next;
			while( list ) {
				oCItem* itemOther = list->GetData();
				if( itemOther && item->instanz == itemOther->instanz && itemOther->HasFlag( ITM_FLAG_ACTIVE ) )
					return itemOther;

				list = list->next;
			}
		}

		return Null;
	}


	HOOK Hook_oCNpc_EV_DropVob PATCH_IF( &oCNpc::EV_DropVob, &oCNpc::EV_DropVob_Union, MultiSlotEnabled );

	int oCNpc::EV_DropVob_Union( oCMsgManipulate* msg ) {
		if( this != player )
			return THISCALL( Hook_oCNpc_EV_DropVob )(msg);

		LastDropedVob = (oCVob*)msg->targetVob;
		int ok = THISCALL( Hook_oCNpc_EV_DropVob )(msg);
		LastDropedVob = Null;
		return ok;
	}
#endif
}