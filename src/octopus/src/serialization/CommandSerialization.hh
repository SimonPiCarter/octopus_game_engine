#ifndef __CommandWriter__
#define __CommandWriter__

#include <fstream>
#include <list>

namespace octopus
{
class Controller;
class Library;
class Command;

/// @brief This class writes a all commands commited by step
/// in this controller
/// A call to this writer is usually preceeded by a specific
/// writer to load the info of the appropriate level (to be enable to reload the initial steps and command + setup of controller)
///
/// Content is writteer as follow
/// - nb of steps
/// - for each step
///     - nb of commands
///     - for each command
///         - command id
///         - commands content
///
void writeCommands(std::ofstream &file_p, Controller const &controller_p);

/// @brief write a lifst of command to the file
/// @param file_p
/// @param list_p
void writeListOfCommand(std::ofstream &file_p, std::list<Command *> const * list_p, size_t step_l);

/// @brief Same ad writeListOfCommand but write plain text instead of binary
void writeDebugListOfCommand(std::ofstream &file_p, std::list<Command *> const * list_p, size_t step_l);

/// @brief Read commands from a file
/// @param file_p
/// @param controller_p
/// @param lib_p
void readCommands(std::ifstream &file_p, Controller &controller_p, Library const &lib_p);

} // namespace octopus


#endif