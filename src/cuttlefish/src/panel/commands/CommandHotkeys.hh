#ifndef __CommandHotkeys__
#define __CommandHotkeys__

#include <SDL2/SDL.h>

#include <string>
#include <unordered_map>

#include "CommandPicture.hh"


namespace cuttlefish
{

class CommandHotkeys
{
public:

	/// @brief singleton getter
	static CommandHotkeys * GetInstance();

	/// @brief Load entries from a file
	void loadFromFile(std::string const &filename_p);

	void addBuildingModelKeycode(std::string const &model_p, Sint32 const &key_p);
	void addUnitModelKeycode(std::string const &model_p, Sint32 const &key_p);
	void addCommandTypeKeycode(CommandPicture::Type type_p, Sint32 const &key_p);

	Sint32 getKeycode(CommandPicture const&command_p) const;

	static std::string getStringCode(Sint32 const &key_p);

private:
	CommandHotkeys();
	static CommandHotkeys *_instance;

	std::unordered_map<Sint32, bool> _mapUsed;

	std::unordered_map<std::string, Sint32> _keyPerBuildingModel;
	std::unordered_map<std::string, Sint32> _keyPerUnitModel;
	std::unordered_map<CommandPicture::Type, Sint32> _keyPerCommandType;
};

} // namespace cuttlefish


#endif
