
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

	TypedArray<Color> GridBuild::get_color_palett()
	{
		TypedArray<Color> res_l;
		for(auto &&pair_l : _palett)
		{
			res_l.push_back(pair_l.second);
		}
		return res_l;
	}

	TypedArray<int> GridBuild::get_labels_palett()
	{
		TypedArray<int> res_l;
		for(auto &&pair_l : _palett)
		{
			res_l.push_back(pair_l.first);
		}
		return res_l;
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

		// sprites
		for(int i = 0 ; i < _world_size && _show_sprite; ++ i)
		{
			for(int j = 0 ; j < _world_size; ++ j)
			{
				if(_data[i][j].texture)
				{
					draw_texture(_data[i][j].texture, Vector2(i*_size_case, j*_size_case) + _data[i][j].offset);
				}
			}
		}

		// color feature indicator
		for(int i = 0 ; i < _world_size && _feature != 0; ++ i)
		{
			for(int j = 0 ; j < _world_size; ++ j)
			{
				if(_data[i][j].blocked)
				{
					Vector2i root_l = _data[i][j].root;
					int index_l = get_index_from_feature(root_l.x, root_l.y);
					Color &color_l = _palett[index_l];
					if(color_l.is_equal_approx(Color(0,0,0,1)))
					{
						color_l = pop_color();
						color_l.a *= 0.75;
					}
					draw_rect(Rect2(Vector2(i*_size_case, j*_size_case), Vector2(_size_case, _size_case)), color_l);
				}
			}
		}
	}

	void GridBuild::_bind_methods()
	{
		ClassDB::bind_method(D_METHOD("set_textures", "standard", "highlighted", "blocked"), &GridBuild::set_textures);
		ClassDB::bind_method(D_METHOD("reset"), &GridBuild::reset);
		ClassDB::bind_method(D_METHOD("get_root_coord"), &GridBuild::get_root_coord);
		ClassDB::bind_method(D_METHOD("set_feature", "feature"), &GridBuild::set_feature);
		ClassDB::bind_method(D_METHOD("get_color_palett"), &GridBuild::get_color_palett);
		ClassDB::bind_method(D_METHOD("get_labels_palett"), &GridBuild::get_labels_palett);
		ClassDB::bind_method(D_METHOD("show_sprite", "show"), &GridBuild::show_sprite);

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

	int GridBuild::get_index_from_feature(int x, int y)
	{
		switch(_feature)
		{
			case 1:
				return _data[x][y].player;
			case 2:
				return _data[x][y].qty;
			case 3:
				return _data[x][y].group;
			case 4:
				return _data[x][y].num_players;
		}
		return 0;
	}

	void GridBuild::build_palett()
	{
		_palett.clear();
		// color feature indicator
		for(int i = 0 ; i < _world_size && _feature != 0; ++ i)
		{
			for(int j = 0 ; j < _world_size; ++ j)
			{
				if(_data[i][j].blocked)
				{
					Vector2i root_l = _data[i][j].root;
					int index_l = get_index_from_feature(root_l.x, root_l.y);
					Color &color_l = _palett[index_l];
					if(color_l.is_equal_approx(Color(0,0,0,1)))
					{
						color_l = pop_color();
						color_l.a *= 0.75;
					}
				}
			}
		}
	}

	Color GridBuild::pop_color()
	{
		static std::list<Color> colors = {
			Color("#696969"),
			Color("#556b2f"),
			Color("#228b22"),
			Color("#7f0000"),
			Color("#483d8b"),
			Color("#b8860b"),
			Color("#008b8b"),
			Color("#000080"),
			Color("#32cd32"),
			Color("#7f007f"),
			Color("#8fbc8f"),
			Color("#b03060"),
			Color("#ff4500"),
			Color("#ff8c00"),
			Color("#ffd700"),
			Color("#00ff00"),
			Color("#00fa9a"),
			Color("#8a2be2"),
			Color("#dc143c"),
			Color("#00ffff"),
			Color("#00bfff"),
			Color("#0000ff"),
			Color("#adff2f"),
			Color("#da70d6"),
			Color("#b0c4de"),
			Color("#ff00ff"),
			Color("#1e90ff"),
			Color("#fa8072"),
			Color("#ff1493"),
			Color("#7b68ee"),
			Color("#ffdead"),
			Color("#ffb6c1")
		};

		Color return_l = colors.front();
		colors.pop_front();
		colors.push_back(return_l);
		return return_l;
	}


} // namespace godot

