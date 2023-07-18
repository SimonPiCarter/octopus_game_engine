#include "CommandSerialization.hh"

#include "controller/Controller.hh"
#include "command/Command.hh"
#include "command/building/BuildingBlueprintCommand.hh"
#include "command/building/BuildingCancelCommand.hh"
#include "command/building/BuildingUnitCancelCommand.hh"
#include "command/building/BuildingRallyPointCommand.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/building/BuildingUpgradeProductionCommand.hh"
#include "command/entity/EntityAbilityCommand.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "command/entity/EntityFlockMoveCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "command/entity/EntityWaitCommand.hh"
#include "command/unit/UnitDropCommand.hh"
#include "command/unit/UnitHarvestCommand.hh"
#include "command/player/PlayerChoseOptionCommand.hh"
#include "library/Library.hh"
#include "state/model/entity/BuildingModel.hh"

#include "logger/Logger.hh"

#include <iostream>

namespace octopus
{

template<typename T>
void write(std::ofstream &file_p, T const &data_p)
{
    file_p.write((char*)&data_p, sizeof(data_p));
}
template<>
void write(std::ofstream &file_p, std::string const &data_p)
{
    write(file_p, data_p.size());
    file_p.write((char*)data_p.c_str(), data_p.size());
}
template<>
void write(std::ofstream &file_p, Handle const &data_p)
{
    write(file_p, data_p.index);
    write(file_p, data_p.revision);
}

struct BinaryWriter
{
    template<typename T>
    void operator()(std::ofstream &file_p, T const &data_p)
    {
        write(file_p, data_p);
    }
};

struct TextWriter
{
    template<typename T>
    void operator()(std::ofstream &file_p, T const &data_p)
    {
        file_p<<data_p<<std::endl;
    }
};

template<>
void TextWriter::operator()(std::ofstream &file_p, Fixed const &data_p)
{
    file_p<<data_p.data()<<std::endl;
}

template<typename T>
void read(std::ifstream &file_p, T *data_p)
{
    file_p.read((char*)data_p, sizeof(*data_p));
}
template<>
void read(std::ifstream &file_p, std::string *data_p)
{
    size_t size_l;
    read(file_p, &size_l);
    data_p->resize(size_l);
    file_p.read(&(*data_p)[0], size_l);
}
template<>
void read(std::ifstream &file_p, Handle *data_p)
{
    read(file_p, &data_p->index);
    read(file_p, &data_p->revision);
}

/// @brief write a command to the file based on the command id
template<typename Writer_t>
void writeCommand(std::ofstream &file_p, Command const *cmd_p, Writer_t writer_p);

/// @brief read a command from the file based on the command id
/// @return a new command to be commited to the controller
Command * readCommand(std::ifstream &file_p, Library const &lib_p);

void writeCommands(std::ofstream &file_p, Controller const &controller_p)
{
    std::vector<std::list<Command *> *> const & commandsPerLevel_l = controller_p.getCommitedCommands();

    // write the number of step
    write(file_p, commandsPerLevel_l.size());
    Logger::getDebug() << ">>nbSteps " << commandsPerLevel_l.size() << std::endl;

    size_t step_l = 0;
    for(std::list<Command *> const * list_l : commandsPerLevel_l)
    {
        // We skip first commands because they must be loaded from context
        // we also skip empty steps to save space
        if(step_l == 0 || list_l->empty())
        {
            ++step_l;
            continue;
        }
        writeListOfCommand(file_p, list_l, step_l);

        ++step_l;
    }
}

void writeListOfCommand(std::ofstream &file_p, std::list<Command *> const * list_p, size_t step_p)
{
    // write the step id
    write(file_p, step_p);
    Logger::getDebug() << ">>step " << step_p << std::endl;
    // write the number of commands for this step
    write(file_p, list_p->size());
    Logger::getDebug() << ">>nbCommands " << list_p->size() << std::endl;

    for(Command const * cmd_l : *list_p)
    {
        writeCommand(file_p, cmd_l, BinaryWriter());
    }

    file_p.flush();
}

void writeDebugListOfCommand(std::ofstream &file_p, std::list<Command *> const * list_p, size_t step_p)
{
    // write the step id
    file_p<<"step : "<<step_p<<std::endl;
    // write the number of commands for this step
    file_p<<"size : "<<list_p->size()<<std::endl;

    for(Command const * cmd_l : *list_p)
    {
        writeCommand(file_p, cmd_l, TextWriter());
    }

    file_p.flush();
}

void readCommands(std::ifstream &file_p, Controller &controller_p, Library const &lib_p)
{

    // read the number of step
    size_t nbSteps_l;
    file_p.read((char*) &nbSteps_l, sizeof(nbSteps_l));

    Logger::getDebug() << "<<nbSteps_l " << nbSteps_l << std::endl;

    int cur_l = file_p.tellg();
    file_p.seekg (0, file_p.end);
    int length_l = file_p.tellg();
    file_p.seekg (cur_l, file_p.beg);

    size_t step_l = 0;
    while(file_p.tellg() < length_l)
    {
        // read the id of this step
        read(file_p, &step_l);

        Logger::getDebug() << "<<step_l "<< step_l << std::endl;

        // read the number of commands for this step
        size_t nbCommands_l;
        read(file_p, &nbCommands_l);

        Logger::getDebug() << "<<nbCommands_l "<< nbCommands_l << std::endl;

        // update controller
        controller_p.setOngoingStep(step_l);

        for(size_t cmd_l = 0 ; cmd_l < nbCommands_l ; ++ cmd_l)
        {
            controller_p.commitCommand(readCommand(file_p, lib_p));
        }
    }

    // update controller
    if(nbSteps_l!=0)
    {
        controller_p.setOngoingStep(std::max(step_l, nbSteps_l-1));
    }
}

template<typename Writer_t>
void writeCommand(std::ofstream &file_p, Command const *cmd_p, Writer_t writer_p)
{
    // write common info
    writer_p(file_p, cmd_p->isQueued());

    if(dynamic_cast<EntityAttackCommand const *>(cmd_p))
    {
        writer_p(file_p, 1ul);
        EntityAttackCommand const *typped_l = dynamic_cast<EntityAttackCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getSource());
        writer_p(file_p, typped_l->getTarget());
        writer_p(file_p, typped_l->isFrozenTarget());
    }
    else if(dynamic_cast<EntityMoveCommand const *>(cmd_p))
    {
        writer_p(file_p, 2ul);
        EntityMoveCommand const *typped_l = dynamic_cast<EntityMoveCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getHandleCommand());
        writer_p(file_p, typped_l->getFinalPoint());
        writer_p(file_p, typped_l->getGridStatus());
        writer_p(file_p, typped_l->getWaypoints().size());
        for(Vector const &vec_l : typped_l->getWaypoints())
        {
            writer_p(file_p, vec_l);
        }
        writer_p(file_p, typped_l->isInit());
        writer_p(file_p, typped_l->isNeverStop());
    }
    else if(dynamic_cast<EntityAttackMoveCommand const *>(cmd_p))
    {
        writer_p(file_p, 3ul);
        EntityAttackMoveCommand const *typped_l = dynamic_cast<EntityAttackMoveCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getSubMoveCommand().getHandleCommand());
        writer_p(file_p, typped_l->getSubMoveCommand().getFinalPoint());
        writer_p(file_p, typped_l->getSubMoveCommand().getGridStatus());
        writer_p(file_p, typped_l->getSubMoveCommand().getWaypoints().size());
        for(Vector const &vec_l : typped_l->getSubMoveCommand().getWaypoints())
        {
            writer_p(file_p, vec_l);
        }
        writer_p(file_p, typped_l->getSubMoveCommand().isInit());
        writer_p(file_p, typped_l->getSubMoveCommand().isNeverStop());
    }
    else if(dynamic_cast<EntityBuildingCommand const *>(cmd_p))
    {
        writer_p(file_p, 4ul);
        EntityBuildingCommand const *typped_l = dynamic_cast<EntityBuildingCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getTarget());
        writer_p(file_p, typped_l->getSubMoveCommand().getHandleCommand());
        writer_p(file_p, typped_l->getSubMoveCommand().getFinalPoint());
        writer_p(file_p, typped_l->getSubMoveCommand().getGridStatus());
        writer_p(file_p, typped_l->getSubMoveCommand().getWaypoints().size());
        for(Vector const &vec_l : typped_l->getSubMoveCommand().getWaypoints())
        {
            writer_p(file_p, vec_l);
        }
        writer_p(file_p, typped_l->getSubMoveCommand().isInit());
    }
    else if(dynamic_cast<EntityWaitCommand const *>(cmd_p))
    {
        writer_p(file_p, 5ul);
        EntityWaitCommand const *typped_l = dynamic_cast<EntityWaitCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getHandleCommand());
    }
    else if(dynamic_cast<BuildingBlueprintCommand const *>(cmd_p))
    {
        writer_p(file_p, 6ul);
        BuildingBlueprintCommand const *typped_l = dynamic_cast<BuildingBlueprintCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getPos());
        writer_p(file_p, typped_l->getPlayer());
        writer_p(file_p, typped_l->getModel()._id);
        writer_p(file_p, typped_l->getBuilders().size());
        for(Handle const &handle_l : typped_l->getBuilders())
        {
            writer_p(file_p, handle_l);
        }
    }
    else if(dynamic_cast<BuildingUnitProductionCommand const *>(cmd_p))
    {
        writer_p(file_p, 7ul);
        BuildingUnitProductionCommand const *typped_l = dynamic_cast<BuildingUnitProductionCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getHandleCommand());
        writer_p(file_p, typped_l->getModel()._id);
    }
    else if(dynamic_cast<UnitDropCommand const *>(cmd_p))
    {
        writer_p(file_p, 8ul);
        UnitDropCommand const *typped_l = dynamic_cast<UnitDropCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getDeposit());
        writer_p(file_p, typped_l->getSubMoveCommand().getHandleCommand());
        writer_p(file_p, typped_l->getSubMoveCommand().getFinalPoint());
        writer_p(file_p, typped_l->getSubMoveCommand().getGridStatus());
        writer_p(file_p, typped_l->getSubMoveCommand().getWaypoints().size());
        for(Vector const &vec_l : typped_l->getSubMoveCommand().getWaypoints())
        {
            writer_p(file_p, vec_l);
        }
        writer_p(file_p, typped_l->getSubMoveCommand().isInit());
    }
    else if(dynamic_cast<UnitHarvestCommand const *>(cmd_p))
    {
        writer_p(file_p, 9ul);
        UnitHarvestCommand const *typped_l = dynamic_cast<UnitHarvestCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getResource());
        writer_p(file_p, typped_l->getSubMoveCommand().getHandleCommand());
        writer_p(file_p, typped_l->getSubMoveCommand().getFinalPoint());
        writer_p(file_p, typped_l->getSubMoveCommand().getGridStatus());
        writer_p(file_p, typped_l->getSubMoveCommand().getWaypoints().size());
        for(Vector const &vec_l : typped_l->getSubMoveCommand().getWaypoints())
        {
            writer_p(file_p, vec_l);
        }
        writer_p(file_p, typped_l->getSubMoveCommand().isInit());
    }
    else if(dynamic_cast<PlayerChoseOptionCommand const *>(cmd_p))
    {
        writer_p(file_p, 10ul);
        PlayerChoseOptionCommand const *typped_l = dynamic_cast<PlayerChoseOptionCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getPlayer());
        writer_p(file_p, typped_l->getKey());
        writer_p(file_p, typped_l->getOption());
    }
    else if(dynamic_cast<BuildingUpgradeProductionCommand const *>(cmd_p))
    {
        writer_p(file_p, 11ul);
        BuildingUpgradeProductionCommand const *typped_l = dynamic_cast<BuildingUpgradeProductionCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getHandleCommand());
        writer_p(file_p, typped_l->getUpgrade()._id);
    }
    else if(dynamic_cast<BuildingUnitCancelCommand const *>(cmd_p))
    {
        writer_p(file_p, 12ul);
        BuildingUnitCancelCommand const *typped_l = dynamic_cast<BuildingUnitCancelCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getHandleCommand());
        writer_p(file_p, typped_l->_idx);
    }
    else if(dynamic_cast<BuildingCancelCommand const *>(cmd_p))
    {
        writer_p(file_p, 13ul);
        BuildingCancelCommand const *typped_l = dynamic_cast<BuildingCancelCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getHandleCommand());
    }
    else if(dynamic_cast<BuildingRallyPointCommand const *>(cmd_p))
    {
        writer_p(file_p, 14ul);
        BuildingRallyPointCommand const *typped_l = dynamic_cast<BuildingRallyPointCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getHandleCommand());
        writer_p(file_p, typped_l->_reset);
        writer_p(file_p, typped_l->_rallyPoint);
        writer_p(file_p, typped_l->_rallyPointEntityActive);
        writer_p(file_p, typped_l->_rallyPointEntity);
    }
    else if(dynamic_cast<EntityFlockMoveCommand const *>(cmd_p))
    {
        writer_p(file_p, 15ul);
        EntityFlockMoveCommand const *typped_l = dynamic_cast<EntityFlockMoveCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getHandles().size());
        for(Handle const &handle_p : typped_l->getHandles())
        {
            writer_p(file_p, handle_p);
        }
        writer_p(file_p, typped_l->getFinalPoint());
        writer_p(file_p, typped_l->isAttackMove());
        writer_p(file_p, typped_l->isNeverStop());
    }
    else if(dynamic_cast<EntityAbilityCommand const *>(cmd_p))
    {
        writer_p(file_p, 16ul);
        EntityAbilityCommand const *typped_l = dynamic_cast<EntityAbilityCommand const *>(cmd_p);
        writer_p(file_p, typped_l->getHandleCommand());
        writer_p(file_p, typped_l->_target);
        writer_p(file_p, typped_l->_pointTarget);
        writer_p(file_p, typped_l->_id);
    }
    else
    {
        throw std::logic_error("unserializable command thrown in file");
    }
}

Command * readCommand(std::ifstream &file_p, Library const &lib_p)
{
    Command * cmd_l = nullptr;
    bool queued_l;
    unsigned long cmdId_p;

    // read common info
    read(file_p, &queued_l);
    read(file_p, &cmdId_p);

    Logger::getDebug() << "command id "<<cmdId_p<<std::endl;

    if(cmdId_p == 1)
    {
        Handle source_l;
        Handle target_l;
        bool frozenTarget_l;

        read(file_p, &source_l);
        read(file_p, &target_l);
        read(file_p, &frozenTarget_l);

        cmd_l = new EntityAttackCommand(source_l, source_l, target_l, frozenTarget_l);
    }
    else if(cmdId_p == 2)
    {
        Handle source_l;
        Vector final_l;
        unsigned long gridStatus_l;
        size_t nbPoints_l;
        std::list<Vector> points_l;
        bool init_l;
        bool neverStop_l;

        read(file_p, &source_l);
        read(file_p, &final_l);
        read(file_p, &gridStatus_l);
        read(file_p, &nbPoints_l);
        for(size_t i = 0 ; i < nbPoints_l ; ++i)
        {
            Vector vec_l;
            read(file_p, &vec_l);
            points_l.push_back(vec_l);
        }
        read(file_p, &init_l);
        read(file_p, &neverStop_l);

        cmd_l = new EntityMoveCommand(source_l, source_l, final_l, gridStatus_l, points_l, init_l, neverStop_l);
    }
    else if(cmdId_p == 3)
    {
        Handle source_l;
        Vector final_l;
        unsigned long gridStatus_l;
        size_t nbPoints_l;
        std::list<Vector> points_l;
        bool init_l;
        bool neverStop_l;

        read(file_p, &source_l);
        read(file_p, &final_l);
        read(file_p, &gridStatus_l);
        read(file_p, &nbPoints_l);
        for(size_t i = 0 ; i < nbPoints_l ; ++i)
        {
            Vector vec_l;
            read(file_p, &vec_l);
            points_l.push_back(vec_l);
        }
        read(file_p, &init_l);
        read(file_p, &neverStop_l);

        cmd_l = new EntityAttackMoveCommand(source_l, source_l, final_l, gridStatus_l, points_l, init_l, neverStop_l);
    }
    else if(cmdId_p == 4)
    {
        Handle target_l;
        Handle source_l;
        Vector final_l;
        unsigned long gridStatus_l;
        size_t nbPoints_l;
        std::list<Vector> points_l;
        bool init_l;

        read(file_p, &target_l);
        read(file_p, &source_l);
        read(file_p, &final_l);
        read(file_p, &gridStatus_l);
        read(file_p, &nbPoints_l);
        for(size_t i = 0 ; i < nbPoints_l ; ++i)
        {
            Vector vec_l;
            read(file_p, &vec_l);
            points_l.push_back(vec_l);
        }
        read(file_p, &init_l);

        cmd_l = new EntityBuildingCommand(source_l, source_l, target_l, final_l, gridStatus_l, points_l, init_l);
    }
    else if(cmdId_p == 5)
    {
        Handle source_l;

        read(file_p, &source_l);

        cmd_l = new EntityWaitCommand(source_l, source_l);
    }
    else if(cmdId_p == 6)
    {
        Vector pos_l;
        unsigned long player_l;
        std::string id_l;
        size_t size_l;
        std::vector<Handle> builders_l;

        read(file_p, &pos_l);
        read(file_p, &player_l);
        read(file_p, &id_l);

        read(file_p, &size_l);
        for(size_t i = 0 ; i < size_l ; ++ i)
        {
            Handle handle_l;
            read(file_p, &handle_l);
            builders_l.push_back(handle_l);
        }

        cmd_l = new BuildingBlueprintCommand(pos_l, player_l, lib_p.getBuildingModel(id_l), builders_l);
    }
    else if(cmdId_p == 7)
    {
        Handle source_l;
        std::string id_l;

        read(file_p, &source_l);
        read(file_p, &id_l);

        cmd_l = new BuildingUnitProductionCommand(source_l, source_l, lib_p.getUnitModel(id_l));
    }
    else if(cmdId_p == 8)
    {
        Handle deposit_l;
        Handle source_l;
        Vector final_l;
        unsigned long gridStatus_l;
        size_t nbPoints_l;
        std::list<Vector> points_l;
        bool init_l;

        read(file_p, &deposit_l);
        read(file_p, &source_l);
        read(file_p, &final_l);
        read(file_p, &gridStatus_l);
        read(file_p, &nbPoints_l);
        for(size_t i = 0 ; i < nbPoints_l ; ++i)
        {
            Vector vec_l;
            read(file_p, &vec_l);
            points_l.push_back(vec_l);
        }
        read(file_p, &init_l);

        cmd_l = new UnitDropCommand(source_l, source_l, deposit_l, final_l, gridStatus_l, points_l, init_l);
    }
    else if(cmdId_p == 9)
    {
        Handle res_l;
        Handle source_l;
        Vector final_l;
        unsigned long gridStatus_l;
        size_t nbPoints_l;
        std::list<Vector> points_l;
        bool init_l;

        read(file_p, &res_l);
        read(file_p, &source_l);
        read(file_p, &final_l);
        read(file_p, &gridStatus_l);
        read(file_p, &nbPoints_l);
        for(size_t i = 0 ; i < nbPoints_l ; ++i)
        {
            Vector vec_l;
            read(file_p, &vec_l);
            points_l.push_back(vec_l);
        }
        read(file_p, &init_l);

        cmd_l = new UnitHarvestCommand(source_l, source_l, res_l, final_l, gridStatus_l, points_l, init_l);
    }
    else if(cmdId_p == 10)
    {
        unsigned long player_l;
        std::string key_l;
        unsigned long option_l;

        read(file_p, &player_l);
        read(file_p, &key_l);
        read(file_p, &option_l);

        cmd_l = new PlayerChoseOptionCommand(player_l, key_l, option_l);
    }
    else if(cmdId_p == 11)
    {
        Handle source_l;
        std::string id_l;

        read(file_p, &source_l);
        read(file_p, &id_l);

        cmd_l = new BuildingUpgradeProductionCommand(source_l, source_l, lib_p.getUpgrade(id_l));
    }
    else if(cmdId_p == 12)
    {
        Handle building_l;
        unsigned long idx_l;

        read(file_p, &building_l);
        read(file_p, &idx_l);

        cmd_l = new BuildingUnitCancelCommand(building_l, idx_l);
    }
    else if(cmdId_p == 13)
    {
        Handle building_l;

        read(file_p, &building_l);

        cmd_l = new BuildingCancelCommand(building_l);
    }
    else if(cmdId_p == 14)
    {
        Handle building_l;
        bool reset_l;
        Vector rallyPoint_l;
        bool rallyPointEntityActive_l;
        Handle rallyPointEntity_l;

        read(file_p, &building_l);
        read(file_p, &reset_l);
        read(file_p, &rallyPoint_l);
        read(file_p, &rallyPointEntityActive_l);
        read(file_p, &rallyPointEntity_l);

        if(reset_l)
        {
            cmd_l = new BuildingRallyPointCommand(building_l);
        }
        else
        {
            cmd_l = new BuildingRallyPointCommand(building_l, rallyPoint_l, rallyPointEntityActive_l, rallyPointEntity_l);
        }
    }
    else if(cmdId_p == 15)
    {
        size_t size_l;
        std::list<Handle> handles_l;
        Vector point_l;
        bool attackMove_l;
        bool neverStop_l;

        read(file_p, &size_l);
        for(size_t i = 0 ; i < size_l ; ++i)
        {
            Handle handle_l;
            read(file_p, &handle_l);
            handles_l.push_back(handle_l);
        }
        read(file_p, &point_l);
        read(file_p, &attackMove_l);
        read(file_p, &neverStop_l);

        cmd_l = new EntityFlockMoveCommand(handles_l, point_l, neverStop_l);
    }
    else if(cmdId_p == 16)
    {
        Handle handle_l;
        Handle target_l;
        Vector pointTarget_l;
        std::string id_l;

        read(file_p, &handle_l);
        read(file_p, &target_l);
        read(file_p, &pointTarget_l);
        read(file_p, &id_l);

        cmd_l = new EntityAbilityCommand(handle_l, target_l, pointTarget_l, id_l);
    }
    if(!cmd_l)
    {
        throw std::logic_error("unserializable command got from file");
    }
    cmd_l->setQueued(queued_l);
    return cmd_l;
}

} // namespace octopus
