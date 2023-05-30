#ifndef __Godoctopus_Controller__
#define __Godoctopus_Controller__

#include <cstdint>
#include <fstream>
#include <list>
#include <map>
#include <thread>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "library/Library.hh"
#include "utils/RandomGenerator.hh"
#include "controller/Controller.hh"
#include "option/Option.h"
#include "option/OptionManager.h"
#include "LevelModel.h"

namespace octopus
{
    class Command;
    class Steppable;
    class Entity;
    class Player;
    class State;
} // namespace octopus


namespace godot {

class Controller : public Node {
    GDCLASS(Controller, Node)

public:
    ~Controller();

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    // demo levels
    void load_wave_level(int playerWave_p, int stepCount_p=3*60*100);
    void load_arena_level(TypedArray<int> const &size_you_p, TypedArray<int> const &size_them_p, TypedArray<String> const &model_you_p, TypedArray<String> const &model_them_p);
    void load_kamikaze_level(int you_p, int them_p, bool fast_p);
    void load_maze_level(int size_p);
    // showcase level
    void load_aoe_level(int size_p);
    void load_chaining_level();
    void load_dot_level(int size_p);
    void load_lifesteal_level(int size_p);

    // levels
    void load_level1(int seed_p, int nb_wave_p);
    void load_level2(int seed_p);
    void load_level_test_anchor(int seed_p);
    void load_level_test_model(int seed_p, godot::LevelModel *level_model_p);

    // replay
    void replay_level(String const &filename_p, bool replay_mode_p);

    // start engine with given level
    void init(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p, size_t size_p=50, std::ofstream *file_p=nullptr);
    void init_replay(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p, size_t size_p, std::ifstream &file_p);
    void init_loading(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p, size_t size_p, std::ifstream &file_p);
    void loading_loop();
    void loop();

    bool has_state() const;

    void _process(double delta) override;

    octopus::Entity const * getEntity(int handle_p) const;
    octopus::Player const * getPlayer(int player_p) const;

    // signals
	void spawn(int handle_p);
	void kill(int handle_p);
	void move(int handle_p);
	void windup(int handle_p);
	void hp_change(int handle_p, float ratio_p);

    // setters
    void set_pause(bool paused_p);
    void set_over(bool over_p);
    // getters
    TypedArray<String> get_models(int handle_p, int player_p) const;
    bool is_building(String const &model_p) const;
    int get_world_size() const;
    int get_steps() const;
    int get_team(int player_p) const;

    // resources getter
	float get_res(String const &res_p, int player_p) const;

    // vision getter
    bool is_visible(int x, int y, int player_p) const;
    bool is_unit_visible(int handle_p, int player_p) const;
    bool is_explored(int x, int y, int player_p) const;
    bool is_entity_explored(int explored_p, int player_p) const;
    PackedByteArray getVisibility(int player_p) const;

    // option getter
    int get_nb_options_available(int player_p) const;
    int get_nb_options_chosen(int player_p) const;
    godot::Option *get_available_option_you(int idx_p, int player_p) const;
    godot::Option *get_available_option_them(int idx_p, int player_p) const;
    godot::Option *get_chosen_option_you(int idx_p, int player_p) const;
    godot::Option *get_chosen_option_them(int idx_p, int player_p) const;

    // signal emmiter

    /// @brief will emit one signal per production to create
    void get_productions(TypedArray<int> const &handles_p, int max_p);

    /// @brief will emit one signal per unit to be shown or hidden
    void get_visible_units(int player_p, int ent_registered_p);

    // commands
    // move & attack
    void add_move_commands(int peer, TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p, bool queued_p);
    void add_move_target_commands(int peer, TypedArray<int> const &handles_p, Vector2 const &target_p, int handleTarget_p, int player_p, bool queued_p);
    void add_attack_move_commands(int peer, TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p, bool queued_p);
    void add_stop_commands(int peer, TypedArray<int> const &handles_p, int player_p, bool queued_p);
    // production
    void add_unit_build_command(int peer, TypedArray<int> const &handles_p, String const &model_p, int player_p);
    void add_unit_build_cancel_command(int peer, int handle_p, int index_p, int player_p);
    // building
    void add_blueprint_command(int peer_p, Vector2 const &target_p, String const &model_p, int player_p, TypedArray<int> const &builders_p);
    void add_building_cancel_command(int peer_p, int handle_p, int player_p);
    // option
    void add_chose_option_command(int peer_p, int option_p, int player_p);

    // step
    /// @brief set the number of queued layer to be setup
    void set_step_control(int prequeued_p);
    /// @brief allow controller to proceed to next step while adding a queued layer
    void next_step();
    /// @brief get the size of the queued layers
    int get_queued_size() const;

    // handling of peer information
    /// @brief add peer info on player
    void add_peer_info(int peer_p, int player_p);
    /// @brief register that peer is done with current queue
    /// will add a new queue layer for the given peer
    void step_done_for_peer(int peer_p);

    ////////////////////////////
    //// Dump methods
    ////////////////////////////

    // auto saving during game
    void set_auto_file_path(String const &path_p);
    void set_auto_file_debug(bool debug_p);
    // dump state as plain text
    void dump_state_as_text(String const &path_p);


    ////////////////////////////
    //// Non godot methods
    ////////////////////////////

    std::map<unsigned long, OptionManager> &getOptionManagers();
    std::map<unsigned long, OptionManager> const &getOptionManagers() const;

    octopus::Library &getLib();
private:
    void newAutoSaveFile();
    octopus::Controller * _controller = nullptr;
    bool _initDone = false;
    std::thread * _controllerThread = nullptr;
    std::string _autoSavePath = "autosave.fas";
    std::ofstream * _autoSaveFile = nullptr;
    std::ofstream * _autoSaveFileDebug = nullptr;
    bool _enableAutoSaveFileDebug = false;
	octopus::Library _lib;
	octopus::RandomGenerator * _rand = nullptr;

    octopus::State const * _state = nullptr;

    bool _over = false;
    bool _paused = false;

	std::list<octopus::StepBundle>::const_iterator _lastIt;

    /// @brief vector storing last visible units when call of get_visible_units
    std::vector<bool> _visibleLastCall;

    /// @brief manager for each player
    std::map<unsigned long, OptionManager> _optionManagers;

    /// @brief if set to true controller loop will be capped by _stepDone
    bool _stepControl = true;
    /// @brief idx of last complete step so the controller can step by
    int _stepDone = 0;

    /// @brief queued command per peer
    std::map<int, std::list<std::list<octopus::Command*> > > _queuedCommandsPerPeer;
    /// @brief player per peer
    std::map<int, unsigned long> _playerPerPeer;
};

}

#endif //__Godoctopus_Controller__
