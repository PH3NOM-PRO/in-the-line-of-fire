#ifndef DCE_PLAYER_MODEL_H
#define DCE_PLAYER_MODEL_H

void DCE_PlayerAnimationEndFrame(DCE_Player * player, unsigned int render_frame);
void DCE_PlayerAnimationStartFrame(DCE_Player * player);
void DCE_InitPlayerModel(DCE_PlayerModel * model, char * fname);
void DCE_InitPlayerHandsModel(DCE_PlayerModel * model, char * fname);

void DCE_InitPlayerModelBuffers(DCE_PlayerModel * mdl, char * body_txt, char * hands_txt,
                                char * body_mdl, char * body_lod1, char * body_lod2,
								char * hands_mdl,
								char * weapon_mdl, char * weapon_lod1, char * weapon_lod2,
								char * body_tex, char * hands_tex, char * weapon_tex);

void DCE_InitPlayerModelBuffersHQ(DCE_PlayerModel * mdl, char * body_txt, char * hands_txt,
                                  char * body_mdl, char * body_lod1, char * body_lod2,
								  char * hands_mdl,
								  char * weapon_mdl, char * weapon_lod1, char * weapon_lod2,
								  char * body_tex, char * hands_tex, char * weapon_tex);

void DCE_PlayerModelLoad(DCE_PlayerModel * mdl, char * body_txt, char * hands_txt,
                                md2_mdl * body_mdl, md2_mdl * body_lod1, md2_mdl * body_lod2,
								md2_mdl * hands_mdl, md2_mdl * weapon_mdl,
								unsigned short body_texID, unsigned short hands_texID, unsigned short weapon_texID);

void DCE_PlayerAIStartFrame(DCE_Player * player);

void DCE_PlayerModelLoadClown(DCE_PlayerModel * mdl, char * body_txt, char * hands_txt);
void DCE_PlayerModelLoadSwat(DCE_PlayerModel * mdl, char * body_txt, char * hands_txt);

#endif
