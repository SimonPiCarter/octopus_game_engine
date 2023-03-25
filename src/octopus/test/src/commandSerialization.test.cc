#include <gtest/gtest.h>

#include <fstream>
#include <cstdio>

#include "command/building/BuildingBlueprintCommand.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "controller/Controller.hh"
#include "library/Library.hh"
#include "serialization/CommandSerialization.hh"
#include "state/model/entity/BuildingModel.hh"

using namespace octopus;

// required to use EXPECT_EQ with on stack vector creation
std::ostream &operator<<(std::ostream &os_p, octopus::Vector const vec_p)
{
	return os_p<<"Vector[x="<<vec_p.x<<", y="<<vec_p.y<<"]";
}

TEST(commandSerializationTest, simple_no_step_attackCommand)
{
    Library lib_l;
    std::string fileName_l = "commandSerializationTest.save";
    // Writing
    {
        // write controller
        Controller controller_l({}, 1.);

        controller_l.commitCommand(new EntityAttackCommand(0,0,1,true));
        controller_l.commitCommand(new EntityAttackCommand(1,1,0,false));


        std::ofstream file_l(fileName_l, std::ios::out | std::ios::binary);
        writeCommands(file_l, controller_l);
    }

    // Reading
    {
        Controller controller_l({}, 1.);

        std::ifstream ifile_l(fileName_l, std::ios::in | std::ios::binary);
        readCommands(ifile_l, controller_l, lib_l);

        ASSERT_EQ(2, controller_l.getCommitedCommands().size());
        ASSERT_EQ(0, controller_l.getCommitedCommands()[0]->size());
        ASSERT_EQ(2, controller_l.getCommitedCommands()[1]->size());

        auto &&it_l = controller_l.getCommitedCommands()[1]->begin();
        Command const *cmd_l = *(it_l++);
        // check command 1
        EntityAttackCommand const * typped_l = dynamic_cast<EntityAttackCommand const *>(cmd_l);
        ASSERT_NE(nullptr, typped_l);
        EXPECT_EQ(0, typped_l->getSource());
        EXPECT_EQ(1, typped_l->getTarget());
        EXPECT_EQ(true, typped_l->isFrozenTarget());

        // check command 2
        cmd_l = *(it_l++);
        typped_l = dynamic_cast<EntityAttackCommand const *>(cmd_l);
        ASSERT_NE(nullptr, typped_l);
        EXPECT_EQ(1, typped_l->getSource());
        EXPECT_EQ(0, typped_l->getTarget());
        EXPECT_EQ(false, typped_l->isFrozenTarget());
    }
    // remove file
    std::remove(fileName_l.c_str());
}

TEST(commandSerializationTest, simple_no_step_moveCommand)
{
    Library lib_l;
    std::string fileName_l = "commandSerializationTest.save";
    // Writing
    {
        // write controller
        Controller controller_l({}, 1.);

        controller_l.commitCommand(new EntityMoveCommand(0,0,{12,3},0,{{12,3}}, false));
        controller_l.commitCommand(new EntityMoveCommand(1,1,{15,2},1,{{7,8}, {15,2}}, true));

        std::ofstream file_l(fileName_l, std::ios::out | std::ios::binary);
        writeCommands(file_l, controller_l);
    }

    // Reading
    {
        Controller controller_l({}, 1.);

        std::ifstream ifile_l(fileName_l, std::ios::in | std::ios::binary);
        readCommands(ifile_l, controller_l, lib_l);

        ASSERT_EQ(2, controller_l.getCommitedCommands().size());
        ASSERT_EQ(0, controller_l.getCommitedCommands()[0]->size());
        ASSERT_EQ(2, controller_l.getCommitedCommands()[1]->size());

        auto &&it_l = controller_l.getCommitedCommands()[1]->begin();
        Command const *cmd_l = *(it_l++);
        // check command 1
        EntityMoveCommand const * typped_l = dynamic_cast<EntityMoveCommand const *>(cmd_l);
        ASSERT_NE(nullptr, typped_l);
        EXPECT_EQ(0, typped_l->getHandleCommand());
        EXPECT_EQ(Vector(12, 3), typped_l->getFinalPoint());
        EXPECT_EQ(0, typped_l->getGridStatus());
        ASSERT_EQ(1u, typped_l->getWaypoints().size());
        EXPECT_EQ(Vector(12, 3), typped_l->getWaypoints().front());
        EXPECT_EQ(false, typped_l->isInit());

        // check command 2
        cmd_l = *(it_l++);
        typped_l = dynamic_cast<EntityMoveCommand const *>(cmd_l);
        ASSERT_NE(nullptr, typped_l);
        EXPECT_EQ(1, typped_l->getHandleCommand());
        EXPECT_EQ(Vector(15, 2), typped_l->getFinalPoint());
        EXPECT_EQ(1, typped_l->getGridStatus());
        ASSERT_EQ(2u, typped_l->getWaypoints().size());
        EXPECT_EQ(Vector(7, 8), typped_l->getWaypoints().front());
        EXPECT_EQ(Vector(15, 2), typped_l->getWaypoints().back());
        EXPECT_EQ(true, typped_l->isInit());
    }
    // remove file
    std::remove(fileName_l.c_str());
}

TEST(commandSerializationTest, simple_no_step_buildngBlueprint)
{
    Library lib_l;
    BuildingModel model_l(true, 1, 10);
    lib_l.registerBuildingModel("model", model_l);
    std::string fileName_l = "commandSerializationTest.save";
    // Writing
    {
        // write controller
        Controller controller_l({}, 1.);

        controller_l.commitCommand(new BuildingBlueprintCommand({1, 2}, 3, lib_l.getBuildingModel("model")));

        std::ofstream file_l(fileName_l, std::ios::out | std::ios::binary);
        writeCommands(file_l, controller_l);
    }

    // Reading
    {
        Controller controller_l({}, 1.);

        std::ifstream ifile_l(fileName_l, std::ios::in | std::ios::binary);
        readCommands(ifile_l, controller_l, lib_l);

        ASSERT_EQ(2, controller_l.getCommitedCommands().size());
        ASSERT_EQ(0, controller_l.getCommitedCommands()[0]->size());
        ASSERT_EQ(1, controller_l.getCommitedCommands()[1]->size());

        auto &&it_l = controller_l.getCommitedCommands()[1]->begin();
        Command const *cmd_l = *(it_l++);
        // check command 1
        BuildingBlueprintCommand const * typped_l = dynamic_cast<BuildingBlueprintCommand const *>(cmd_l);
        ASSERT_NE(nullptr, typped_l);
        EXPECT_EQ(Vector(1, 2), typped_l->getPos());
        EXPECT_EQ(3, typped_l->getPlayer());
        // compare ptrs here
        EXPECT_EQ(&lib_l.getBuildingModel("model"), &typped_l->getModel());
    }
    // remove file
    std::remove(fileName_l.c_str());
}
