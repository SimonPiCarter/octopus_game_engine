
#include "GridBuild.h"

#include <algorithm>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot
{
	void GridBuild::set_textures(AtlasTexture *standard_p, AtlasTexture *highlighted_p, AtlasTexture *blocked_p)
	{
		_standard = standard_p;
		_highlighted = highlighted_p;
		_blocked = blocked_p;
	}

	void GridBuild::reset()
	{
		update_data();
	}

	TypedArray<Vector2i> GridBuild::get_root_coord()
	{
		TypedArray<Vector2i> array_l;
		for(int i = 0 ; i < _world_size; ++ i)
		{
			for(int j = 0 ; j < _world_size; ++ j)
			{
				if(_data[i][j].blocked && _data[i][j].root == Vector2i(i,j))
				{
					array_l.push_back(_data[i][j].root);
				}
			}
		}
		return array_l;
	}


	void GridBuild::_ready()
	{
		update_data();
	}

	void GridBuild::_process(double delta_p)
	{
		Node2D::_process(delta_p);
	}

	void GridBuild::_draw()
	{
		for(int i = 0 ; i < _world_size; ++ i)
		{
			for(int j = 0 ; j < _world_size; ++ j)
			{
				if(_data[i][j].highlighted)
				{
					if(_highlighted)
					{
						draw_texture_rect_region(_highlighted->get_atlas(),
							Rect2(Vector2(i*_size_case, j*_size_case), Vector2(_size_case, _size_case)) ,
							_highlighted->get_region());
					}
				}
				else if(_data[i][j].blocked)
				{
					if(_highlighted)
					{
						draw_texture_rect_region(_blocked->get_atlas(),
							Rect2(Vector2(i*_size_case, j*_size_case), Vector2(_size_case, _size_case)) ,
							_blocked->get_region());
					}
				}
				else
				{
					if(_standard)
					{
						draw_texture_rect_region(_standard->get_atlas(),
							Rect2(Vector2(i*_size_case, j*_size_case), Vector2(_size_case, _size_case)) ,
							_standard->get_region());
					}
				}
			}
		}


		for(int i = 0 ; i < _world_size; ++ i)
		{
			for(int j = 0 ; j < _world_size; ++ j)
			{
				if(_data[i][j].texture)
				{
					draw_texture(_data[i][j].texture, Vector2(i*_size_case, j*_size_case) + _data[i][j].offset);
				}
			}
		}
	}

	void GridBuild::_bind_methods()
	{
		ClassDB::bind_method(D_METHOD("set_textures", "standard", "highlighted", "blocked"), &GridBuild::set_textures);
		ClassDB::bind_method(D_METHOD("reset"), &GridBuild::reset);
		ClassDB::bind_method(D_METHOD("get_root_coord"), &GridBuild::get_root_coord);

		// case edition
		ClassDB::bind_method(D_METHOD("set_highlighted", "x", "y", "highlighted"), &GridBuild::set_highlighted);
		ClassDB::bind_method(D_METHOD("get_highlighted", "x", "y"), &GridBuild::get_highlighted);
		ClassDB::bind_method(D_METHOD("set_blocked", "x", "y", "blocked"), &GridBuild::set_blocked);
		ClassDB::bind_method(D_METHOD("get_blocked", "x", "y"), &GridBuild::get_blocked);
		ClassDB::bind_method(D_METHOD("set_case_index", "x", "y", "index"), &GridBuild::set_index);
		ClassDB::bind_method(D_METHOD("get_case_index", "x", "y"), &GridBuild::get_index);
		ClassDB::bind_method(D_METHOD("set_player", "x", "y", "player"), &GridBuild::set_player);
		ClassDB::bind_method(D_METHOD("get_player", "x", "y"), &GridBuild::get_player);
		ClassDB::bind_method(D_METHOD("set_qty", "x", "y", "qty"), &GridBuild::set_qty);
		ClassDB::bind_method(D_METHOD("get_qty", "x", "y"), &GridBuild::get_qty);
		ClassDB::bind_method(D_METHOD("set_group", "x", "y", "group"), &GridBuild::set_group);
		ClassDB::bind_method(D_METHOD("get_group", "x", "y"), &GridBuild::get_group);
		ClassDB::bind_method(D_METHOD("set_num_players", "x", "y", "num_players"), &GridBuild::set_num_players);
		ClassDB::bind_method(D_METHOD("get_num_players", "x", "y"), &GridBuild::get_num_players);
		ClassDB::bind_method(D_METHOD("set_texture", "x", "y", "texture"), &GridBuild::set_texture);
		ClassDB::bind_method(D_METHOD("get_texture", "x", "y"), &GridBuild::get_texture);
		ClassDB::bind_method(D_METHOD("set_offset", "x", "y", "offset"), &GridBuild::set_offset);
		ClassDB::bind_method(D_METHOD("get_offset", "x", "y"), &GridBuild::get_offset);
		ClassDB::bind_method(D_METHOD("set_root", "x", "y", "root"), &GridBuild::set_root);
		ClassDB::bind_method(D_METHOD("get_root", "x", "y"), &GridBuild::get_root);

		ADD_GROUP("GridBuild", "GridBuild_");
	}

	void GridBuild::update_data()
	{
		_data.clear();
		_data.resize(_world_size, std::vector<GridBuildData>(_world_size, GridBuildData()));
	}

} // namespace godot

