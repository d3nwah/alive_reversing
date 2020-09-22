#include "stdafx_ao.h"
#include "Function.hpp"
#include "Door.hpp"
#include "Abe.hpp"
#include "SwitchStates.hpp"
#include "ResourceManager.hpp"
#include "Collisions.hpp"
#include "stdlib.hpp"

START_NS_AO

struct Door_Data
{
    int field_0;
    int field_4;
    int field_8;
    int field_C;
    int field_10;
    int field_14;
    int field_18;
    int field_1C;
    int field_20;
    int field_24;
    int field_28;
    int field_2C;
};
ALIVE_ASSERT_SIZEOF(Door_Data, 0x30);

const Door_Data sDoorData_4BA508[16] =
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 6632, 6608, 56, 62, 6632, 6608, 56, 62, 6632, 6608, 56, 62 },
    { 4784, 4760, 55, 48, 4784, 4760, 55, 48, 4784, 4760, 55, 48 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 6624, 6600, 63, 62, 2036, 2012, 34, 29, 2072, 2048, 51, 27 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 3176, 3152, 52, 69, 1048, 1024, 21, 29, 1016, 992, 26, 31 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 6632, 6608, 56, 62, 6632, 6608, 56, 62, 6632, 6608, 56, 62 },
    { 6632, 6608, 56, 62, 6632, 6608, 56, 62, 6632, 6608, 56, 62 },
    { 6624, 6600, 63, 62, 2036, 2012, 34, 29, 2072, 2048, 51, 27 },
    { 3176, 3152, 52, 69, 1048, 1024, 21, 29, 1016, 992, 26, 31 }
};

Door* Door::ctor_40E010(Path_Door* pTlv, int tlvInfo)
{
    ctor_417C10(); // NOTE: Pointless base ctor omitted

    SetVTable(this, 0x4BA808);
    field_4_typeId = Types::eDoor_21;

    field_E4_tlvInfo = tlvInfo;

    field_E8_start_state =  pTlv->field_26_start_state;
    field_EE_door_closed = pTlv->field_28_door_closed;
    field_F0_switch_id = pTlv->field_22_id;

    if (field_F0_switch_id == 1)
    {
        field_F0_switch_id = 0;
    }

    field_EA_door_number = pTlv->field_20_door_number;

    field_EC_current_state = (field_EE_door_closed == 0) == SwitchStates_Get(field_F0_switch_id);

    if ((sActiveHero_507678->field_FC_current_motion == eAbeStates::State_156_DoorEnter_42D370 ||
         sActiveHero_507678->field_FC_current_motion == eAbeStates::State_157_DoorExit_42D780) &&
        field_EC_current_state == 1 &&
        field_EA_door_number == sActiveHero_507678->field_196_door_id)
    {
        field_EC_current_state = 0;
    }

    const int idx = static_cast<int>(gMap_507BA8.field_0_current_level);

    FP scale = {};
    PathLine* pLine = nullptr;
    BYTE** ppRes = nullptr;
    PSX_Point mapCoords = {};
    switch (field_E8_start_state)
    {
    case 0:
        ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, 2012, 1, 0);
        if (!ppRes)
        {
            field_6_flags.Clear(BaseGameObject::eDrawable_Bit4);
            field_6_flags.Set(BaseGameObject::eDead_Bit3);
            return this;
        }

        if (sDoorData_4BA508[idx].field_4)
        {
            if (field_EC_current_state)
            {
                Animation_Init_417FD0(
                    sDoorData_4BA508[idx].field_0,
                    sDoorData_4BA508[idx].field_8,
                    sDoorData_4BA508[idx].field_C,
                    ppRes,
                    1);
            }
            else
            {
                Animation_Init_417FD0(
                    sDoorData_4BA508[idx].field_4,
                    sDoorData_4BA508[idx].field_8,
                    sDoorData_4BA508[idx].field_C,
                    ppRes,
                    1);
            }

            if (pTlv->field_1E_scale == 1)
            {
                field_BC_sprite_scale = FP_FromDouble(0.5);
                field_C6_scale = 0;
                field_10_anim.field_C_layer = 6;
            }
            else
            {
                field_BC_sprite_scale = FP_FromInteger(1);
                field_C6_scale = 1;
                field_10_anim.field_C_layer = 25;
            }

            if (sCollisions_DArray_504C6C->RayCast_40C410(
                FP_FromInteger(pTlv->field_C_sound_pos.field_0_x + (pTlv->field_14_bottom_right.field_0_x - pTlv->field_10_top_left.field_0_x) / 2),
                FP_FromInteger(pTlv->field_10_top_left.field_2_y),
                FP_FromInteger(pTlv->field_C_sound_pos.field_0_x + (pTlv->field_14_bottom_right.field_0_x - pTlv->field_10_top_left.field_0_x) / 2),
                FP_FromInteger(pTlv->field_14_bottom_right.field_2_y),
                &pLine,
                &field_A8_xpos,
                &field_AC_ypos,
                field_BC_sprite_scale != FP_FromDouble(0.5) ? 7 : 0x70))
            {
                field_AC_ypos -= (FP_FromInteger(12) * field_BC_sprite_scale);
                gMap_507BA8.GetCurrentCamCoords_444890(&mapCoords);
                auto aux = Grid_SnapX_41FAA0(field_BC_sprite_scale, FP_GetExponent(field_A8_xpos) - mapCoords.field_0_x);
                field_A8_xpos = FP_FromInteger((aux) + mapCoords.field_0_x);
            }
            else
            {
                field_A8_xpos = FP_FromInteger(pTlv->field_C_sound_pos.field_0_x + 12);
                field_AC_ypos = FP_FromInteger(pTlv->field_C_sound_pos.field_2_y + 24);
            }
            break;
        }
        ResourceManager::FreeResource_455550(ppRes);
        field_6_flags.Clear(BaseGameObject::eDrawable_Bit4);
        field_6_flags.Set(BaseGameObject::eDead_Bit3);
        return this;

    case 1:
        if (gMap_507BA8.field_0_current_level == LevelIds::eRuptureFarmsReturn_13)
        {
            field_10_anim.field_C_layer = 25;
            scale = FP_FromInteger(1);
        }
        else
        {
            field_10_anim.field_C_layer = 6;
            scale = FP_FromDouble(0.5);
        }
        ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, 2019, 1, 0);
        if (!ppRes || sDoorData_4BA508[idx].field_14 == 0)
        {
            field_6_flags.Clear(BaseGameObject::eDrawable_Bit4);
            field_6_flags.Set(BaseGameObject::eDead_Bit3);
            return this;
        }

        if (field_EC_current_state)
        {
            Animation_Init_417FD0(
                sDoorData_4BA508[idx].field_10 ,
                sDoorData_4BA508[idx].field_18,
                sDoorData_4BA508[idx].field_1C,
                ppRes,
                1);
        }
        else
        {
            Animation_Init_417FD0(
                sDoorData_4BA508[idx].field_14,
                sDoorData_4BA508[idx].field_18,
                sDoorData_4BA508[idx].field_1C,
                ppRes,
                1);
        }

        if (sCollisions_DArray_504C6C->RayCast_40C410(
            FP_FromInteger(pTlv->field_C_sound_pos.field_0_x) + FP_FromInteger((pTlv->field_14_bottom_right.field_0_x - pTlv->field_10_top_left.field_0_x) / 2),
            FP_FromInteger(pTlv->field_10_top_left.field_2_y),
            FP_FromInteger(pTlv->field_C_sound_pos.field_0_x) + FP_FromInteger((pTlv->field_14_bottom_right.field_0_x - pTlv->field_10_top_left.field_0_x) / 2),
            FP_FromInteger(pTlv->field_14_bottom_right.field_2_y),
            &pLine,
            &field_A8_xpos,
            &field_AC_ypos,
            scale != FP_FromDouble(0.5) ? 7 : 0x70))
        {
            field_AC_ypos += FP_FromInteger(4);
            gMap_507BA8.GetCurrentCamCoords_444890(&mapCoords);
            field_A8_xpos = FP_FromInteger(Grid_SnapX_41FAA0(scale, FP_GetExponent(field_A8_xpos) - mapCoords.field_0_x) + mapCoords.field_0_x);
        }
        else
        {
            field_A8_xpos = FP_FromInteger(pTlv->field_C_sound_pos.field_0_x);
            field_AC_ypos = FP_FromInteger(pTlv->field_C_sound_pos.field_2_y);
        }
        field_BC_sprite_scale = FP_FromInteger(1);
        break;

    case 2:
        if (gMap_507BA8.field_0_current_level == LevelIds::eRuptureFarmsReturn_13 ||
            gMap_507BA8.field_0_current_level == LevelIds::eRuptureFarms_1)
        {
            ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, 2012, 1, 0);
        }
        else
        {
            ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, 2018, 1, 0);
        }

        if (ppRes)
        {
            if (sDoorData_4BA508[idx].field_24)
            {
                if (field_EC_current_state)
                {
                    Animation_Init_417FD0(
                        sDoorData_4BA508[idx].field_20,
                        sDoorData_4BA508[idx].field_28,
                        sDoorData_4BA508[idx].field_2C,
                        ppRes,
                        1);
                }
                else
                {
                    Animation_Init_417FD0(
                        sDoorData_4BA508[idx].field_24,
                        sDoorData_4BA508[idx].field_28,
                        sDoorData_4BA508[idx].field_2C,
                        ppRes,
                        1);
                }

                field_10_anim.field_C_layer = 6;

                if (gMap_507BA8.field_0_current_level == LevelIds::eRuptureFarmsReturn_13 ||
                    gMap_507BA8.field_0_current_level == LevelIds::eRuptureFarms_1)
                {
                    if (sCollisions_DArray_504C6C->RayCast_40C410(
                        FP_FromInteger(pTlv->field_C_sound_pos.field_0_x + (pTlv->field_14_bottom_right.field_0_x - pTlv->field_10_top_left.field_0_x) / 2),
                        FP_FromInteger(pTlv->field_10_top_left.field_2_y),
                        FP_FromInteger(pTlv->field_C_sound_pos.field_0_x + (pTlv->field_14_bottom_right.field_0_x - pTlv->field_10_top_left.field_0_x) / 2),
                        FP_FromInteger(pTlv->field_14_bottom_right.field_2_y),
                        &pLine,
                        &field_A8_xpos,
                        &field_AC_ypos,
                        7))
                    {
                        field_AC_ypos -= (FP_FromInteger(12) * field_BC_sprite_scale);
                        gMap_507BA8.GetCurrentCamCoords_444890(&mapCoords);
                        field_A8_xpos = FP_FromInteger(Grid_SnapX_41FAA0(FP_FromInteger(1), FP_GetExponent(field_A8_xpos) - mapCoords.field_0_x) + mapCoords.field_0_x);
                    }
                    else
                    {
                        field_A8_xpos = FP_FromInteger(pTlv->field_C_sound_pos.field_0_x + 12);
                        field_AC_ypos = FP_FromInteger(pTlv->field_C_sound_pos.field_2_y + 24);
                    }
                }
                else
                {
                    field_A8_xpos = FP_FromInteger(pTlv->field_C_sound_pos.field_0_x + 9);
                    field_AC_ypos = FP_FromInteger(pTlv->field_C_sound_pos.field_2_y + 20);
                }

                field_BC_sprite_scale = FP_FromInteger(1);

                field_F2_hubs_ids[0] = pTlv->field_2A_hubs[0];
                field_F2_hubs_ids[1] = pTlv->field_2A_hubs[1];
                field_F2_hubs_ids[2] = pTlv->field_2A_hubs[2];
                field_F2_hubs_ids[3] = pTlv->field_2A_hubs[3];
                field_F2_hubs_ids[4] = pTlv->field_2A_hubs[4];
                field_F2_hubs_ids[5] = pTlv->field_2A_hubs[5];
                field_F2_hubs_ids[6] = pTlv->field_2A_hubs[6];
                field_F2_hubs_ids[7] = pTlv->field_2A_hubs[7];
                break;
            }
        }
        field_6_flags.Clear(BaseGameObject::eDrawable_Bit4);
        field_6_flags.Set(BaseGameObject::eDead_Bit3);
        return this;
    }

    field_A8_xpos += FP_FromInteger(pTlv->field_3E_x_offset);
    field_AC_ypos += FP_FromInteger(pTlv->field_40_y_offset);

    if (field_EC_current_state == 0)
    {
        field_10_anim.field_4_flags.Clear(AnimFlags::eBit3_Render);
    }
    field_10_anim.field_4_flags.Clear(AnimFlags::eBit2_Animate);
    return this;
}

BaseGameObject* Door::dtor_40E710()
{
    SetVTable(this, 0x4BA808);
    gMap_507BA8.TLV_Reset_446870(field_E4_tlvInfo, -1, 0, 0);
    return dtor_417D10();
}

void Door::VScreenChanged()
{
    VScreenChanged_40EDE0();
}

void Door::VScreenChanged_40EDE0()
{
    field_6_flags.Set(BaseGameObject::eDead_Bit3);
}

BaseGameObject* Door::VDestructor(signed int flags)
{
    return Vdtor_40EDF0(flags);
}

Door* Door::Vdtor_40EDF0(signed int flags)
{
    dtor_40E710();
    if (flags & 1)
    {
        ao_delete_free_447540(this);
    }
    return this;
}

END_NS_AO
