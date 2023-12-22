#pragma once

#include <map>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>

namespace godot {

class Controller;

struct GridBuildData {
	// if the case is highlighted
	bool highlighted = false;

	// if the case is available or not
	bool blocked = false;

	// index of the model in this case
	int index = -1;

	// the player of the model
	int player = 0;
	// the qty of entities
	int qty = 1;
	// the group to which this entity belongs
	int group = -1;
	// the minimal number of player to spawn this entity
	int num_players = 0;

	Texture2D* texture = nullptr;
	Vector2 offset;

	/// @brief coordinate of the root case if this case is blocked but has non texture/index
	Vector2i root;
};

class GridBuild : public Node2D {
	GDCLASS(GridBuild, Node2D)

public:
	void set_world_size(int size_p) { _world_size = size_p; }
	int get_world_size() const { return _world_size; }

	void set_textures(AtlasTexture *standard_p, AtlasTexture *highlighted_p, AtlasTexture *blocked_p);
	void reset();
	// get an array with all the coordinate of rooted entities
	TypedArray<Vector2i> get_root_coord();

	void set_feature(int feature_p) { _feature = feature_p; build_palett(); }
	TypedArray<Color> get_color_palett();
	TypedArray<int> get_labels_palett();
	void show_sprite(bool show_p) { _show_sprite = show_p; }

	void _ready() override;
	void _process(double delta_p) override;
	void _draw() override;

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

	///////////////////
	// Case edition  //
	///////////////////
	void set_highlighted(int x, int y, bool highlighted_p) { _data[x][y].highlighted = highlighted_p; }
	bool get_highlighted(int x, int y) { return _data[x][y].highlighted; }

	void set_blocked(int x, int y, bool blocked_p) { _data[x][y].blocked = blocked_p; }
	bool get_blocked(int x, int y) { return _data[x][y].blocked; }

	void set_index(int x, int y, int index_p) { _data[x][y].index = index_p; }
	int get_index(int x, int y) { return _data[x][y].index; }

	void set_player(int x, int y, int player_p) { _data[x][y].player = player_p; }
	int get_player(int x, int y) { return _data[x][y].player; }

	void set_qty(int x, int y, int qty_p) { _data[x][y].qty = qty_p; }
	int get_qty(int x, int y) { return _data[x][y].qty; }

	void set_group(int x, int y, int group_p) { _data[x][y].group = group_p; }
	int get_group(int x, int y) { return _data[x][y].group; }

	void set_num_players(int x, int y, int num_players_p) { _data[x][y].num_players = num_players_p; }
	int get_num_players(int x, int y) { return _data[x][y].num_players; }

	void set_texture(int x, int y, Texture2D *texture_p) { _data[x][y].texture = texture_p; }
	Texture2D* get_texture(int x, int y) { return _data[x][y].texture; }

	void set_offset(int x, int y, Vector2 const &offset_p) { _data[x][y].offset = offset_p; }
	Vector2 const & get_offset(int x, int y) { return _data[x][y].offset; }

	void set_root(int x, int y, Vector2i const &root_p) { _data[x][y].root = root_p; }
	Vector2i const & get_root(int x, int y) { return _data[x][y].root; }

private:
	/// @brief reset the data
	void update_data();
	/// @brief get the index ofthe case based from the feature
	int get_index_from_feature(int x, int y);
	/// @brief build palett
	void build_palett();
	/// @brief cycle between 32 colors
	Color pop_color();

	/// base parameters
	int _size_case = 32;
	int _world_size = 250;
	AtlasTexture * _standard = nullptr;
	AtlasTexture * _highlighted = nullptr;
	AtlasTexture * _blocked = nullptr;

	/// @brief data
	std::vector<std::vector<GridBuildData> > _data;

	/// @brief colors to cycle through
	std::map<int, Color> _palett;

	/// @brief feature to display color palett from
	/// 0 : none
	/// 1 : player
	/// 2 : qty
	/// 3 : group
	/// 4 : num players
	int _feature = 0;

	/// @brief tell if we need to show sprites
	bool _show_sprite = true;
};

}
;