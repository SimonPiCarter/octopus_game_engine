#ifndef __CommandPicture__
#define __CommandPicture__

namespace cuttlefish
{
class SpriteModel;
class Text;

/// @brief This class handles basic icon picture
/// plus a progress bar on top to track production
class CommandPicture
{
public:
	enum class Type
	{
		Production,
		AttackMove,
		Stop
	};
	~CommandPicture();

	Type _type { Type::Production };
	SpriteModel * _model {nullptr};
	Text * _text {nullptr};
};

} // namespace cuttlefish


#endif
