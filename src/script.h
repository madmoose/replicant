#ifndef SCRIPT_H
#define SCRIPT_H

class engine_t;

struct script_t
{
	engine_t *engine;

	script_t(engine_t *a_engine)
		: engine(a_engine)
	{}

	virtual void initialize_scene() = 0;
	virtual void scene_frame_advanced(int frame) = 0;

	void set_loop_special(int loop, bool wait);
	void set_loop_default(int loop);

	void sound_play(int id, int volume, int a3, int a4, int a5);

	void actor_voice_over(int sentence_id, int actor_id);
	void outtake_play(int id, int a2, int a3);

	void ambient_sounds_add_sound(int id, int time1, int time2, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11);
	void ambient_sounds_add_looping_sound(int id, int volume, int a3, int a4);
};

script_t *get_script_by_name(engine_t *engine, const char *name);

/*
	script methods:

	_SCRIPT_Initialize_Scene
	_SCRIPT_Mouse_Click
	_SCRIPT_Clicked_On_3D_Object
	_SCRIPT_Clicked_On_Actor
	_SCRIPT_Clicked_On_Item
	_SCRIPT_Scene_Loaded
	_SCRIPT_Player_Walked_In
	_SCRIPT_Player_Walked_Out
	_SCRIPT_Scene_Frame_Advanced
	_SCRIPT_Clicked_On_Exit
	_SCRIPT_Actor_Changed_Goal
	_SCRIPT_Dialogue_Queue_Flushed
	_SCRIPT_Clicked_On_2D_Region
*/

#endif
