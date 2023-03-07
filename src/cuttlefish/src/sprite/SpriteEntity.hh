#ifndef __SpriteEntity__
#define __SpriteEntity__

#include "Sprite.hh"
#include "progressBar/ProgressBar.hh"

#include "state/Handle.hh"

namespace cuttlefish
{

/// @brief sprite used to render an entity in the world based on  Sprite
class SpriteEntity : public Sprite
{
	public:
		SpriteEntity(octopus::Handle const &ent_p, Texture const * texture_p, Picture *background_p, Picture *bar_p, double scale_p, int logX_p, int logY_p, int width_p, int height_p,
			std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, bool absolute_p=false);
		~SpriteEntity();

		octopus::Handle const & getHandle() const;

		void renderLifeBar(Window &window_p, double elapsed_p);

		void setLifePercent(int percent_p);

		bool hasDyingState() const;
		void setDyingState(unsigned long state_p);

		unsigned long getDyingState() const;

	private:
		/// @brief the handle of the entity displayed
		octopus::Handle const _handle;

		ProgressBar * _lifeBar;

		unsigned long _dyingState {0};
		bool _hasDyingState {false};
};

} // namespace cuttlefish

#endif
