#include <gtest/gtest.h>

#include "step/unit/UnitDataStep.hh"
#include "state/State.hh"
#include "state/entity/Unit.hh"
#include "state/player/Player.hh"

///
/// This test suite aims at checking that EntityMoveStep works properly
/// - Move an entity in a step
/// - Is correctly undone
///

using namespace octopus;

struct UnitDataTest : public UnitData
{
	UnitDataTest(int val_p) : value(val_p) {}
	virtual UnitData* clone() const override
	{
		return new UnitDataTest(value);
	}
	virtual void copyFrom(UnitData const *other_p) override
	{
		UnitDataTest const * data_l = dynamic_cast<UnitDataTest const *>(other_p);
		value = data_l->value;
	}

	int value = 0;
};

TEST(unitDataTest, simple)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	octopus::UnitModel unitModel_l { false, 1, 1, 10. };
	UnitDataTest * data_l = new UnitDataTest(12);
	unitModel_l._unitData = data_l;
	Unit * unit_l = new Unit { { 3, 3. }, false, unitModel_l};
	state_l.addEntity(unit_l);

	data_l = new UnitDataTest(25);
	UnitDataStep step_l(Handle(0), data_l);

	SteppableData * memento_l = step_l.newData(state_l);

	UnitDataTest * unitData_l = dynamic_cast<UnitDataTest *>(unit_l->_unitData.get());
	EXPECT_NE(nullptr, unitData_l);
	EXPECT_EQ(12, unitData_l->value);

	step_l.apply(state_l);

	unitData_l = dynamic_cast<UnitDataTest *>(unit_l->_unitData.get());
	EXPECT_NE(nullptr, unitData_l);
	EXPECT_EQ(25, unitData_l->value);

	step_l.revert(state_l, memento_l);

	unitData_l = dynamic_cast<UnitDataTest *>(unit_l->_unitData.get());
	EXPECT_NE(nullptr, unitData_l);
	EXPECT_EQ(12, unitData_l->value);

	delete memento_l;
}

TEST(unitDataTest, double)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	octopus::UnitModel unitModel_l { false, 1, 1, 10. };
	UnitDataTest * data_l = new UnitDataTest(12);
	unitModel_l._unitData = data_l;
	Unit * unit_l = new Unit { { 3, 3. }, false, unitModel_l};
	state_l.addEntity(unit_l);

	data_l = new UnitDataTest(25);
	UnitDataStep step1_l(Handle(0), data_l);
	SteppableData * memento1_l = step1_l.newData(state_l);

	// step 1
	{
		UnitDataTest * unitData_l = dynamic_cast<UnitDataTest *>(unit_l->_unitData.get());
		EXPECT_NE(nullptr, unitData_l);
		EXPECT_EQ(12, unitData_l->value);

		step1_l.apply(state_l);

		unitData_l = dynamic_cast<UnitDataTest *>(unit_l->_unitData.get());
		EXPECT_NE(nullptr, unitData_l);
		EXPECT_EQ(25, unitData_l->value);

		// step2
		{
			data_l = new UnitDataTest(37);
			UnitDataStep step2_l(Handle(0), data_l);
			SteppableData * memento2_l = step2_l.newData(state_l);

			step2_l.apply(state_l);

			unitData_l = dynamic_cast<UnitDataTest *>(unit_l->_unitData.get());
			EXPECT_NE(nullptr, unitData_l);
			EXPECT_EQ(37, unitData_l->value);

			step2_l.revert(state_l, memento2_l);

			unitData_l = dynamic_cast<UnitDataTest *>(unit_l->_unitData.get());
			EXPECT_NE(nullptr, unitData_l);
			EXPECT_EQ(25, unitData_l->value);

			delete memento2_l;
		}

		step1_l.revert(state_l, memento1_l);

		unitData_l = dynamic_cast<UnitDataTest *>(unit_l->_unitData.get());
		EXPECT_NE(nullptr, unitData_l);
		EXPECT_EQ(12, unitData_l->value);

		delete memento1_l;
	}

}
