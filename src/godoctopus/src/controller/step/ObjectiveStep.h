#ifndef __Godoctopus_ObjectiveStep__
#define __Godoctopus_ObjectiveStep__

#include "step/custom/CustomStep.hh"


namespace godot
{
class Controller;

class AbstractObjectiveStep : public octopus::CustomKeyStep
{
public:
	AbstractObjectiveStep(std::string const &objective_id) : octopus::CustomKeyStep(objective_id) {}

	virtual void add_signals(Controller & controller) const = 0;
};

/// @brief this step adds a new objective
class AddObjectiveStep : public AbstractObjectiveStep
{
public:
    AddObjectiveStep(std::string const &objective_id, std::string const &text, int count, bool main)
        : AbstractObjectiveStep(objective_id), _text(text), _count(count), _main(main)
    {}
    std::string const _text;
	int const _count;
	bool const _main;

	virtual void add_signals(Controller & controller) const final;
};

/// @brief this step removes an objective
class RemoveObjectiveStep : public AbstractObjectiveStep
{
public:
    RemoveObjectiveStep(std::string const &objective_id)
        : AbstractObjectiveStep(objective_id)
    {}

	virtual void add_signals(Controller & controller) const final;
};

/// @brief this step fails an objective
class FailObjectiveStep : public AbstractObjectiveStep
{
public:
    FailObjectiveStep(std::string const &objective_id, bool fail)
        : AbstractObjectiveStep(objective_id), _fail(fail)
    {}
	bool const _fail;

	virtual void add_signals(Controller & controller) const final;
};

/// @brief this step completes an objective
class CompleteObjectiveStep : public AbstractObjectiveStep
{
public:
    CompleteObjectiveStep(std::string const &objective_id, bool complete)
        : AbstractObjectiveStep(objective_id), _complete(complete)
    {}
	bool const _complete;

	virtual void add_signals(Controller & controller) const final;
};

/// @brief this step increment an objective
class IncrementObjectiveStep : public AbstractObjectiveStep
{
public:
    IncrementObjectiveStep(std::string const &objective_id, bool increment)
        : AbstractObjectiveStep(objective_id), _increment(increment)
    {}
	bool const _increment;

	virtual void add_signals(Controller & controller) const final;
};

}

#endif
